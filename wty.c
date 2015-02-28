#include <stdio.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include "util.h"

static xcb_ewmh_connection_t* ewmh;
static xcb_connection_t* conn;
static xcb_screen_t* screen;

void usage(char*);
void init_ewmh();
xcb_atom_t get_type(xcb_window_t);
char* type_to_string(xcb_atom_t);

void
usage(char* name)
{
    fprintf(stderr,"usage: %s <wid>\n",name);
    exit(1);
}

void 
init_ewmh()
{
    if (!(ewmh = calloc(1, sizeof(xcb_ewmh_connection_t)))) printf("Fail\n");
    xcb_intern_atom_cookie_t* cookie = xcb_ewmh_init_atoms(conn,ewmh);
    xcb_ewmh_init_atoms_replies(ewmh,cookie,(void*)0);
}

xcb_atom_t 
get_type(xcb_window_t win)
{
    xcb_ewmh_get_atoms_reply_t win_type;
    if (xcb_ewmh_get_wm_window_type_reply(ewmh, xcb_ewmh_get_wm_window_type(ewmh, win), &win_type, (void*)0))
    {
        return win_type.atoms[0];
    }
    else
    {
        xcb_kill_client(conn,win);
        return 0;
    }
}

char* 
type_to_string(xcb_atom_t atom)
{
    if(atom == ewmh->_NET_WM_WINDOW_TYPE_DESKTOP) return "desktop";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_DOCK) return "dock";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_TOOLBAR) return "toolbar";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_MENU) return "menu";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_UTILITY) return "utility";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_SPLASH) return "splash";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_DIALOG) return "dialog";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_DROPDOWN_MENU) return "dropdown";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_POPUP_MENU) return "popup";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_TOOLTIP) return "tooltip";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_NOTIFICATION) return "notification";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_COMBO) return "combo";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_DND) return "dnd";
    else if(atom == ewmh->_NET_WM_WINDOW_TYPE_NORMAL) return "normal";
    return "";
}

int 
main(int argc, char** argv)
{
    init_xcb(&conn);
    get_screen(conn,&screen);
    init_ewmh();
    if(argc < 2 ) usage(argv[0]);
    xcb_window_t win = strtoul(argv[1],NULL,16);
    if(!exists(conn,win)) usage(argv[0]);
    xcb_atom_t type = get_type(win);
    if(type)
    {
        printf("%s",type_to_string(type));
        fflush(stdout);
    }
    return 0;
}
