#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>

#include "../util.h"

static xcb_connection_t *conn;

// return x
// return y
// return w
// return h

int main(int argc, char *argv[])
{
    init_xcb(&conn); 
    int retval = 0;
     
    return retval;
}
