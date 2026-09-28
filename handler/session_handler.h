#ifndef SESSION_H
#define SESSION

struct connection_info_struct;

int create_session_user(char *token_h, int user_id);
int manage_session(struct connection_info_struct *con_info, char *token, int user_id);
char *return_session(struct connection_info_struct *con_info,char *token, char *username);
int authorize_session_token(char *token);
char *return_user_session(char *token);
void set_header(struct connection_info_struct *con_info, char *token);

#endif