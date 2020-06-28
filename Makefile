CC = gcc
CFLAGS = -Wall
LDFLAGS = -lpthread

.PHONY: all clean

all: monitor signal

monitor: monitor.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

signal: signal.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f monitor
	rm -f signal
