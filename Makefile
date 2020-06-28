CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread

.PHONY: all clean

all: monitor

monitor: monitor.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f monitor
