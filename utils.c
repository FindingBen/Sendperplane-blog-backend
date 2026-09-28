#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>
#include "handler/utils.h"
#include <ctype.h>


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

static void from_hex(size_t hex_l,char *hex_str, char *byte_str){
        if(strlen(hex_str) != hex_l*2){
            printf("NOT EQUAL");
            return;
        }

        unsigned char high_nibble;
        
        unsigned char low_nibble;
        for(size_t i = 0; i < hex_l; ++i){
            char high = hex_str[i*2];
            char low  = hex_str[i*2 + 1];

            if(high >= '0' && high <= '9')      high_nibble = high - '0';
            else if(high >= 'a' && high <= 'f') high_nibble = high - 'a' + 10;
            else if(high >= 'A' && high <= 'F') high_nibble = high - 'A' + 10;
            else { /* invalid — handle error */ }

            if(low >= '0' && low <= '9')      low_nibble = low - '0';
            else if(low >= 'a' && low <= 'f') low_nibble = low - 'a' + 10;
            else if(low >= 'A' && low <= 'F') low_nibble = low - 'A' + 10;
            else { /* invalid — handle error */ }

            byte_str[i] = (high_nibble << 4) | low_nibble;
        }
}

char *token_h(char *token){
    static const char hex_digits[] = "0123456789abcdef";
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int token_hash_length = 0;

    if (token == NULL) {
        return NULL;
    }

    const char *token_value = strchr(token, '=');
    token_value = token_value == NULL ? token : token_value + 1;

    if (EVP_Digest(token_value, strlen(token_value),
                digest, &token_hash_length,
                EVP_sha256(), NULL) != 1 ||
        token_hash_length != 32) {
        return NULL;
    }

    char *token_hash = malloc(65);
    if (token_hash == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < token_hash_length; i++) {
        token_hash[i * 2] = hex_digits[digest[i] >> 4];
        token_hash[i * 2 + 1] = hex_digits[digest[i] & 0x0f];
    }
    token_hash[64] = '\0';
    return token_hash;
}

const char *password_h(char *password){
    
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
    if(original_s == NULL || original_p == NULL){
    fprintf(stderr, "Failed to decode stored salt/hash — corrupt data?\n");
        return 1;
    }
    
    printf("\n");
    int hash_result = PKCS5_PBKDF2_HMAC(input_password,pass_len,original_s,SALT_L,iteration, EVP_sha256(),HASH_L,hash);
    printf("HASH RESULT %d", hash_result);
    if(hash_result != 1){
        unsigned long err = ERR_get_error();
        char errbuf[256];
        ERR_error_string_n(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "PBKDF2 failed: %s\n", errbuf);
    }

    int result = CRYPTO_memcmp(hash, original_p, HASH_L);
    printf("FINAL RESULT %d", result);
    return result;
}

unsigned char *recompute_hash_to_bytes(char *hex_str){

    unsigned char *bytes;
    
    size_t hex_l = strlen(hex_str) / 2;

    printf("SALT L %d",hex_l);
    if(hex_l == SALT_L){
        bytes = malloc(SALT_L);
    }
    else if(hex_l == HASH_L){
        bytes = malloc(HASH_L);
    }
    else{
        printf("NULLLL — hex_l (%zu) matched neither SALT_L (%d) nor HASH_L (%d)\n", hex_l, SALT_L, HASH_L);
        return NULL;
    }
    printf("HEX STR \n %s", hex_str);
    from_hex(hex_l,hex_str,bytes);
    
    return bytes;

}

char *hash_parts(char *hash_password, int occurance){
    
    
    if(occurance==0){
        size_t size_iter = strcspn(hash_password,"$");
        char *extracted_iterator = malloc(size_iter+1);
        if (extracted_iterator == NULL) {
            return NULL;
        }
        strncpy(extracted_iterator, hash_password, size_iter);
        extracted_iterator[size_iter] = '\0';
        return extracted_iterator;
    }
    else if(occurance==1){
        char *sal_val = strchr(hash_password, '$') + 1;

        size_t sal_size = strcspn(sal_val,"$");
        char *extracted_sal = malloc(sal_size+1);
        strncpy(extracted_sal, sal_val,sal_size);
        extracted_sal[sal_size] ='\0';
        return extracted_sal;
    }
    else if(occurance==2){
        char *sal_val = strchr(hash_password, '$') + 1;
        char *hash_val = strchr(sal_val,'$') + 1;
        size_t hash_size = strcspn(sal_val,"");
        char *extracted_hash = malloc(hash_size+1);
        strncpy(extracted_hash, hash_val, hash_size);
        extracted_hash[hash_size] = '\0';
        
        return extracted_hash;
    }

}