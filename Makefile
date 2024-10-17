CC = g++
BENCH_CFLAGS = -std=c++11 -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread
TEST_CFLAGS = -O0 -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=address -g

.PHONY: all test clean

all: test

test: 
	$(CC) $(TEST_CFLAGS) select.c test.c -o test.out
	./test.out 

benchmark:
	$(CC) -O2 select.c benchmark.c $(BENCH_CFLAGS) -o benchmark.out
	./benchmark.out --benchmark_out=benchmark.json --benchmark_out_format=json
clean:
	rm -f test.out benchmark.out

