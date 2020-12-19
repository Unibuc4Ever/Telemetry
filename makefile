CC=gcc
FLAGS=-O0 -g
DAEMON_CFILES:=$(shell find . | grep "\./Daemon.*c$$")
CLIENT_CFILES:=$(shell find . | grep "\./Client.*c$$")
SHARED_CFILES:=$(shell find . | grep "\./Shared.*c$$")
SHARED_LIB:=-IShared -pthread
SANITIZER:=-fsanitize=address,undefined,signed-integer-overflow
WARNINGS:=-Wall -Wextra

all: client daemon

daemon: $(DAEMON_CFILES) ${SHARED_CFILES}
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(DAEMON_CFILES) -o daemon.out
	
client: $(SHARED_CFILES) $(CLIENT_CFILES)
	$(CC) -c $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(CLIENT_CFILES)
	ar rc libtelemetry.a fifo_parser.o  standard.o  telemetry.o  treap.o 

demo: client
	$(CC) $(COMP_FLAGS) -o main.out Demo/main.c libtelemetry.a