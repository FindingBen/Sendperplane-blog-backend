#ifndef ROUTES_H
#define ROUTES_H

#include "http_types.h"

struct connection_info_struct;

typedef HTTP_response (*route_handler_fn)(struct connection_info_struct *con_info);
typedef struct {
    const char *method;
    const char *path;
    route_handler_fn handler;
} route_t;

extern const route_t routes[];
extern const size_t NUM_ROUTES;


#endif