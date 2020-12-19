#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <time.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

#include "sniffer.h"

#define CHECK_INTERVAL  (1 * 1 * 5) // in seconds

FifoParser daemon_parser;

static int last_checked_seconds = 0;

// Processes a request.
// A request is basically the path of a FIFO file.
int ProcessRequest(char* request)
{
    printf(" ===== Received request \'%s\'!\n", request);
    fflush(stdout);

    FifoParser request_parser;
    FifoInit(&request_parser, request, 0);

    int type, err;
    err = ParseInt(&request_parser, &type);

    if (!err && type == 1)
        ProcessBroadcastRequest(&request_parser);
    else if (!err && type == 2)
        ProcessCallbackRequest(&request_parser);
    else if (!err && type == 3)
        ProcessCallbackCancelRequest(&request_parser);
    else if (!err && type == 4)
        ProcessHistoryRequest(&request_parser);
    else
        err = 1;

    err |= FifoClose(&request_parser);
    err |= unlink(request);

    return err;
}

int PeriodicRoutine()
{
    printf(" --- Periodic Routine --- \n");
    int err = 0;
    err |= CallbackDeleteForNonexistentPID();
    int nr_deleted = HistoryDeleteTooOldMessages();
    
    if (nr_deleted > 0)
        printf(" --- Deleted %d expired messages\n", nr_deleted);

    if (err)
        printf(" --- Error in routine, error: %d\n", err);
    return err;
}

int StartRuntime()
{
    printf("Initialization of the Daemon...\n");
    
    int err = FifoInit(&daemon_parser, DAEMON_FIFO_CHANNEL, 0);
    if (err) {
        printf("Unable to create pipe!");
        return err;
    }

    // Listen for requests.
    while (1) {
        char request[2 * MAX_LENGTH_FIFO_NAME + 1];
        ParseWord(&daemon_parser, request, 2 * MAX_LENGTH_FIFO_NAME);

        printf("Processed with error %d\n", ProcessRequest(request));
        fflush(stdout);

        int current_time_seconds = time(NULL);
        if (current_time_seconds - last_checked_seconds > CHECK_INTERVAL) {
            PeriodicRoutine();
            last_checked_seconds = current_time_seconds;
        }
    } 
    return 0; 
}