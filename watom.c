/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <err.h>
#include <string.h>

#include "util.h"

static xcb_connection_t *conn;

static void usage(char *);
static int get_watom(xcb_window_t, char *, char *);

static void
usage(char *name)
{
	fprintf(stderr, "usage: %s <wid> <ATOM_NAME> <ATOM_TYPE>\n", name);
	exit(1);
}

static xcb_atom_t xcb_atom_get(xcb_connection_t *conn, char *name)
{
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, strlen(name), name);
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, NULL);
  return !reply ? XCB_ATOM_STRING : reply->atom;
}

static int
get_watom(xcb_window_t win, char *prop, char *type)
{
  int len = 0;
  xcb_get_property_cookie_t cookie;
  xcb_get_property_reply_t *r;
  
  cookie = xcb_get_property(conn, 0, win,
  xcb_atom_get(conn, prop),
  xcb_atom_get(conn, type),
    0L, 32L);
  r = xcb_get_property_reply(conn, cookie, NULL);

  if (r) {
    len = xcb_get_property_value_length(r);
    
    if(len == 0){
      free(r);
      return 1;
    }

    printf("%.*s\n", len, (char *) xcb_get_property_value(r));
    free(r);
    return 0;
  }

  warnx("could not get %s\n", prop);
  free(r);
  return 1;
}

int
main(int argc, char **argv)
{
	int r = 0;

	if (argc != 4)
		usage(argv[0]);

	init_xcb(&conn);

	r = get_watom(strtoul(argv[1], NULL, 16), argv[2], argv[3]);

	kill_xcb(&conn);

	return r;
}
