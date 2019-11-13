#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <getopt.h>
#include <xcb/xcb.h>

#include "util.h"
#include "arg.h"

void usage(char*);
void list_events(void);
void register_events(xcb_window_t, uint32_t);
void print_event(xcb_generic_event_t*);

static xcb_connection_t *conn;
static xcb_screen_t *scr;
static uint32_t mask = XCB_EVENT_MASK_NO_EVENT
                     | XCB_EVENT_MASK_STRUCTURE_NOTIFY
                     | XCB_EVENT_MASK_FOCUS_CHANGE
                     | XCB_EVENT_MASK_ENTER_WINDOW
                     /* | XCB_EVENT_MASK_LEAVE_WINDOW */
                     /* | XCB_EVENT_MASK_KEY_PRESS */
                     /* | XCB_EVENT_MASK_KEY_RELEASE */
                     /* | XCB_EVENT_MASK_BUTTON_PRESS */
                     /* | XCB_EVENT_MASK_BUTTON_RELEASE */
                     /* | XCB_EVENT_MASK_POINTER_MOTION */
                     /* | XCB_EVENT_MASK_POINTER_MOTION_HINT */
                     /* | XCB_EVENT_MASK_BUTTON_1_MOTION */
                     /* | XCB_EVENT_MASK_BUTTON_2_MOTION */
                     /* | XCB_EVENT_MASK_BUTTON_3_MOTION */
                     /* | XCB_EVENT_MASK_BUTTON_4_MOTION */
                     /* | XCB_EVENT_MASK_BUTTON_5_MOTION */
                     /* | XCB_EVENT_MASK_BUTTON_MOTION */
                     /* | XCB_EVENT_MASK_KEYMAP_STATE */
                     /* | XCB_EVENT_MASK_EXPOSURE */
                     /* | XCB_EVENT_MASK_VISIBILITY_CHANGE */
                     /* | XCB_EVENT_MASK_RESIZE_REDIRECT */
                     /* | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY */
                     /* | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT */
                     /* | XCB_EVENT_MASK_PROPERTY_CHANGE */
                     /* | XCB_EVENT_MASK_COLOR_MAP_CHANGE */
                     /* | XCB_EVENT_MASK_OWNER_GRAB_BUTTON */
                     ;
static uint32_t rootmask = XCB_EVENT_MASK_NO_EVENT
                         | XCB_EVENT_MASK_STRUCTURE_NOTIFY
                         | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
                         | XCB_EVENT_MASK_BUTTON_PRESS
                         | XCB_EVENT_MASK_BUTTON_RELEASE
                         ;

static const char *evname[] = {
        [0]                     = "EVENT_ERROR",
        [XCB_CREATE_NOTIFY]     = "CREATE",
        [XCB_DESTROY_NOTIFY]    = "DESTROY",
        [XCB_BUTTON_PRESS]      = "BUTTON_PRESS",
        [XCB_BUTTON_RELEASE]    = "BUTTON_RELEASE",
        [XCB_MOTION_NOTIFY]     = "MOTION",
        [XCB_ENTER_NOTIFY]      = "ENTER",
        [XCB_LEAVE_NOTIFY]      = "LEAVE",
        [XCB_CONFIGURE_NOTIFY]  = "CONFIGURE",
        [XCB_KEY_PRESS]         = "KEY_PRESS",
        [XCB_FOCUS_IN]          = "FOCUS_IN",
        [XCB_FOCUS_OUT]         = "FOCUS_OUT",
        [XCB_KEYMAP_NOTIFY]     = "KEYMAP",
        [XCB_EXPOSE]            = "EXPOSE",
        [XCB_GRAPHICS_EXPOSURE] = "GRAPHICS_EXPOSURE",
        [XCB_NO_EXPOSURE]       = "NO_EXPOSURE",
        [XCB_VISIBILITY_NOTIFY] = "VISIBILITY",
        [XCB_UNMAP_NOTIFY]      = "UNMAP",
        [XCB_MAP_NOTIFY]        = "MAP",
        [XCB_MAP_REQUEST]       = "MAP_REQUEST",
        [XCB_REPARENT_NOTIFY]   = "REPARENT",
        [XCB_CONFIGURE_REQUEST] = "CONFIGURE_REQUEST",
        [XCB_GRAVITY_NOTIFY]    = "GRAVITY",
        [XCB_RESIZE_REQUEST]    = "RESIZE_REQUEST",
        [XCB_CIRCULATE_NOTIFY]  = "CIRCULATE",
        [XCB_PROPERTY_NOTIFY]   = "PROPERTY",
        [XCB_SELECTION_CLEAR]   = "SELECTION_CLEAR",
        [XCB_SELECTION_REQUEST] = "SELECTION_REQUEST",
        [XCB_SELECTION_NOTIFY]  = "SELECTION",
        [XCB_COLORMAP_NOTIFY]   = "COLORMAP",
        [XCB_CLIENT_MESSAGE]    = "CLIENT_MESSAGE",
        [XCB_MAPPING_NOTIFY]    = "MAPPING"
};

