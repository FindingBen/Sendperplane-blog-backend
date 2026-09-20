#ifndef MAIN_H
#define MAIN_H

#include "server.h"

int handle_register(struct connection_info_struct *con_info);
int handle_return_user(struct connection_info_struct *con_info);
int handle_return_users(struct connection_info_struct *con_info);
int handle_main_page(struct connection_info_struct *con_info);

#endif