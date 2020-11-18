CC=gcc
FLAGS=-O0 -g
CFILES:=$(shell ls | grep .c$)
SANITIZER:=-fsanitize=address,undefined,signed-integer-overflow
WARNINGS:=-Wall -Wextra

all: 
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(CFILES)
run:
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(CFILES)
	./a.out