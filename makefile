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

client: $(CLIENT_CFILES) $(SHARED_CFILES)
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(CLIENT_CFILES) -o client.out
