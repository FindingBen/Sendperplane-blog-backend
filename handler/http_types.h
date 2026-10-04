#ifndef HTTP_H
#define HTTP_H

typedef enum{

    GET = 15,
    POST = 16,
    DELETE_ = 17

} HTTP_TYPE_REQUEST;

typedef enum {
  CREATED = 201,
  OK = 200,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  NOT_FOUND = 404,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501
} HTTP_status;

typedef struct {
    char *body;
    HTTP_status status;
} HTTP_response;

#endif