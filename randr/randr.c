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


int 
get_outputs(xcb_connection_t* conn, xcb_randr_provider_t provider, xcb_randr_output_t** os)
{
    xcb_randr_get_provider_info_cookie_t pic;
    pic = xcb_randr_get_provider_info(conn, provider, 0);
   
    xcb_randr_get_provider_info_reply_t *pir;
    // TODO: Error check the following line.
    pir = xcb_randr_get_provider_info_reply(conn, pic, NULL);
    
    *os = xcb_randr_get_provider_info_outputs(pir);
    return xcb_randr_get_provider_info_outputs_length(pir);
}



xcb_randr_get_output_info_reply_t *
get_output_info(xcb_connection_t* conn, xcb_randr_output_t output){
    xcb_randr_get_output_info_cookie_t oic;

    // TODO: Figure out the use of the timestamp below.
    oic = xcb_randr_get_output_info(conn, output, 0);
    xcb_randr_get_output_info_reply_t *output_info_reply;
   
    // TODO: Error check the following line.
    output_info_reply = xcb_randr_get_output_info_reply(conn, oic, NULL);

    return output_info_reply;
}

uint8_t* get_output_name(xcb_connection_t* conn, xcb_randr_output_t output){
    xcb_randr_get_output_info_reply_t* r;
    r = get_output_info(conn, output);
    return xcb_randr_get_output_info_name(r);
}

int get_output_connection(xcb_connection_t* conn, xcb_randr_output_t output){
    xcb_randr_get_output_info_reply_t* r;
    r = get_output_info(conn, output);
    return r->connection;
}

