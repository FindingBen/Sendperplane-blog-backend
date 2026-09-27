#include <stdio.h>
#include <microhttpd.h>
#include "handler/server.h"
#include "handler/db.h"


int main(){

    load_env(".env");

    struct MHD_Daemon *deamon;
    deamon = MHD_start_daemon(
    MHD_USE_INTERNAL_POLLING_THREAD,
    PORT,
    NULL,
    NULL,
    &response_handler,
    NULL,
    MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL, MHD_OPTION_END
    );
    
    if(NULL == deamon){
        return 1;
    }

    getchar();

    MHD_stop_daemon(deamon);
    return 0;

    
}