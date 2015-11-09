#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "randr.h"
#include "../util.h"

static xcb_connection_t *conn;
static xcb_screen_t *scrn;

static void 
usage(char *name)
{
    printf("usage: %s \n", name);
}

uint8_t*
get_output_name(xcb_randr_output_t output){
    xcb_randr_get_output_info_cookie_t oic;

    // TODO: Figure out the use of the timestamp below.
    oic = xcb_randr_get_output_info(conn, output, 0);
    xcb_randr_get_output_info_reply_t *output_info_reply;
   
    // TODO: Error check the following line.
    output_info_reply = xcb_randr_get_output_info_reply(conn, oic, NULL);
    return xcb_randr_get_output_info_name(output_info_reply);
}

void 
print_output(xcb_randr_output_t output)
{
    uint8_t* name = get_output_name(output);
    if (output_info_reply->connection == 0) {
        printf("%s\n", name);
    }
}

int 
main(int argc, char *argv[])
{
    if (argc > 1) usage(argv[0]);
    init_xcb(&conn);
    get_screen(conn, &scrn);
    xcb_randr_provider_t* providers;
    int p_len = get_providers(conn, scrn, &providers);
    for (int i = 0; i < p_len; ++i) {
        int o_len = 0;
        xcb_randr_output_t* outputs = get_outputs(conn, providers[i], &o_len);
        for (int j = 0; j < o_len; ++j) {
            print_output(outputs[j]);
        }
    }
    return 0;
}
