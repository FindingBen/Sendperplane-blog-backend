#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <winsock2.h>
#include <microhttpd.h>
#include <windows.h>
#include "handler/server.h"
#include "handler/user_handler.h"
#include "handler/http_types.h"
#include "handler/db.h"
#include "handler/utils.h"
#include "handler/auth.h"

struct connection_info_struct
{
    int connectiontype;
    char *answerstring;
    struct MHD_PostProcessor *postprocessor;
};

#define MAXNAMESIZE 100
#define POSTBUFFERSIZE 500

static void print_con_info(struct connection_info_struct *con_info){
    printf("con_info => connectiontype: %d, answerstring: %s, postprocessor: %p\n",
        con_info->connectiontype,
        con_info->answerstring ? con_info->answerstring : "(null)",
        (void*)con_info->postprocessor);
}

enum MHD_Result response_handler(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **req_cls
){
        struct MHD_Response *response;

        if(strcmp(method,"GET")==0){
            const char *main_page = home_page();
            const char *user_response_text = return_user("Bessn");
            

            if(strcmp(url, "/")==0){
                return manage_response(main_page,connection,response);
            }
            else if(strcmp(url, "/users.html")==0){
                return manage_response(user_response_text,connection,response);
            }
            else if(strcmp(url, "/login.html")==0){
                authenticate_user("Bessn","123");
                return manage_response(main_page,connection,response);
            }
            return MHD_NO;
        }
        else if(strcmp(method,"POST")==0){
            struct connection_info_struct *con_info;

            // first call for this request: set up state and wait for body data
            if(*req_cls == NULL){
                printf("PHASE2",req_cls);
                con_info = malloc(sizeof(struct connection_info_struct));
                if(con_info == NULL){
                    return MHD_NO;
                }

                con_info->answerstring = NULL;
                con_info->postprocessor = NULL;
                con_info->connectiontype = POST;

                *req_cls = (void*) con_info;
                return MHD_YES;
            }

            con_info = *req_cls;
            
            // still receiving the raw JSON body, buffer it
            if(*upload_data_size != 0){
                char *body = malloc(*upload_data_size + 1);
                if(body == NULL){
                    return MHD_NO;
                }

                memcpy(body, upload_data, *upload_data_size);
                body[*upload_data_size] = '\0';

                con_info->answerstring = body;

                *upload_data_size = 0;
                return MHD_YES;
            }

            // all body data received, respond
            print_con_info(con_info);
            const int request = handle_post_request(con_info->answerstring);
            if(request == 1){
                const char *error = "there has been error";
                return manage_response(error, connection, response);
            }
            else{
                const char *page = "Success!";
                return manage_response(page, connection, response);
            }
            
        }

        return MHD_NO;
}

const int manage_response(const char *page,struct MHD_Connection *connection, struct MHD_Response *response){
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK,response);

    MHD_destroy_response(response);

    return ret;

}

void request_completed(void *cls, struct MHD_Connection *connection, void **req_cls, enum MHD_RequestTerminationCode toe){
    struct connection_info_struct *con_info = *req_cls;

    if(con_info == NULL){
        return;
    }
    else if(con_info -> connectiontype == POST){
        if(con_info -> postprocessor){
            MHD_destroy_post_processor(con_info ->postprocessor);
        }
        if(con_info -> answerstring){
            free(con_info->answerstring);
        }
        
        free(con_info);
        *req_cls = NULL;
    }
}

const int handle_post_request(char *answerstring){

   char *username = extractValuesForJson(answerstring, "\"username\":");
   char *email = extractValuesForJson(answerstring, "\"email\":");
   char *password = extractValuesForJson(answerstring, "\"password_hash\":");

   const char *password_hash = password_h(password);

   int create_response = create_user(username, email, password_hash);

   return create_response;


}

const char *home_page(){
    const char *page = "<html><body><a href='/users.html'>Go to users here</a></body></html>";

    return page;
}