void
usage(char *name)
{
	fprintf(stderr, "usage: %s [-l] [-rm <mask>]\n", name);
	exit(1);
}

void
list_events(void)
{
	printf(
		"XCB_EVENT_MASK_NO_EVENT .............. 0\n"
		"XCB_EVENT_MASK_KEY_PRESS ............. 1\n"
		"XCB_EVENT_MASK_KEY_RELEASE ........... 2\n"
		"XCB_EVENT_MASK_BUTTON_PRESS .......... 4\n"
		"XCB_EVENT_MASK_BUTTON_RELEASE ........ 8\n"
		"XCB_EVENT_MASK_ENTER_WINDOW .......... 16\n"
		"XCB_EVENT_MASK_LEAVE_WINDOW .......... 32\n"
		"XCB_EVENT_MASK_POINTER_MOTION ........ 64\n"
		"XCB_EVENT_MASK_POINTER_MOTION_HINT ... 128\n"
		"XCB_EVENT_MASK_BUTTON_1_MOTION ....... 256\n"
		"XCB_EVENT_MASK_BUTTON_2_MOTION ....... 512\n"
		"XCB_EVENT_MASK_BUTTON_3_MOTION ....... 1024\n"
		"XCB_EVENT_MASK_BUTTON_4_MOTION ....... 2048\n"
		"XCB_EVENT_MASK_BUTTON_5_MOTION ....... 4096\n"
		"XCB_EVENT_MASK_BUTTON_MOTION ......... 8192\n"
		"XCB_EVENT_MASK_KEYMAP_STATE .......... 16384\n"
		"XCB_EVENT_MASK_EXPOSURE .............. 32768\n"
		"XCB_EVENT_MASK_VISIBILITY_CHANGE ..... 65536\n"
		"XCB_EVENT_MASK_STRUCTURE_NOTIFY ...... 131072\n"
		"XCB_EVENT_MASK_RESIZE_REDIRECT ....... 262144\n"
		"XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY ... 524288\n"
		"XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT . 1048576\n"
		"XCB_EVENT_MASK_FOCUS_CHANGE .......... 2097152\n"
		"XCB_EVENT_MASK_PROPERTY_CHANGE ....... 4194304\n"
		"XCB_EVENT_MASK_COLOR_MAP_CHANGE ...... 8388608\n"
		"XCB_EVENT_MASK_OWNER_GRAB_BUTTON ..... 16777216\n"
		);
}

void
register_events(xcb_window_t w, uint32_t m)
{
	uint32_t val[] = { m };

	xcb_change_window_attributes(conn, w, XCB_CW_EVENT_MASK, val);
	xcb_flush(conn);
}

