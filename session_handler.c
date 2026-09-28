#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "handler/session_handler.h"
#include "handler/db.h"
#include "handler/user_handler.h"
#include "handler/utils.h"
#include "handler/server.h"

char *return_session(struct connection_info_struct *con_info, char *token, char *username){

    char *user_object = return_user(username);
    char *user_id = extractValuesForJson(user_object, "\"id\":");
    int user_id_num = atoi(user_id);
    int hash_user_session = manage_session(con_info, token, user_id_num);
    if(hash_user_session==1){
        return token;
    }else{
        return "ERROR";
    }

}

char *return_user_session(char *token){
    char *query = "SELECT user_id FROM sessions WHERE token_hash = decode($1, 'hex') AND expires_at > CURRENT_TIMESTAMP AND revoked_at IS NULL LIMIT 1;";
    const char *params[1] = {token};
    char *session_response = executeGetUserForAuth(query, params);

    return session_response;
}

int create_session_user(char *token_h, int user_id){

    const char *query =
        "INSERT INTO sessions (token_hash, user_id, expires_at) "
        "VALUES (decode($1, 'hex'), $2, CURRENT_TIMESTAMP + INTERVAL '1 day');";

    char user_id_buffer[12];
    snprintf(user_id_buffer, sizeof(user_id_buffer), "%d", user_id);
    const char *params[2] = {token_h, user_id_buffer};

    int session_response = executePostQueryToJson(query, 2, params);

    return session_response;

}

int manage_session(struct connection_info_struct *con_info, char *token, int user_id){

    char *hashed_token_session = token_h(token);

    int create_session_response = create_session_user(hashed_token_session, user_id);
    if(create_session_response==1){
        set_header(con_info, token);
    }
    else{
        return 0;
    }
    return create_session_response;

}

void set_header(struct connection_info_struct *con_info, char *token){
    
    con_info->set_cookie="KEY=<$token>; Path=/; HttpOnly; SameSite=Lax; Max-Age=86400";
}

int authorize_session_token(char *token){

    char *hash_user_token = token_h(token);

    char *valid_session = return_user_session(hash_user_token);
    if(strlen(valid_session) > 0){
        return 1;
    }
    else{
        return 0;
    }

}