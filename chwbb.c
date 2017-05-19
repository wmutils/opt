/* See LICENSE file for copyright and license details. */

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "arg.h"
#include "util.h"

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))

static xcb_connection_t *conn;

static void usage(char *name);
static void set_border(long, xcb_window_t);
static unsigned long multiplycolor(unsigned long col, double fact);

static const double bevelfacts[]    = { 1.25, 1.0, 0.5 }; /* multiply outer/middle/inner bordercolor with x */
static unsigned int borders[] = { 2, 4, 2 };  /* outer, middle, inner border */
static long colbase, colaccent, colflat;

static xcb_screen_t *scr;

unsigned long
multiplycolor(unsigned long col, double fact) {
	int r = (int)(((0xFF0000 & col) >> 16) * fact);
	int g = (int)(((0x00FF00 & col) >>  8) * fact);
	int b = (int)(((0x0000FF & col) >>  0) * fact);
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b;
	return (0xFF000000 & col) | (r << 16) | (g <<  8) | (b <<  0);
}

static void
usage(char *name)
{
	fprintf(stderr, "usage: %s <-sc ...> <wid> [wid...]\n", name);
	exit(1);
}

static void
set_border(long color, xcb_window_t win)
{
	uint32_t values[1];
	short w, h, b, x, y, z;

	xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(conn,
			xcb_get_geometry(conn, win), NULL);

	if (geom == NULL)
		return;


	w = (short)geom->width;
	h = (short)geom->height;
	x = (short)borders[2];
	y = (short)borders[1];
	z = (short)borders[0];
	b = (unsigned short)x+y+z;

	xcb_rectangle_t base[] = {
		{0, h+x,w+x+y,y},
		{w+x, 0, y,h+x},
		{0,h+b+z,w+x+y,y},
		{w+x,h+b+z+y,y,x},
		{w+b+z,0,y,h+x+y},
		{w+b+z+y,h+x,x,y},
		{w+b+z, h+b+z,y+x,y},
		{w+b+z,h+b+z+y,y,x},
	};

	xcb_rectangle_t accent[] = {
		{w,0,x,h},
		{0,h,w+x,x},
		{0,h+b,w+2*b,z},
		{w+b,0,z,h+2*b},
	};

	xcb_rectangle_t flat[] = {
		{0,h+x+y,w+b,z},
		{w+x+y,0,z,h+y+x},
		{0,(h+2*b)-x,w+x,x},
		{w+x+y,h+b+z,z,x+y},
		{w+b+z+y,0,x,h+x},
		{w+b+z,h+x+y,y+x,z},
		{(w+2*b)-x,(h+2*b)-x,x,x},
	};

	xcb_pixmap_t pmap = xcb_generate_id(conn);
	xcb_create_pixmap(conn, scr->root_depth, pmap, win,
			geom->width  + (b*2),
			geom->height + (b*2));

	xcb_gcontext_t gc = xcb_generate_id(conn);
	xcb_create_gc(conn, gc, pmap, 0, NULL);

	values[0] = colbase;
	xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, values);
	xcb_poly_fill_rectangle(conn, pmap, gc, 8, base);

	values[0] = colaccent;
	xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, values);
	xcb_poly_fill_rectangle(conn, pmap, gc, 4, accent);

	values[0] = colflat;
	xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, values);
	xcb_poly_fill_rectangle(conn, pmap, gc, 7, flat);

	values[0] = pmap;
	xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXMAP, values);

	xcb_free_pixmap(conn, pmap);
	xcb_free_gc(conn, gc);
}

int
main(int argc, char **argv)
{
	char *argv0;
	int color;

	color = -1;

	if (argc < 2)
		usage(argv[0]);

	ARGBEGIN {
		case 'i':
			borders[2] = strtoul(EARGF(usage(argv0)), NULL, 10);
			break;
		case 'o':
			borders[0] = strtoul(EARGF(usage(argv0)), NULL, 10);
			break;
		case 'm':
			borders[1] = strtoul(EARGF(usage(argv0)), NULL, 10);
			break;
		case 'c':
			color = strtoul(EARGF(usage(argv0)), NULL, 16);
			break;
		default:
			usage(argv0);
			/* NOTREACHED */
	} ARGEND

	init_xcb(&conn);
	get_screen(conn, &scr);

	colaccent = multiplycolor(color, bevelfacts[0]);
	colbase = multiplycolor(color, bevelfacts[1]);
	colflat = multiplycolor(color, bevelfacts[2]);

	/* assume remaining arguments are windows */
	while (*argv)
		set_border(color, strtoul(*argv++, NULL, 16));

	xcb_aux_sync(conn);

	kill_xcb(&conn);

	return 0;
}