void
print_event(xcb_generic_event_t *e)
{
	xcb_enter_notify_event_t *en;
	xcb_focus_in_event_t *ef;

	switch (e->response_type & ~0x80) {
		case XCB_ENTER_NOTIFY:
		case XCB_LEAVE_NOTIFY:
			en = (xcb_enter_notify_event_t *)e;
			if (en->mode != XCB_NOTIFY_MODE_NORMAL)
				return;
			printf("%s\t0x%08x\n", evname[e->response_type], en->event);
			break;

		case XCB_FOCUS_IN:
		case XCB_FOCUS_OUT:
			ef = (xcb_focus_in_event_t *)e;
			if (ef->mode != XCB_NOTIFY_MODE_NORMAL)
				return;
			printf("%s\t0x%08x\n", evname[e->response_type], ef->event);
			break;

		case XCB_CREATE_NOTIFY:
		case XCB_DESTROY_NOTIFY:
			printf("%s\t0x%08x\n", evname[e->response_type],
				((xcb_create_notify_event_t*)e)->window);
			break;

		case XCB_UNMAP_NOTIFY:
		case XCB_MAP_NOTIFY:
			printf("%s\t0x%08x\n", evname[e->response_type],
				((xcb_map_notify_event_t*)e)->window);
			break;

		case XCB_BUTTON_PRESS:
		case XCB_BUTTON_RELEASE:
			printf("%s\t0x%08x %d\n", evname[e->response_type],
				((xcb_button_press_event_t*)e)->event,
				((xcb_button_press_event_t*)e)->detail);
			break;

		/*
		 * MOTION_NOTIFY. Be careful with this one...
		 *         It triggers ALOT! 
		 *
		 *          |\.---./|_
	 	 *         (o)   (°) ''--._
	 	 *         / .A--.         '\
	 	 *         |'     `.         \
	 	 *         /                 \
		 *        /_;`|__\------(__/-\
		 */
		case XCB_MOTION_NOTIFY:
			printf("%s\t0x%08x\n", evname[e->response_type],
				((xcb_motion_notify_event_t*)e)->event);
			break;
		/* please... handle me ... ;_;
		case XCB_KEY_PRESS:
		case XCB_KEY_RELEASE:
		case XCB_KEYMAP_NOTIFY:
		case XCB_EXPOSE:
		case XCB_GRAPHICS_EXPOSURE:
		case XCB_NO_EXPOSURE:
		case XCB_VISIBILITY_NOTIFY:
		case XCB_MAP_REQUEST:
		case XCB_REPARENT_NOTIFY:
		case XCB_CONFIGURE_NOTIFY:
		case XCB_CONFIGURE_REQUEST:
		case XCB_GRAVITY_NOTIFY:
		case XCB_RESIZE_REQUEST:
		case XCB_CIRCULATE_NOTIFY:
		case XCB_CIRCULATE_REQUEST:
		case XCB_PROPERTY_NOTIFY:
		case XCB_SELECTION_CLEAR:
		case XCB_SELECTION_REQUEST:
		case XCB_SELECTION_NOTIFY:
		case XCB_COLORMAP_NOTIFY:
		case XCB_CLIENT_MESSAGE:
		case XCB_MAPPING_NOTIFY:
		*/
	}
}

int
main (int argc, char **argv)
{
	int i, wn;
	char *argv0;
	xcb_window_t *wc;
	xcb_generic_event_t *e;
	xcb_create_notify_event_t *ec;

	ARGBEGIN {
		case 'l':
			list_events();
			exit(0);
		case 'r':
			  rootmask = strtoul(EARGF(usage(argv0)), NULL, 10);
			  break;
		case 'm':
			  mask = strtoul(EARGF(usage(argv0)), NULL, 10);
			  break;
		default: usage(argv0);
	} ARGEND;

	init_xcb(&conn);
	get_screen(conn, &scr);

	/*
	 * We need to get notifed of window creations, no matter what, because
	 * we need to register the event mask on all newly created windows
	 */
	register_events(scr->root, rootmask);

	/* register the events on all mapped windows */
	wn = get_windows(conn, scr->root, &wc);
	for (i=0; i<wn; i++)
		register_events(wc[i], mask);

	while(xcb_flush(conn)) {
		e = xcb_wait_for_event(conn);
		switch (e->response_type & ~0x80)
		{
			/* register events for each new windows */
			case XCB_CREATE_NOTIFY:
				ec = (xcb_create_notify_event_t*)e;
				register_events(ec->window, mask);
				/* FALLTHROUGH */
		}
		print_event(e);
		fflush(stdout);

		free(e);
	}

	kill_xcb(&conn);

	return 0;
}
