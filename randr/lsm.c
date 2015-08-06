#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>

#include "../util.h"

static xcb_connection_t *conn;
static xcb_screen_t *scrn;

static void 
usage(char *name)
{
    printf("usage: %s \n", name);
}

xcb_randr_provider_t* 
get_providers(int *length)
{
    xcb_randr_get_providers_cookie_t providers_cookie;
    providers_cookie = xcb_randr_get_providers(conn, scrn->root);
    xcb_randr_get_providers_reply_t *providers_reply;
   
    // TODO: Error check the following line.
    providers_reply = xcb_randr_get_providers_reply(conn, providers_cookie, NULL);
    
    xcb_randr_provider_t *providers = xcb_randr_get_providers_providers(providers_reply);
    *(length) = xcb_randr_get_providers_providers_length(providers_reply);
    return providers;
}

xcb_randr_output_t*
get_outputs(xcb_randr_provider_t provider, int *length)
{
    xcb_randr_get_provider_info_cookie_t provider_info_cookie;

    // TODO: Figure out the use of the timestamp below.
    provider_info_cookie = xcb_randr_get_provider_info(conn, provider, 0);
    xcb_randr_get_provider_info_reply_t *provider_info_reply;
   
    // TODO: Error check the following line.
    provider_info_reply = xcb_randr_get_provider_info_reply(conn, provider_info_cookie, NULL);
    
xcb_randr_output_t *outputs = xcb_randr_get_provider_info_outputs(provider_info_reply);
    *(length) = xcb_randr_get_provider_info_outputs_length(provider_info_reply);
    return outputs;
}

void 
print_output(xcb_randr_output_t output)
{
     
    xcb_randr_get_output_info_cookie_t output_info_cookie;

    // TODO: Figure out the use of the timestamp below.
    output_info_cookie = xcb_randr_get_output_info(conn, output, 0);
    xcb_randr_get_output_info_reply_t *output_info_reply;
   
    // TODO: Error check the following line.
    output_info_reply = xcb_randr_get_output_info_reply(conn, output_info_cookie, NULL);
    uint8_t* name = xcb_randr_get_output_info_name(output_info_reply);
    if (output_info_reply->connection == 0) {
        printf("%s\n", name);
    }
}

int 
main(int argc, char *argv[])
{
    init_xcb(&conn);
    get_screen(conn, &scrn);
    int p_len = 0;
    xcb_randr_provider_t* providers = get_providers(&p_len);
    for (int i = 0; i < p_len; ++i) {
        int o_len = 0;
        xcb_randr_output_t* outputs = get_outputs(providers[i], &o_len);
        for (int j = 0; j < o_len; ++j) {
            print_output(outputs[j]);
        }
    }
        
    // usage(argv[0]);
    return 0;
}
