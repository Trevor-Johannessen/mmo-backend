# Find all the C files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name  '*.c' ! -name 'test_*.c' ! -name 'main.c')
TEST := $(shell find $(SRC_DIRS) -name 'test_*.c')
PKGS := -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libmongoc-1.0 -I/usr/include/libbson-1.0 -lglib-2.0 -lssl -lcrypto -lmongoc-1.0 -lbson-1.0
OBJS := $(shell find $(SRC_DIRS) -name  '*.o')

all: 
	gcc -o out main.c $(SRCS) $(PKGS)
	
test:
	gcc -g -o test $(SRCS) $(PKGS) $(TEST) -lcriterion

debug:
	gcc -g -o out main.c $(SRCS) $(PKGS) 

clean:
	rm -f $(OBJS)
	rm -f out
	rm -f test

.PHONY: all clean debug