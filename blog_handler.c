#include "handler/db.h"
#include <stdio.h>
#include <libpq-fe.h>
#include <stdlib.h>
#include <string.h>


const char *return_blogs(){
    const char *query = "Select * from blogs";

    const char *blogs_response = executeGetQueryToJson(query);

    return blogs_response;
}

const char *return_blog(const int blog_id){
    const char *query = "Select * from blogs where id=$1";
    char blog_id_buffer[12];
    snprintf(blog_id_buffer, sizeof(blog_id_buffer), "%d", blog_id);
    const char *params[1] = {blog_id_buffer};

    const char *blog_response= executeGetUserForAuth(query, params);

    return blog_response;
}

int create_blog(const char *title, const char *body, int user_id){
    const char *query = "INSERT INTO blogs (title, body, user_id) VALUES ($1, $2, $3)";
    
    char user_id_buffer[100];
    snprintf(user_id_buffer, sizeof(user_id_buffer), "%d", user_id);
    
    const char *params[3] = {title, body, user_id_buffer};

    int create_blog_response = executePostQueryToJson(query, 3, params);

    return create_blog_response;
}

int update_blog(char *body, char *title, int body_id){

    const char *query = "UPDATE blogs SET title = COALESCE($1, title), body = COALESCE($2, body), updated_at = CURRENT_TIMESTAMP WHERE id=$3";
    if (title == NULL && body == NULL) {
        return 0;
    }

    char body_id_buffer[12];
    snprintf(body_id_buffer, sizeof(body_id_buffer), "%d", body_id);

    const char *params[3] = {title, body, body_id_buffer};
    int update_blog_response = executePostQueryToJson(query, 3, params);
    return update_blog_response;


}