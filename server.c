#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <microhttpd.h>
#include "handler/server.h"
#include "handler/user_handler.h"
#include "handler/main_handlers.h"
#include "handler/http_types.h"
#include "handler/db.h"
#include "handler/utils.h"
#include "handler/auth.h"
#include "handler/routes.h"

#define MAXNAMESIZE 100
#define POSTBUFFERSIZE 500
#define MAX_BODY_SIZE (1024 * 1024)

static void print_con_info(struct connection_info_struct *con_info){
    printf("con_info => connectiontype: %d, answerstring: %s, postprocessor: %p\n",
        con_info->connectiontype,
        con_info->answerstring ? con_info->answerstring : "(null)",
        (void*)con_info->postprocessor);
}

static int add_cors_headers(struct MHD_Response *response, struct MHD_Connection *connection){
    const char *allowed_origin = getenv("CORS_ORIGIN");
    const char *request_origin = MHD_lookup_connection_value(
        connection, MHD_HEADER_KIND, "Origin");

    if (allowed_origin == NULL || request_origin == NULL ||
        strcmp(allowed_origin, request_origin) != 0) {
        return 0;
    }

    MHD_add_response_header(response, "Access-Control-Allow-Origin", allowed_origin);
    MHD_add_response_header(response, "Access-Control-Allow-Credentials", "true");
    MHD_add_response_header(response, "Vary", "Origin");
    return 1;
}

static enum MHD_Result handle_preflight(struct MHD_Connection *connection){
    struct MHD_Response *response = MHD_create_response_from_buffer(
        0, "", MHD_RESPMEM_PERSISTENT);
    if (response == NULL) {
        return MHD_NO;
    }

    if (add_cors_headers(response, connection)) {
        MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
        MHD_add_response_header(response, "Access-Control-Max-Age", "86400");
    }

    enum MHD_Result result = MHD_queue_response(
        connection, MHD_HTTP_NO_CONTENT, response);
    MHD_destroy_response(response);
    return result;
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
        struct connection_info_struct *con_info;

    if (strcmp(method, "OPTIONS") == 0) {
        return handle_preflight(connection);
    }

        if(strcmp(method,"GET")==0){
            struct MHD_Response *response = NULL;
            const char *token =
            MHD_lookup_connection_value(connection, MHD_COOKIE_KIND, "KEY");
            con_info = calloc(1, sizeof(struct connection_info_struct));
            if (con_info == NULL) {
                return MHD_NO;
            }

            con_info->connection = connection;
            con_info->response = response;
            con_info->connectiontype = GET;
            con_info->set_cookie=token;
        

            int dispatcher_response = dispatcher(method, url, con_info);

            free(con_info);
            return dispatcher_response;
        }

        if (strcmp(method, "POST") == 0) {
            if (*req_cls == NULL) {
                int result = parse_post_body(NULL, upload_data_size, upload_data, req_cls);
                con_info = *req_cls;
                if (con_info != NULL) {
                    con_info->connection = connection;
                    con_info->response = NULL;
                }
                return result;
            }

            con_info = *req_cls;

            if (*upload_data_size != 0) {
                return parse_post_body(con_info, upload_data_size, upload_data, req_cls);
            }

            int request = dispatcher(method, url, con_info);
           
            
            const char *page = request == 0
                ? "there has been error"
                : "Success!";

            return manage_response(page, connection, NULL, con_info->set_cookie);
        }

        return MHD_NO;
}

const int manage_response(const char *page,struct MHD_Connection *connection, struct MHD_Response *response, char *cookie){
    int ret;

    response = MHD_create_response_from_buffer(strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    if (response == NULL) {
        return MHD_NO;
    }

    add_cors_headers(response, connection);

    if (cookie != NULL) {
        MHD_add_response_header(response,
                                MHD_HTTP_HEADER_SET_COOKIE,
                                cookie);
    }
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


int dispatcher(const char *method, const char *url, struct connection_info_struct *con_info){
    
    if (con_info == NULL) {
        return MHD_NO;
    }

    const char *query_string = strchr(url, '?');
    size_t url_path_length = query_string == NULL
        ? strlen(url)
        : (size_t)(query_string - url);

    for(size_t i=0;i< NUM_ROUTES;++i){
        if(strcmp(method, routes[i].method)==0 &&
           strlen(routes[i].path) == url_path_length &&
           strncmp(url, routes[i].path, url_path_length)==0){
            return routes[i].handler(con_info);
        }
    }

    return MHD_YES;

}

int parse_post_body(struct connection_info_struct *con_info, size_t *upload_data_size, const char *upload_data, void **req_cls){

    if(*req_cls == NULL){
        con_info = calloc(1, sizeof(struct connection_info_struct));
        if (con_info == NULL) {
            return MHD_NO;
        }

        con_info->connectiontype = POST;
        con_info->postprocessor = NULL;

        *req_cls = (void*) con_info;
        return MHD_YES;
    }

    if(*upload_data_size != 0){
                size_t old_len = con_info->answerstring_len;
                size_t new_len = old_len + *upload_data_size;
                
                if(new_len > MAX_BODY_SIZE){
                    return MHD_NO;
                }
                
                char *new_buf = realloc(con_info->answerstring, new_len + 1);
                
                if(new_buf == NULL){
                    return MHD_NO;
                }
                con_info->answerstring = new_buf;

                memcpy(con_info->answerstring + old_len, upload_data, *upload_data_size);
                con_info->answerstring[new_len] = '\0';
                con_info->answerstring_len = new_len;

                *upload_data_size = 0;
                return MHD_YES;
            }

    return MHD_YES;

}

const char *home_page(){
    const char *page = "<html><body><a href='/users.html'>Go to users here</a></body></html>";

    return page;
}