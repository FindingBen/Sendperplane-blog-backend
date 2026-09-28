#include <string.h>
#include <stdlib.h>
#include "handler/main_handlers.h"
#include "handler/blog_handler.h"
#include "handler/server.h"
#include "handler/user_handler.h"
#include "handler/db.h"
#include "handler/utils.h"
#include "handler/auth.h"
#include "handler/session_handler.h"

int handle_register(struct connection_info_struct *con_info){

    char *username = extractValuesForJson(con_info->answerstring, "\"username\":");
    char *email = extractValuesForJson(con_info->answerstring, "\"email\":");
    char *password = extractValuesForJson(con_info->answerstring, "\"password_hash\":");

    const char *password_hash = password_h(password);

    int create_response = create_user(username, email, password_hash);

    return create_response;
}

int handle_login(struct connection_info_struct *con_info){

    char *username = extractValuesForJson(con_info->answerstring, "\"username\":");
    char *password = extractValuesForJson(con_info->answerstring, "\"password\":");

    char *cookie;
    int auth_response = authenticate_user(username, password);

    if(auth_response == 1){
        cookie = generate_cookie();
        char *session_response = return_session(con_info,cookie,username);
        
        return 1;
    }
    else{
        return 0;
    }
}

int handle_return_user(struct connection_info_struct *con_info){

    char *username = con_info->answerstring;

    char *user_response = return_user(username);

    int response = manage_response(user_response, con_info->connection,con_info->response);

    return response;

}

int handle_return_users(struct connection_info_struct *con_info){

    char *user_response = return_users();

    int response = manage_response(user_response, con_info->connection,con_info->response);

    return response;
}

int handle_main_page(struct connection_info_struct *con_info){
    const char *home_page_response = home_page();
    
    int response = manage_response(home_page_response, con_info->connection, con_info->response);

    return response;
}

int handle_create_blog(struct connection_info_struct *con_info){
    
    char *blog_title = extractValuesForJson(con_info->answerstring, "\"title\":");

    char *blog_body = extractValuesForJson(con_info->answerstring, "\"body\":");

    char *user_id = extractValuesForJson(con_info->answerstring, "\"user_id\":");
    
    int user_int_id = atoi(user_id);

    int create_blog_response = create_blog(blog_title, blog_body, user_int_id);

    return create_blog_response;

}

int handle_return_blogs(struct connection_info_struct *con_info){

    char *blogs_response = return_blogs();

    int response = manage_response(blogs_response, con_info->connection,con_info->response);

    return response;

}

int handle_update_blog(struct connection_info_struct *con_info){

    char *body = extractValuesForJson(con_info->answerstring, "\"body\":");
    char *title = extractValuesForJson(con_info->answerstring, "\"title\":");
    char *blog_id = extractValuesForJson(con_info->answerstring, "\"blog_id\":");
    printf("IDDD %s", blog_id);
    int blog_id_int = atoi(blog_id);
    int update_blog_response = update_blog(body, title, blog_id_int);

    return update_blog_response;
}