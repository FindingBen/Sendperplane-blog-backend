#include "handler/db.h"
#include <stdio.h>
#include <libpq-fe.h>


const char *return_blogs(){
    const char *query = "Select * from blogs";
}

const char *return_blog(const int blog_id){
    const char *query = "Select * from blogs where id=$1";
    const char param[1] = {blog_id};
}

int create_blog(const char *title, const char *body,const int user_id){
    const char *query = "INSERT INTO blogs (title, body, user_id) VALUES ($1, $2, $3)";
    const char *params[3] = {title, body, user_id};

    int create_blog_response = executePostQueryToJson(query, params);

    return create_blog_response;
}