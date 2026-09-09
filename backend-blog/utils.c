#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>
#include "handler/utils.h"


#define HASH_L 32
#define SALT_L 16
#define HASH_H 64
#define SALT_H 32
#define ITER 100000

static void to_hex(int src_l,unsigned char *src, unsigned char *dst_src){
 char *h = "0123456789abcdef"; //hex characters
 for(int i = 0; i < src_l;++i){
    dst_src[i*2] = h[src[i] >> 4];
    dst_src[i*2+1] = h[src[i] & 0xF];
 }
 dst_src[src_l * 2] = '\0';

}
//26f70111920d0317ac8d0427eaf138cef4946b33d0ad3cb186f6a1ca801b0fc3B
//7ad7027f1b29b8dae8a8a650ed6e9704
//26f70111920d0317ac8d0427eaf138cef4946b33d0ad3cb186f6a1ca801b0fc3
static void from_hex(size_t hex_l,char *hex_str, char *byte_str){
    printf("STRIG %s", hex_str);
    int r = hex_l % 2;
    if(r == 0){
        size_t byte_count = hex_l / 2;
        for(size_t i = 0; i < byte_count; ++i){
            
        }
    }
}

const char *password_h(char *password){
    
    unsigned char *hash = malloc(HASH_L);
    unsigned char *salt = malloc(SALT_L);
    unsigned char out[32];

    size_t pass_len = strlen(password);

    if(password == NULL){

        return NULL;
        
    }

    int salt_result = RAND_bytes(salt,SALT_L);

    if(salt_result != 1){
        unsigned long err = ERR_get_error();
        char errbuf[256];
        ERR_error_string_n(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "PBKDF2 failed: %s\n", errbuf);
    }

    int hash_result = PKCS5_PBKDF2_HMAC(password,pass_len,salt,SALT_L,ITER, EVP_sha256(),HASH_L,out);

    if(hash_result != 1){
        unsigned long err = ERR_get_error();
        char errbuf[256];
        ERR_error_string_n(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "PBKDF2 failed: %s\n", errbuf);
    }


    unsigned char *salt_hex = malloc(SALT_H + 1); // +1 for the null terminator to_hex writes
    unsigned char *hash_hex = malloc(HASH_H + 1); // +1 for the null terminator to_hex writes

    to_hex(SALT_L, salt, salt_hex);
    to_hex(HASH_L, out, hash_hex);

    char iter_str[12];
    snprintf(iter_str, sizeof(iter_str), "%d", ITER);

    size_t total_len = strlen(iter_str) + 1 + strlen(salt_hex) + 1 + strlen(hash_hex) + 1;
    char *combined = malloc(total_len);

    snprintf(combined, total_len, "%s$%s$%s", iter_str, salt_hex, hash_hex);

    return combined;

}
//100000$28$21266238

int password_verify(char *password, char *input_password, int iteration,char *salt){

    unsigned char *hash = malloc(HASH_L);
    unsigned char *original_p = malloc(256);
    unsigned char *original_s = malloc(SALT_L);

    size_t pass_len = strlen(input_password);

    if(input_password == NULL){
        return 1;
    }

    original_s = recompute_hash_to_bytes(salt);
    original_p = recompute_hash_to_bytes(password);
    printf("COMMMM %s", original_s);
    int hash_result = PKCS5_PBKDF2_HMAC(input_password,pass_len,original_s,SALT_L,iteration, EVP_sha256(),HASH_L,hash);

    if(hash_result != 1){
        unsigned long err = ERR_get_error();
        char errbuf[256];
        ERR_error_string_n(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "PBKDF2 failed: %s\n", errbuf);
    }

    int result = CRYPTO_memcmp(hash, original_p, HASH_L);
    
    return result;
}

unsigned char *recompute_hash_to_bytes(char *hex_str){

    unsigned char out[32];
    unsigned char *bytes;
    
    printf("HEX_STR %s", hex_str);
    size_t hex_l = strlen(hex_str) / 2;

    printf("SIZEE %zu", hex_l);

    if(hex_l == SALT_L){
        bytes = malloc(SALT_L);
    }
    else if(hex_l == HASH_L){
        bytes = malloc(SALT_L);
    }
    else{
        return NULL;
    }
    printf("HEXssss %s", hex_str);
    from_hex(hex_l,hex_str,bytes);
    printf("BYTES %s",bytes);
    
    return bytes;

}

char *hash_parts(char *hash_password, int occurance){
    
    
    if(occurance==0){
        size_t size_iter = strcspn(hash_password,"$");
        char *extracted_iterator = malloc(size_iter);
        char *iterator = strncpy(extracted_iterator, hash_password, size_iter);
        return iterator;
    }
    else if(occurance==1){
        char *sal_val = strchr(hash_password, '$') + 1;

        size_t sal_size = strcspn(sal_val,"$");
        char *extracted_sal = malloc(sal_size);
        char *salt = strncpy(extracted_sal, sal_val,sal_size);
        return salt;
    }
    else if(occurance==2){
        char *sal_val = strchr(hash_password, '$') + 1;
        char *hash_val = strchr(sal_val,'$') + 1;
        size_t hash_size = strcspn(sal_val,"");
        char *extracted_hash = malloc(hash_size);
        char *hash = strncpy(extracted_hash, hash_val, hash_size);

        return hash;
    }

}