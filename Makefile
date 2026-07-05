CC = gcc
CFLAGS = -Wall -Wextra

cloc: main.c
	$(CC) $(CFLAGS) -o cloc main.c

build: cloc

run: cloc
	./cloc .

clean:
	rm -f cloc *.o

.PHONY: build run clean
