#ifndef UTILS_H
#define UTILS_H

const char *password_h(char *password);

char *hash_parts(char *hash_password, int occurance);

void get_hash_parts(char *hash_password, char **iter_out, char **salt_out, char **hash_out);

int password_verify(char *password, char *input_password, int iteration,char *salt);

unsigned char *recompute_hash_to_bytes(char *hex_str);

#endif