CC = g++
CFLAGS = -O2
TEST_CFLAGS = -O0 -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=address -g

.PHONY: all test clean

all: test

test: 
	$(CC) $(TEST_CFLAGS) select.c test.c -o test.out
	./test.out 

clean:
	rm -f test.out

