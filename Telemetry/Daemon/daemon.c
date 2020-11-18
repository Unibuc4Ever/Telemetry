/**
 * This is not yet a full daemon, as it's still missing some important features.
 * It's used for development purpuses.
 */

#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

// Path of the FIFO channel used by the Daemon.
char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";

// File descriptor of the pipe.
int fd;

// Buffer with the extracted data from the pipe.
#define DIM 100
char buffer[DIM];
// Position in the buffer.
int p_act;

// Returns one character from the buffer.
char GetBufferChar()
{
    if (!buffer[p_act]) {
        memset(buffer, 0, p_act);
        read(fd, buffer, sizeof(buffer));
        p_act = 0; 
    }

    return buffer[p_act++];
}

// Processes a request.
// A request is basically the path of FIFO file.
void ProcessRequest(char* request)
{
    // TODO:
    printf("Received request \'%s\'!\n", request);
    fflush(stdout);
}

int main()
{
    printf("Initialization of the Daemon...\n");
    printf("Creating pipe \'%s\'...\n", DAEMON_FIFO_CHANNEL);
    fflush(stdout);

    // Create and open FIFO.
    int stat = mkfifo(DAEMON_FIFO_CHANNEL, S_IRUSR | S_IWUSR);
    
    if (stat == -1) {
        printf("Unable to create pipe!\n");
        char message[100];
        perror(message);
        printf("Error message: %s\n", message);
    }
    else
        printf("Successfully created the pipe!\n");
    fflush(stdout);
    
    // Open file descriptor.
    fd = open(DAEMON_FIFO_CHANNEL, O_RDWR);
    if (fd == -1) {
        printf("Unable to open file descriptor!\n");
        char message[100];
        perror(message);
        printf("Error message: %s\n", message);
        printf("Exiting...\n");
        exit(errno);
    }
    else
        printf("Successfully opened file descriptor!\n");
    
    printf("Starting to listen for requests...\n");
    fflush(stdout);

    // Listen for requests.
    while (1) {
        char request[1000];
        memset(request, 0, sizeof request);
        int p = 0;

        char c;
        while (1) {
            c = GetBufferChar();
            if (c == 0) {
                printf("Unexpected character: \'\\0\'. Exiting...\n");
                exit(-1);
            }
            if (c == ' ' || c == '\n')
                break;
            request[p++] = c;
        }

        ProcessRequest(request);
    } 
    return 0; 
} 
