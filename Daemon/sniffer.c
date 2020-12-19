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

void SendHistory(int PID, int entries_found, const char** channels, const char** messages)
{
    char personal_fifo_channel[100];
    strcpy(personal_fifo_channel, PERSONAL_RECEIVE_CHANNEL);
    AppendInt(personal_fifo_channel + strlen(personal_fifo_channel), PID);

    FifoParser parser;
    FifoInit(&parser, personal_fifo_channel, 0);

    PrintInt(&parser, 2);
    PrintInt(&parser, entries_found);
    for (int i = 0; i < entries_found; ++i) {
        PrintInt(&parser, strlen(channels[i]));
        PrintString(&parser, channels[i], strlen(channels[i]));
        PrintInt(&parser, strlen(messages[i]));
        PrintString(&parser, messages[i], strlen(messages[i]));
    }

    FifoClose(&parser);
}

void SendCallback(int PID, int token, char* channel, char* message)
{
    char personal_fifo_channel[100];
    strcpy(personal_fifo_channel, PERSONAL_RECEIVE_CHANNEL);
    AppendInt(personal_fifo_channel + strlen(personal_fifo_channel), PID);

    printf("before sending, fifo_name: add %d to %s\n", PID, personal_fifo_channel);

    FifoParser parser;
    FifoInit(&parser, personal_fifo_channel, 0);

    PrintInt(&parser, 1);
    PrintInt(&parser, token);
    PrintInt(&parser, strlen(channel));
    PrintString(&parser, channel, strlen(channel));
    PrintInt(&parser, strlen(message));
    PrintString(&parser, message, strlen(message));

    FifoClose(&parser);

    printf("Sent callback:\n");
    printf("    PID: %d\n    Token: %d\n", PID, token);
    printf("    Channel: %s\n    Message: %s\n", channel, message);
}

void ProcessBroadcastRequest(FifoParser* parser)
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

    if (!isValidPath(channel)) {
        printf(" !!! Broadcast denied because channel invalid:\n    `%s`\n", channel);
        return ;
    }

    /// Tell the clients to call their callbacks
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

    /// Save the message in history
    HistoryStorageAdd(channel, message);
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

    if (!isValidPath(channel)) {
        printf(" !!! Callback Token Request denied, for wrong channel:\n\
    `%s`\n", channel);
        return ;
    }

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

void ProcessHistoryRequest(FifoParser* parser)
{
    printf("Received History request:\n");
    /// Read the request
    int max_entries, channel_length, personal_fifo_id;
    char channel[100];

    int err = 0;
    err |= ParseInt(parser, &personal_fifo_id);
    err |= ParseInt(parser, &max_entries);
    err |= ParseInt(parser, &channel_length);
    if (!err)
        err |= ParseString(parser, channel, channel_length);

    if (err)
        return ;

    if (!isValidPath(channel)) {
        printf(" !!! History Request denied, for wrong channel:\n\
    `%s`\n", channel);
        return ;
    }

    // print request
    const char** history_channels;
    const char** history_messages;
    int nr_entries;
    err |= HistoryStorageQuery(max_entries, channel, 
                &history_channels, &history_messages, &nr_entries);

    if (err)
        return ;

    printf("Performed history request, nr results: %d\n", nr_entries);

    SendHistory(personal_fifo_id, nr_entries, history_channels, history_messages);

    free(history_channels);
    free(history_messages);
    // TODO: Fix memory leaks.
}

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
        printf(" -- Error in routine, error: %d\n", err);
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
        char request[1000];
        ParseWord(&daemon_parser, request, 1000);

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