CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: HTTP_Server
HTTP_Server: HTTP_Server.c
	$(CC) $(CFLAGS) -o server HTTP_Server.c
clean:
	rm -f HTTP_Server
