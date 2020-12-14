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
#include "storage.h"

FifoParser daemon_parser;

void SendCallback(int PID, int token, char* channel, char* message)
{
    char personal_fifo_channel[100] = "/tmp/TelemetryClientNr";
    AppendInt(personal_fifo_channel + strlen(personal_fifo_channel), PID);

    FifoParser parser;
    FifoInit(&parser, personal_fifo_channel, 0);

    PrintInt(&parser, token);
    PrintInt(&parser, strlen(channel));
    PrintString(&parser, channel, strlen(channel));
    PrintInt(&parser, strlen(message));
    PrintString(&parser, message, strlen(message));

    printf("Sent callback:\n");
    printf("    PID: %d\n    Token: %d\n", PID, token);
    printf("    Channel: %s\n    Message: %s\n", channel, message);
}

void ProcessBroadcast(FifoParser* parser)
{
    printf("Received broadcast request:\n");
    char channel[1000], message[1000];
    memset(channel, 0, sizeof channel);
    memset(message, 0, sizeof message);

    int err, channel_length, message_length;
    err = ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);
    if (!err)
        err |= ParseInt(parser, &message_length);
    if (!err)
        err |= ParseString(parser, message, message_length);

    if (err) {
        printf("Received error %d\n", err);
        return;
    }

    printf("    Message length: %d\n    Message: %s\n", message_length, message);
    printf("    Channel length: %d\n    Channel: %s\n", channel_length, channel);

    Callback* callbacks;
    int number_of_callbacks;
    err = StorageGetCallbacks(channel, &callbacks, &number_of_callbacks);

    // Nothing to do.
    if (err) {
        printf("Found no callbacks: %d\n", err);
        return;
    } 

    for (int i = 0; i < number_of_callbacks; i++)
        SendCallback(callbacks[i].PID, callbacks[i].token,
                     channel, message);
}

void ProcessCallbackRequest(FifoParser* parser)
{
    char channel[1000];
    int pid, token;
    memset(channel, 0, sizeof channel);

    int err, channel_length;
    err = ParseInt(parser, &token);
    if (!err)
        err |= ParseInt(parser, &pid);
    if (!err)
        err |= ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);
    
    printf("Received callback request at:\n");
    printf("    channel: \'%s\'\n", channel);
    printf("    token: %d\n", token);
    printf("    PID: %d\n", pid);
    fflush(stdout);

    Callback callback = { pid, token };
    StorageAdd(callback, channel);
}

void ProcessCallbackCancelRequest(FifoParser* parser)
{
    printf("Received callback cancel request:\n");
    int pid, token;

    int err;
    err = ParseInt(parser, &token);
    if (!err)
        err |= ParseInt(parser, &pid);
    
    printf("Received callback cancel request at:\n");
    printf("    token: %d\n", token);
    printf("    PID: %d\n", pid);
    fflush(stdout);

    Callback callback = { pid, token };
    StorageRemove(callback);
}

// Processes a request.
// A request is basically the path of a FIFO file.
int ProcessRequest(char* request)
{
    printf(" --- Received request \'%s\'!\n", request);
    fflush(stdout);

    FifoParser request_parser;
    FifoInit(&request_parser, request, 0);

    int type, err;
    err = ParseInt(&request_parser, &type);

    if (!err && type == 1)
        ProcessBroadcast(&request_parser);
    else if (!err && type == 2)
        ProcessCallbackRequest(&request_parser);
    else if (!err && type == 3)
        ProcessCallbackCancelRequest(&request_parser);
    else
        err = 1;

    err |= FifoClose(&request_parser);
    err |= unlink(request);

    return err;
}

int StartRuntime()
{
    printf("Initialization of the Daemon...\n");
    
    int err = FifoInit(&daemon_parser, "/tmp/TelemetryRequests", 0);
    if (err) {
        printf("Unable to create pipe!");
        return err;
    }

    // Listen for requests.
    while (1) {
        char request[1000];
        ParseWord(&daemon_parser, request, 1000);

        printf("Processed with error %d\n", ProcessRequest(request));
        fflush(stdout);
    } 
    return 0; 
}