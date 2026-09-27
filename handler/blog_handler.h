#ifndef BLOG_H
#define BLOG_H


int create_blog(const char *title, const char *body,int user_id);
const char *return_blog(const int blog_id);
const char *return_blogs();
int update_blog(char *body, char *title, int body_id);

#endif