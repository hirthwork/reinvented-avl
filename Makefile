COMMON_CFLAGS = -std=c99 -Wall -Wextra -Werror -pipe
OUTPUT_FILE = test
COVERAGE_DIR = cov

.PHONY: all clean

all: clean
	$(CC) $(COMMON_CFLAGS) $(CFLAGS) -fprofile-arcs -ftest-coverage -ggdb \
	    test.c -o $(OUTPUT_FILE)
	./$(OUTPUT_FILE)
	gcov -a -b -c -f test.c

clean:
	rm -rf $(OUTPUT_FILE){,.gcda,.gcno} *.c.gcov $(COVERAGE_DIR)

