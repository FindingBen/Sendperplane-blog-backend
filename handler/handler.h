#ifndef HANDLER_H
#define HANDLER_H

enum MHD_Result print_out_key(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

const char *return_string(void);
const char *return_another_page(void);
const char *return_not_found(void);
const char *images_page(void);

int handle_image_request(const char *method, const char *url, struct MHD_Connection *connection);

const char *file_to_read(const char *url);

#endif
