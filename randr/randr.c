// randr.c 
//
// Utilities for getting information about different types of displays.
#include "randr.h"

int 
get_providers(xcb_connection_t* conn, xcb_screen_t* scrn, xcb_randr_provider_t **ps)
{
    xcb_randr_get_providers_cookie_t pc;
    pc = xcb_randr_get_providers(conn, scrn->root);

    xcb_randr_get_providers_reply_t *pr;
    pr = xcb_randr_get_providers_reply(conn, pc, NULL);
    if (!pr)
        errx(1, "Could not aquire the array of providers.");
    
    *ps = xcb_randr_get_providers_providers(pr);
    
    return xcb_randr_get_providers_providers_length(pr);
}

xcb_randr_output_t*
get_outputs(xcb_connection_t* conn, xcb_randr_provider_t provider, int *length)
{
    xcb_randr_get_provider_info_cookie_t pic;
    pic = xcb_randr_get_provider_info(conn, provider, 0);
   
    xcb_randr_get_provider_info_reply_t *pir;
    // TODO: Error check the following line.
    pir = xcb_randr_get_provider_info_reply(conn, pic, NULL);
    
    xcb_randr_output_t *o = xcb_randr_get_provider_info_outputs(pir);
    *(length) = xcb_randr_get_provider_info_outputs_length(pir);
    return o;
}


