#include <string.h>
#include "handler/routes.h"
#include "handler/server.h"
#include "handler/main_handlers.h"
#include "handler/blog_handler.h"

#define ROUTE_REGISTER      "/register"
#define ROUTE_LOGIN         "/login"
#define ROUTE_CREATE_BLOG   "/create_blog"


const route_t routes[] = {
    { "POST",   "/register",     handle_register},
    {"POST", "/login", handle_login},
    {"GET", "/logout", handle_logout},
    {"GET",     "/",     handle_main_page},
    {"GET", "/user", handle_return_user},
    {"GET", "/users", handle_return_users},
    { "POST", "/create_blog",  handle_create_blog  },
    { "POST", "/update_blog",  handle_update_blog  },
    { "GET", "/blogs",  handle_return_blogs  },

};

const size_t NUM_ROUTES  = (sizeof(routes) / sizeof(routes[0]));