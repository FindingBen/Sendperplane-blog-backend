#ifndef USER_H
#define USER_H

const char *return_users(void);

const char *return_user(const char *username);

int create_user(const char *username, const char *email, const char *password_hash);

#endif