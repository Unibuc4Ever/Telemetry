CC=gcc
FLAGS=-l./-O3 -Wall -c
CFILES:=$(shell ls | grep .c$)
SANITIZER:=-fsanitize=address,undefined,signed-integer-overflow
WARNINGS:=-Wall -Wextra

all: 
	$(CC) $(SANITIZER) $(WARNINGS) $(CFILES)