#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "handler/main_handlers.h"
#include "handler/blog_handler.h"
#include "handler/server.h"
#include "handler/user_handler.h"
#include "handler/db.h"
#include "handler/utils.h"
#include "handler/auth.h"
#include "handler/session_handler.h"

static HTTP_response make_http_response(char *body, HTTP_status status){
    return (HTTP_response){body, status};
}

HTTP_response handle_register(struct connection_info_struct *con_info){

    char *username = extractValuesForJson(con_info->answerstring, "\"username\":");
    char *email = extractValuesForJson(con_info->answerstring, "\"email\":");
    char *password = extractValuesForJson(con_info->answerstring, "\"password_hash\":");

    if (username == NULL || email == NULL || password == NULL) {
        return make_http_response("{\"error\":\"Missing registration fields\"}", BAD_REQUEST);
    }

    const char *password_hash = password_h(password);
    int create_response = create_user(username, email, password_hash);
    return create_response
        ? make_http_response("{\"message\":\"User created\"}", CREATED)
        : make_http_response("{\"error\":\"Unable to create user\"}", INTERNAL_SERVER_ERROR);
}

HTTP_response handle_login(struct connection_info_struct *con_info){

    char *username = extractValuesForJson(con_info->answerstring, "\"username\":");
    char *password = extractValuesForJson(con_info->answerstring, "\"password\":");

    if (username == NULL || password == NULL) {
        return make_http_response("{\"error\":\"Username and password are required\"}", BAD_REQUEST);
    }

    int auth_response = authenticate_user(username, password);

    if (auth_response != 1) {
        return make_http_response("{\"error\":\"Invalid username or password\"}", UNAUTHORIZED);
    }

    char *cookie = generate_cookie();
    if (cookie == NULL) {
        return make_http_response("{\"error\":\"Unable to create session\"}", INTERNAL_SERVER_ERROR);
    }

    {
        char *session_response = return_session(con_info,cookie,username);
        free(cookie);
        if (session_response == NULL || strcmp(session_response, "ERROR") == 0) {
            return make_http_response("{\"error\":\"Unable to create session\"}", INTERNAL_SERVER_ERROR);
        }
    }

    return make_http_response("{\"message\":\"Login successful\"}", OK);
}

HTTP_response handle_logout(struct connection_info_struct *con_info){
    char *token = con_info->cookie;

    char *token_hash = token_h(token);
    int logout = expire_user_session(token_hash);

    if (logout) {
        con_info->set_cookie =
            "KEY=; Path=/; HttpOnly; SameSite=Lax; Max-Age=0; "
            "Expires=Thu, 01 Jan 1970 00:00:00 GMT";
        return make_http_response("{\"message\":\"Logged out\"}", OK);
    }
    con_info->set_cookie = NULL;
    return make_http_response(
        "{\"error\":\"Unable to log out\"}",
        INTERNAL_SERVER_ERROR
    );
}

HTTP_response handle_return_user(struct connection_info_struct *con_info){

    // const char *username = MHD_lookup_connection_value(
    //     con_info->connection, MHD_GET_ARGUMENT_KIND, "username");
    const char *token = con_info->cookie;
    printf("TOKEN %s", token);
    if (token == NULL) {
        return make_http_response("{\"error\":\"Missing token\"}", BAD_REQUEST);
    }
    char *hash_user_token = token_h(token);
    char *user_response = return_user_session(hash_user_token);
    printf("USERNAME %s", user_response);
    if (user_response == NULL) {
        return make_http_response("{\"error\":\"Unable to retrieve user\"}", INTERNAL_SERVER_ERROR);
    }
    if (strcmp(user_response, "[]") == 0) {
        return make_http_response("{\"error\":\"User not found\"}", NOT_FOUND);
    }
    return make_http_response(user_response, OK);
}

