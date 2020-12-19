CC=gcc
FLAGS=-O0 -g
DAEMON_CFILES:=$(wildcard ./Daemon/*.c)
CLIENT_CFILES:=$(wildcard ./Client/*.c)
SHARED_CFILES:=$(wildcard ./Shared/*.c)
SHARED_LIB:=-IShared -IClient -pthread
SANITIZER:=-fsanitize=address,undefined,signed-integer-overflow
WARNINGS:=-Wall -Wextra

CLIENT_O_FILES:=$(patsubst %.c, %.o, $(CLIENT_CFILES) $(SHARED_CFILES))

COMP_FLAGS:=$(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB)

all: client daemon

daemon: $(DAEMON_CFILES) $(SHARED_CFILES)
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(DAEMON_CFILES) -o daemon.out

client: $(SHARED_CFILES) $(CLIENT_CFILES)
	$(CC) -c $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(CLIENT_CFILES)
	ar rcs telemetry.a fifo_parser.o  standard.o  telemetry.o  treap.o 

demo: client
	# $(CC) -c $(COMP_FLAGS) ./Demo/main.c -o main.o
	$(CC) -L -ltelemetry $(COMP_FLAGS) ./Demo/main.c 