#include <sys/socket.h>
#include <netdb.h>

typedef struct header_node {
    char *header_name;
    char *header_value;
    struct header_node *next_header;
} HeaderList;

typedef struct {
    int fd;
    struct sockaddr_storage client_addr;
} ConnInfo;

int open_listenfd(char *port);
void *web_server_handle_client(void* fd_arg);
void web_server_get_route(BufferedReader *br, char *method, char *route, char *args);
HeaderList *web_server_get_headers(BufferedReader *br);
void destroy_header_list(HeaderList *head);
void print_header_list(HeaderList *head);
void *error(char* msg, char *hostname, char *port, BufferedReader *br);


#define MAX_LINE 1024 // lines cannot exceed 1023 characters
#define MAX_HEADER 128 // header names cannot exceed 127 characters