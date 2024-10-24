# Makefile for DNS ANY Reply Simulation

CC = gcc
CFLAGS = -Wall -Wextra
TARGET = dns_any_reply
SRC = dns_any_reply.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
