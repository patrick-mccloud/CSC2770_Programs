CC = gcc
CFLAGS = -Wall -Wextra -pthread
all: Threaded
Threaded: ThreadedMatrixMultiply.c
	$(CC) $(CFLAGS) -o Threaded ThreadedMatrixMultiply.c

clean:
	rm -f Threaded
