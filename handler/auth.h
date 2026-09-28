#ifndef AUTH_H
#define AUTH_H

int authenticate_user(const char *username, char *user_password);
char *generate_cookie(void);

#endif