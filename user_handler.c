#include "handler/db.h"
#include <stdio.h>
#include <libpq-fe.h>


const char *return_users(){
    const char *query = "Select * from users;";

    const char *users_response = executeGetQueryToJson(query);

    return users_response;
}

const char *return_user(const char *username){
    const char *query = "SELECT * from users where username=$1;";
    const char *param[1] = {username};

    char *user_data = executeGetUserForAuth(query, param);

    return user_data;

}

int create_user(const char *username, const char *email, const char *password_hash){
    
    const char *query = "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3);";
    const char *params[3] = {username, email, password_hash};

    int create_user_response = executePostQueryToJson(query,params);

    return create_user_response;
    
}