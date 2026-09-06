#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>

#define PORT 8888

enum MHD_Result response_handler(void *cls, struct MHD_Connection *connection,
const char *url, const char *method, const char *version, const char *data,
size_t *upload_data_size, void **req_cls);

const int manage_response(const char *page,struct MHD_Connection *connection, struct MHD_Response *response);

void request_completed(void *cls, struct MHD_Connection *connection, void **req_cls, enum MHD_RequestTerminationCode toe);

const char *home_page(void);

const int handle_post_request(char *answerstring);

#endif