HTTP_response handle_return_users(struct connection_info_struct *con_info){

    int is_authenticated = authorize_session_token(con_info->cookie);
    if(is_authenticated==0){
        return make_http_response("{\"error\":\"User not authenticated\"}", UNAUTHORIZED);
    }
    else{
        char *user_response = return_users();

        return user_response != NULL
            ? make_http_response(user_response, OK)
            : make_http_response("{\"error\":\"Unable to retrieve users\"}", INTERNAL_SERVER_ERROR);
    }
    
}

HTTP_response handle_main_page(struct connection_info_struct *con_info){
    const char *home_page_response = home_page();
    return make_http_response((char *)home_page_response, OK);
}

HTTP_response handle_create_blog(struct connection_info_struct *con_info){
    int is_authenticated = authorize_session_token(con_info->cookie);

    if(is_authenticated==1){
        char *blog_title = extractValuesForJson(con_info->answerstring, "\"title\":");

        char *blog_body = extractValuesForJson(con_info->answerstring, "\"body\":");

        char *user_id = extractValuesForJson(con_info->answerstring, "\"user_id\":");

        if (blog_title == NULL || blog_body == NULL || user_id == NULL) {
            return make_http_response("{\"error\":\"Missing blog fields\"}", BAD_REQUEST);
        }
        
        int user_int_id = atoi(user_id);

        int create_blog_response = create_blog(blog_title, blog_body, user_int_id);

        return create_blog_response
            ? make_http_response("{\"message\":\"Blog created\"}", CREATED)
            : make_http_response("{\"error\":\"Unable to create blog\"}", INTERNAL_SERVER_ERROR);
    }
    else{
        return make_http_response("{\"error\":\"User not authenticated\"}", UNAUTHORIZED);

    }
    

}

HTTP_response handle_return_blogs(struct connection_info_struct *con_info){

    char *blogs_response = return_blogs();
    return blogs_response != NULL
        ? make_http_response(blogs_response, OK)
        : make_http_response("{\"error\":\"Unable to retrieve blogs\"}", INTERNAL_SERVER_ERROR);
}

HTTP_response handle_return_blog(struct connection_info_struct *con_info){

    const char *blog_id_value = MHD_lookup_connection_value(
        con_info->connection,
        MHD_GET_ARGUMENT_KIND,
        "id");
    if (blog_id_value == NULL) {
        return make_http_response("{\"error\":\"Missing blog id\"}", BAD_REQUEST);
    }

    char *end = NULL;
    long blog_id = strtol(blog_id_value, &end, 10);
    if (end == blog_id_value || *end != '\0' || blog_id <= 0 || blog_id > INT_MAX) {
        return make_http_response("{\"error\":\"Invalid blog id\"}", BAD_REQUEST);
    }

    const char *blog_response = return_blog((int)blog_id);
    if (blog_response == NULL) {
        return make_http_response("{\"error\":\"Unable to retrieve blog\"}", INTERNAL_SERVER_ERROR);
    }
    if (strcmp(blog_response, "[]") == 0) {
        return make_http_response("{\"error\":\"Blog not found\"}", NOT_FOUND);
    }
    return make_http_response((char *)blog_response, OK);
}

HTTP_response handle_update_blog(struct connection_info_struct *con_info){

    char *body = extractValuesForJson(con_info->answerstring, "\"body\":");
    char *title = extractValuesForJson(con_info->answerstring, "\"title\":");
    char *blog_id = extractValuesForJson(con_info->answerstring, "\"blog_id\":");
    if (blog_id == NULL || (body == NULL && title == NULL)) {
        return make_http_response("{\"error\":\"Blog id and at least one field are required\"}", BAD_REQUEST);
    }

    int blog_id_int = atoi(blog_id);
    int update_blog_response = update_blog(body, title, blog_id_int);

    return update_blog_response
        ? make_http_response("{\"message\":\"Blog updated\"}", OK)
        : make_http_response("{\"error\":\"Unable to update blog\"}", INTERNAL_SERVER_ERROR);
}