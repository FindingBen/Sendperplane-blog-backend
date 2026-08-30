#include "server.c"
#include <stdio.h>

int main(){

    struct MHD_Daemon *deamon;

    deamon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, &on_client_connect, NULL,&answer_to_connection,NULL, MHD_OPTION_END);

    if(NULL == deamon){
        return 1;
    }

    getchar();

    MHD_stop_daemon(deamon);
    return 0;


}