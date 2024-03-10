typedef struct buffered_reader {
    int buf_size;
    int fd;
    char* buf;
} BufferedReader;

BufferedReader *br_init(int buf_size, int fd);
void br_destroy(BufferedReader *br);
char* br_read_line(BufferedReader *br, int *flag);
int br_read(BufferedReader *br, int start);