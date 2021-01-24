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
debug: daemon_debug client_debug

daemon: $(DAEMON_CFILES) $(SHARED_CFILES)
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(DAEMON_CFILES) -o daemon.out

daemon_debug: $(DAEMON_CFILES) $(SHARED_CFILES)
	$(CC) $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(DAEMON_CFILES) -o daemon.out -DDEBUG
	
client: $(SHARED_CFILES) $(CLIENT_CFILES)
	$(CC) -c $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(CLIENT_CFILES)
	ar rc libtelemetry.a fifo_parser.o  standard.o  telemetry.o  treap.o 

client_debug: $(SHARED_CFILES) $(CLIENT_CFILES)
	$(CC) -c $(FLAGS) $(SANITIZER) $(WARNINGS) $(SHARED_LIB) $(SHARED_CFILES) $(CLIENT_CFILES) -DDEBUG
	ar rc libtelemetry.a fifo_parser.o  standard.o  telemetry.o  treap.o 

demo: Demo/main.c libtelemetry.a
	$(CC) $(COMP_FLAGS) -o demo.out Demo/main.c libtelemetry.a

clean:
	rm *.out
	rm *.a
	rm *.o
