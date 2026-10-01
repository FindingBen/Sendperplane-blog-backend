#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <microhttpd.h>
#include "handler/server.h"
#include "handler/db.h"

#define DEFAULT_PORT 8080

int main(){

    load_env(".env");

    const char *port_env = getenv("PORT");
    char *port_end = NULL;
    long port_value = port_env == NULL ? DEFAULT_PORT : strtol(port_env, &port_end, 10);
    if (port_env != NULL &&
        (port_end == port_env || *port_end != '\0' || port_value < 1 || port_value > 65535)) {
        fprintf(stderr, "Invalid PORT value: %s\n", port_env);
        return 1;
    }

    struct MHD_Daemon *deamon;
    deamon = MHD_start_daemon(
    MHD_USE_INTERNAL_POLLING_THREAD,
    (uint16_t)port_value,
    NULL,
    NULL,
    &response_handler,
    NULL,
    MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL, MHD_OPTION_END
    );
    
    if(NULL == deamon){
        return 1;
    }

    while(1) {
    Sleep(1);
    }

    MHD_stop_daemon(deamon);
    return 0;

    
}