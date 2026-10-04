#ifndef MAIN_H
#define MAIN_H

#include "server.h"

HTTP_response handle_register(struct connection_info_struct *con_info);
HTTP_response handle_return_user(struct connection_info_struct *con_info);
HTTP_response handle_return_users(struct connection_info_struct *con_info);
HTTP_response handle_main_page(struct connection_info_struct *con_info);
HTTP_response handle_create_blog(struct connection_info_struct *con_info);
HTTP_response handle_return_blogs(struct connection_info_struct *con_info);
HTTP_response handle_return_blog(struct connection_info_struct *con_info);
HTTP_response handle_update_blog(struct connection_info_struct *con_info);
HTTP_response handle_login(struct connection_info_struct *con_info);
HTTP_response handle_logout(struct connection_info_struct *con_info);

#endif