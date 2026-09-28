#include "handler/auth.h"
#include "handler/user_handler.h"
#include "handler/db.h"
#include "handler/utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/rand.h>


int authenticate_user(const char *username, char *user_password){

    if(username==NULL){
        return 0;
    }

    char *user_data = return_user(username);

    if(user_data == NULL){
        return 0;
    }

    char *hashed_p = extractValuesForJson(user_data, "\"password_hash\":");


    char *iterator = hash_parts(hashed_p,0);
    
    char *salt = hash_parts(hashed_p,1);
    char *hash = hash_parts(hashed_p,2);

    char *endptr;
    long val = strtol(iterator, &endptr, 10);
    
    int auth_result = password_verify(hash, user_password, val, salt);

    return auth_result == 0;

}


char *generate_cookie(void){
    static const char hex_digits[] = "0123456789abcdef";
    unsigned char random_bytes[32];
    if (RAND_bytes(random_bytes, sizeof(random_bytes)) != 1) {
        return NULL;
    }

    char *value = malloc(69);
    if (value == NULL) {
        return NULL;
    }

    memcpy(value, "KEY=", 4);
    for (size_t i = 0; i < sizeof(random_bytes); i++) {
        value[4 + i * 2] = hex_digits[random_bytes[i] >> 4];
        value[5 + i * 2] = hex_digits[random_bytes[i] & 0x0f];
    }
    value[68] = '\0';
    return value;
}
