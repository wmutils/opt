/**
*      Copyright (c) 2014, Broseph <dcat (at) iotek (dot) org>
*
*      Permission to use, copy, modify, and/or distribute this software for any
*      purpose with or without fee is hereby granted, provided that the above
*      copyright notice and this permission notice appear in all copies.
*
*      THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*      WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*      MERCHANTABILITY AND FITNESS IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*      ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*      WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*      ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*      OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**/

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "arg.h"
#include "util.h"

char *argv0;
static xcb_connection_t *conn;
static xcb_screen_t *scr;

static void usage       (char *name);
static void set2border  (xcb_window_t, int, int, int, int);

static void
usage (char *name)
{
	fprintf(stderr, "usage: %s <-I color> <-O color> <-i size> <-o size> [wid...]\n", name);
	exit(1);
}

static void
set2border (win, oc, os, ic, is)
xcb_window_t win;
int oc; /* outer color */
int os; /* outer size  */
int ic; /* inner color */
int is; /* inner size  */
{
	if (os < 0 || oc < 0 || is < 0 || ic < 0)
		return;

	uint32_t values[1];
	short w, h, b, o;

	xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(conn,
			xcb_get_geometry(conn, win), NULL);

	if (geom == NULL)
		return;

	if (is + os > geom->border_width)
		warnx("warning: pixmap is greater than border size");

	w = (short)geom->width;
	h = (short)geom->height;
	b = (unsigned short)is+os;
	o = (unsigned short)os;

	xcb_rectangle_t inner[] = {
		/* you're not supposed to understand this. */
		{     w,0,b-o     ,h+b-   o      },
		{     w+b   +o,  0,   b  -o,     h+         b  -  o},
		{     0,h   ,w+b  -o,b-   o      },
		{     0,h   +b+      o,   w+     b-         o, b -o},
		{     w+b+o,b        +h    +o,b,b}
	};

	xcb_rectangle_t outer[] = {
		{w + b - o, 0, o, h + b * 2},
		{w + b,     0, o, h + b * 2},
		{0, h + b - o, w + b * 2, o},
		{0, h + b,     w + b * 2, o},
		{1, 1, 1, 1}
	};

	xcb_pixmap_t pmap = xcb_generate_id(conn);
	xcb_create_pixmap(conn, scr->root_depth, pmap, win,
			geom->width  + (b*2),
			geom->height + (b*2));
	xcb_gcontext_t gc = xcb_generate_id(conn);
	xcb_create_gc(conn, gc, pmap, 0, NULL);

	values[0] = oc;
	xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, values);
	xcb_poly_fill_rectangle(conn, pmap, gc, 5, outer);

	values[0] = ic;
	xcb_change_gc(conn, gc, XCB_GC_FOREGROUND, values);
	xcb_poly_fill_rectangle(conn, pmap, gc, 5, inner);

	values[0] = pmap;
	xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXMAP, values);

	xcb_free_pixmap(conn, pmap);
	xcb_free_gc(conn, gc);
}

int
main (int argc, char **argv)
{
	int oc,os,ic,is;

	if (argc < 2)
		usage(argv[0]);

	oc = os = ic = is = -1;
	ARGBEGIN {
		case 'I':
			ic = strtoul(EARGF(usage(argv0)), NULL, 16);
			break;
		case 'O':
			oc = strtoul(EARGF(usage(argv0)), NULL, 16);
			break;
		case 'i':
			is = strtoul(EARGF(usage(argv0)), NULL, 10);
			break;
		case 'o':
			os = strtoul(EARGF(usage(argv0)), NULL, 10);
			break;
		case 'h':
			usage(argv0);
			/* NOTREACHED */
	} ARGEND

	init_xcb(&conn);
	get_screen(conn, &scr);

	/* assume remaining arguments are windows */
	while (*argv)
		set2border(strtoul(*argv++, NULL, 16), oc, os, ic, is);

	xcb_aux_sync(conn);

	kill_xcb(&conn);

	return 0;
}
