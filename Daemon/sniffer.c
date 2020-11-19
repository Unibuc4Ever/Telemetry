#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

#include "sniffer.h"
#include "fifo_parser.h"

FifoParser daemon_parser;

// Processes a request.
// A request is basically the path of a FIFO file.
void ProcessRequest(char* request)
{
    printf("Received request \'%s\'!\n", request);
    fflush(stdout);

    FifoParser request_parser;
    FifoInit(&request_parser, request_parser)
}

int StartRuntime()
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