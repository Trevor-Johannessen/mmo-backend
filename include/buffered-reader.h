typedef struct buffered_reader {
    char* buf;
    int fd;
    int flags;
    int buf_size;
} BufferedReader;

BufferedReader *br_init(int fd, int buf_size);
void br_destroy(BufferedReader *br);
char* br_read_line(BufferedReader *br);
int br_read(BufferedReader *br, int start);