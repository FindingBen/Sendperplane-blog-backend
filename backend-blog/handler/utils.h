#ifndef UTILS_H
#define UTILS_H

const char *password_h(char *password);

char *hash_parts(char *hash_password, int occurance);

void get_hash_parts(char *hash_password, char **iter_out, char **salt_out, char **hash_out);

#endif