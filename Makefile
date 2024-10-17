CC = g++
BENCH_CFLAGS = -std=c++11 -isystem ../benchmark/include -L../benchmark/build/src -lbenchmark -lpthread
TEST_CFLAGS = -O0 -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=address -g

.PHONY: all test notebook b64 clean

all: test

test: 
	$(CC) $(TEST_CFLAGS) select.c test.c -o test.out
	./test.out 

notebook:
	set -e; \
	TMP=$$(tar --transform "s,^,select/," -czf - Makefile *.c *.h | base64 -w 0) ;\
	cat template.ipynb | awk "{sub(/__TAR__B64__GOES__HERE__/,\"$$TMP\")}1" > notebook.ipynb

b64:
	tar --transform "s,^,select/," -czf - Makefile *.c *.h | base64 -w 0 && echo ""

benchmark:
	$(CC) -O2 select.c benchmark.c $(BENCH_CFLAGS) -o benchmark.out
	taskset -c 0 \
		./benchmark.out \
		--benchmark_out=benchmark.json \
		--benchmark_out_format=json

clean:
	rm -f test.out benchmark.out notebook.ipynb

