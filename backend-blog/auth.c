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
    printf("HEREEE %s \n", hash);
    char *endptr;
    long val = strtol(iterator, &endptr, 10);
    printf("ITERR %d", val);
    int auth_result = password_verify(hash, user_password, val, salt);

    return auth_result;


}
