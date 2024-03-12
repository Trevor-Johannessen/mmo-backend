#include "include/web-socket.h"
#include "include/buffered-reader.h"
#include <openssl/sha.h>

void create_web_socket(BufferedReader *br, HeaderList *headers){
    char *key = header_list_get_header(headers, "Sec-WebSocket-Key");
    char *hash = hash_key(key);
}


void sha1_hash(const char *input, unsigned char *hash) {
    SHA1((const unsigned char *)input, strlen(input), hash);
}

char *hash_key(char* key){
    const char *suffix = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char *concatenation;
    unsigned char *hash;
    int concat_len;

    if(!key) return 0x0;

    // concatenate key with suffix
    concat_len = strlen(key) + strlen(suffix);
    concatenation = (char*)malloc(concat_len+1);
    concatenation[concat_len] = '\0';
    strcpy(concatenation, key);
    strcpy(concatenation+strlen(key), suffix);
    hash = (char *)malloc(SHA_DIGEST_LENGTH+1);
    hash[SHA_DIGEST_LENGTH] = '\0';

    SHA1((const unsigned char *)concatenation, concat_len, hash);

    printf("Input String: %s\n", concatenation);
    printf("SHA-1 Hash: ");
    print_hash(hash);

    return hash;
}

void print_hash(unsigned char *hash){
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}