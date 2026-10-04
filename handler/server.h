#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>
#include <stddef.h>
#include "http_types.h"

struct connection_info_struct
{
    int connectiontype;
    char *answerstring;
    size_t answerstring_len;
    struct MHD_Connection *connection;
    struct MHD_Response *response;
    struct MHD_PostProcessor *postprocessor;
    char *cookie;
    char *set_cookie;
};

enum MHD_Result response_handler(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *data,
    size_t *upload_data_size,
    void **req_cls
);

enum MHD_Result manage_response(
    HTTP_response app_response,
    struct MHD_Connection *connection,
    char *cookie
);

void request_completed(
    void *cls,
    struct MHD_Connection *connection,
    void **req_cls,
    enum MHD_RequestTerminationCode toe
);

HTTP_response dispatcher(const char *method, const char *url, struct connection_info_struct *con_info);

int parse_post_body(struct connection_info_struct *con_info, size_t *upload_data_size, const char *upload_data, void **req_cls);

const char *home_page(void);


#endif