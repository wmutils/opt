#ifndef WMUTILS_RANDR
#define WMUTILS_RANDR

#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <err.h>

// Return the list of outputs provided by a provider. 
//int 
//get_outputs(xcb_randr_provider_t provider, xcb_randr_output_t **os);

xcb_randr_output_t*
get_outputs(xcb_connection_t* conn, xcb_randr_provider_t provider, int *length);

// Return an array of providers shown by randr.
int 
get_providers(xcb_connection_t* con, xcb_screen_t* scrn, xcb_randr_provider_t **ps);

#endif /* end of include guard: WMUTILS_RANDR */
