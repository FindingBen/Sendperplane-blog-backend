#include <string.h>
#include "handler/main_handlers.h"
#include "handler/server.h"
#include "handler/user_handler.h"
#include "handler/db.h"
#include "handler/utils.h"

int handle_register(struct connection_info_struct *con_info){

    char *username = extractValuesForJson(con_info->answerstring, "\"username\":");
    char *email = extractValuesForJson(con_info->answerstring, "\"email\":");
    char *password = extractValuesForJson(con_info->answerstring, "\"password_hash\":");

    const char *password_hash = password_h(password);

    int create_response = create_user(username, email, password_hash);

    return create_response;
}

int handle_return_user(struct connection_info_struct *con_info){

    char *username = con_info->answerstring;

    char *user_response = return_user(username);

    int response = manage_response(user_response, con_info->connection,con_info->response);

    return response;

}

int handle_return_users(struct connection_info_struct *con_info){
    char *username = con_info->answerstring;

    char *user_response = return_users();

    int response = manage_response(user_response, con_info->connection,con_info->response);

    return response;
}

int handle_main_page(struct connection_info_struct *con_info){
    printf("HERE");
    const char *home_page_response = home_page();
    printf("\n AAAAA");
    int response = manage_response(home_page_response, con_info->connection, con_info->response);

    return response;
}