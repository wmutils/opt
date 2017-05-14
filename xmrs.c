/* See LICENSE file for copyright and license details. */
#include <xcb/xcb.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "util.h"

#define CLEANMASK(m)	((m & ~0x80))
#define MAX(a, b) (a > b ? a : b)

static xcb_connection_t	*conn;
static xcb_screen_t	*scr;

static void usage(char *);
static xcb_window_t get_focuswin(void);
static void cleanup(void);

static void
cleanup(void)
{
	kill_xcb(&conn);
}

static void
usage(char *name)
{
	fprintf(stderr, "usage: %s <wid>\n", name);
	exit(1);
}

static xcb_window_t
get_focuswin(void) {
	xcb_window_t w = 0;

	xcb_get_input_focus_cookie_t  c;
	xcb_get_input_focus_reply_t  *r;

	c = xcb_get_input_focus(conn);
	r = xcb_get_input_focus_reply(conn, c, NULL);

	if (r == NULL)
		errx(1, "failed to get focused window");

	w = r->focus;
	free(r);
	return w;
}

static void
mresize(xcb_window_t win) {
	uint32_t values[3];
	xcb_get_geometry_reply_t *geom;
	xcb_query_pointer_reply_t *ptr;
	xcb_generic_event_t *ev = NULL;
	int orig_width, orig_height;

	geom = xcb_get_geometry_reply(conn, xcb_get_geometry(conn, win), NULL);
	xcb_warp_pointer(conn, XCB_NONE, win, 0, 0, 0, 0,
			geom->width, geom->height);

	if (!geom)
		errx(1, "could not get window size");

	orig_width = geom->width;
	orig_height = geom->height;

	if (!scr->root)
		return;

	xcb_grab_pointer(conn, 0, scr->root,
			XCB_EVENT_MASK_BUTTON_RELEASE
			| XCB_EVENT_MASK_BUTTON_PRESS,
			XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
			scr->root, XCB_NONE, XCB_CURRENT_TIME);

	xcb_flush(conn);


	for (;;) {
		ev = xcb_poll_for_event(conn);

		if (geom->width != orig_width || geom->height != orig_height)
			if (ev != NULL) {
				if (ev->response_type == XCB_MOTION_NOTIFY)
					break;
				exit(0);
			}

		geom = xcb_get_geometry_reply(conn,
				xcb_get_geometry(conn, win), NULL);
		if (!geom)
			errx(1, "could not get window size");


		ptr = xcb_query_pointer_reply(conn,
				xcb_query_pointer(conn, scr->root), 0);

		if (!ptr)
			errx(1, "could not get pointer location");

		values[0] = MAX(1, ptr->root_x - geom->x - 2 * geom->border_width);
		values[1] = MAX(1, ptr->root_y - geom->y - 2 * geom->border_width);


		if (values[0] >= 0 && values[1] >= 0)
			xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_WIDTH
					| XCB_CONFIG_WINDOW_HEIGHT, values);
		xcb_flush(conn);
	}

	xcb_ungrab_pointer(conn, XCB_CURRENT_TIME);
}

int
main(int argc, char **argv)
{
	xcb_window_t win;

	if (argc > 2)
		usage(argv[0]);

	init_xcb(&conn);
	get_screen(conn, &scr);
	atexit(cleanup);

	if (argc == 2)
		win = strtoul(argv[1], NULL, 16);
	else
		win = get_focuswin();

	mresize(win);

	return 0;
}
