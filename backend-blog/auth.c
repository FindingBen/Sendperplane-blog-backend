#include "handler/auth.h"
#include "handler/user_handler.h"
#include "handler/db.h"
#include "handler/utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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

    int auth_result = validate_pass(user_password, iterator, salt, hash);

    return auth_result;


}


int validate_pass(char *user_password, int iterator, char *salt_h, char *hash_h){

    unsigned char *hashed_p = password_h(user_password);

    unsigned char *user_hash = recompute_hash_to_bytes(hash_h);

}