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

#include "fifo_parser.h"

FifoParser daemon_parser;

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
    
    int err = FifoInit(&daemon_parser, "/tmp/TelemetryRequests", 1);
    if (err) {
        printf("Unable to create pipe!");
        return err;
    }

    // Listen for requests.
    while (1) {
        char request[1000];
        ParseWord(&daemon_parser, request, 1000);

        ProcessRequest(request);
    } 
    return 0; 
} 
