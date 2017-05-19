#ifndef WMUTILS_RANDR
#define WMUTILS_RANDR

#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <err.h>

// Return the list of outputs provided by a provider. 
//int 
//get_outputs(xcb_randr_provider_t provider, xcb_randr_output_t **os);
int
get_outputs(xcb_connection_t* conn, xcb_randr_provider_t provider, xcb_randr_output_t** os);

// Return an array of providers shown by randr.
int 
get_providers(xcb_connection_t* con, xcb_screen_t* scrn, xcb_randr_provider_t **ps);

uint8_t* get_output_name(xcb_connection_t* conn, xcb_randr_output_t output);
int get_output_connection(xcb_connection_t* conn, xcb_randr_output_t output);

#endif /* end of include guard: WMUTILS_RANDR */
