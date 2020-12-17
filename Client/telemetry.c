#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

#include "fifo_parser.h"
#include "treap.h"

// Wheter InitializeTelemetry was called or not.
int initialized;

// DS storing all the callbacks.
Treap* callbacks;
// Flag for not having multhreading issues.
pthread_mutex_t callbacks_is_busy;

// Parsers.
static FifoParser daemon_fifo, personal_cb_fifo, personal_history_fifo;
static int personal_fifo_id;

// Thread used by the callback checker.
static pthread_t callback_thread;

 // Channel used for sending messages to the daemon.
static const char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";
static char PERSONAL_CB_CHANNEL[100] = "/tmp/TelemetryCallbackNr";
static char PERSONAL_HISTORY_CHANNEL[100] = "/tmp/TelemetryHistoryNr";

// Should see how to not make it exit too fast, and break stuff.
void* CallbackTelemetryChecker(void* _)
{
    (void)(_);
    // Just try to read stuff from personal_fifo.
    while (1) {
        int err, token, channel_length, message_length, op_code = -1;
        char channel[1000], message[1000];
        err = ParseInt(&personal_cb_fifo, &op_code);
        if (!err)
            err |= ParseInt(&personal_cb_fifo, &token);
        if (!err)
            err |= ParseInt(&personal_cb_fifo, &channel_length);
        if (!err)
            err |= ParseString(&personal_cb_fifo, channel, channel_length);
        if (!err)
            err |= ParseInt(&personal_cb_fifo, &message_length);
        if (!err)
            err |= ParseString(&personal_cb_fifo, message, message_length);
        if (err) {
            printf("Error while getting data from daemon: %d\n", err);
            return NULL;
        }
        err = pthread_mutex_lock(&callbacks_is_busy);
        if (err) {
            printf("Unable to lock mutex: ");
            perror(NULL);
            return NULL;
        }
        void* data;
        err = TreapFind(callbacks, token, &data);
        pthread_mutex_unlock(&callbacks_is_busy);

        printf("Received token %d from cannel %s, with message %s\n",
            token, channel, message);
        fflush(stdout);

        void(*callback)(const char* channel, const char* message) = data;
        (*callback)(channel, message);
    }
}

int GetSyncHistory(const char* channel, int max_entries, 
                   int* found_entries, char*** channels, char*** messages)
{
    return 0;
}

int GenerateRandomName()
{
    static int cnt = 1;
    return getpid() * 1000 + cnt++;
}

int InitializeTelemetry()
{
    if (initialized)
        return 0;

    // Channel used for sending messages to the daemon.
    int err = FifoInit(&daemon_fifo, DAEMON_FIFO_CHANNEL, 0);
    
    if (err)
        return err;

    personal_fifo_id = getpid();
    // Create and open callback FIFO.
    AppendInt(PERSONAL_CB_CHANNEL + strlen(PERSONAL_CB_CHANNEL), personal_fifo_id);
    err = FifoInit(&personal_cb_fifo, PERSONAL_CB_CHANNEL, 1);

    // Create and open history FIFO.
    AppendInt(PERSONAL_HISTORY_CHANNEL + strlen(PERSONAL_HISTORY_CHANNEL), personal_fifo_id);
    err |= FifoInit(&personal_history_fifo, PERSONAL_HISTORY_CHANNEL, 1);

    if (err)
        return err;

    // Launching new thread.
    err |= pthread_create(&callback_thread, NULL, CallbackTelemetryChecker, NULL);

    if (err)
        return err;

    initialized = 1;

    printf("Finished initialization!\n");
    fflush(stdout);
    return 0;
}

int BroadcastTelemetry(const char* channel, const char* message)
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    char broadcast_fifo[100] = "/tmp/TelemetryBroadcastNr";
    AppendInt(broadcast_fifo + strlen(broadcast_fifo), GenerateRandomName());

    FifoParser parser;
    int err = FifoInit(&parser, broadcast_fifo, 1);
    
    if (err)
        return err;

    err |= PrintInt(&parser, 1);
    err |= PrintInt(&parser, strlen(channel));
    err |= PrintString(&parser, channel, strlen(channel));
    err |= PrintInt(&parser, strlen(message));
    err |= PrintString(&parser, message, strlen(message));

    // err |= FifoClose(&parser);

    if (!err)
        err |= PrintString(&daemon_fifo, broadcast_fifo, strlen(broadcast_fifo));
    return err;
}

int RegisterCallback(const char* channel, void(*callback)(const char* channel, const char* message))
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    static int counter = 0;
    char broadcast_fifo[100] = "/tmp/TelemetryBroadcastNr";
    AppendInt(broadcast_fifo + strlen(broadcast_fifo), GenerateRandomName());

    FifoParser parser;
    counter++;
    int err = FifoInit(&parser, broadcast_fifo, 1);
    
    if (err)
        return err;

    err |= PrintInt(&parser, 2);
    err |= PrintInt(&parser, counter);
    err |= PrintInt(&parser, personal_fifo_id);
    err |= PrintInt(&parser, strlen(channel));
    err |= PrintString(&parser, channel, strlen(channel));
    // err |= FifoClose(&parser); -> TODO: how to fix this

    if (!err)
        err |= PrintString(&daemon_fifo, broadcast_fifo, strlen(broadcast_fifo));

    // Save callback in DS
    if (!err) {
        err |= pthread_mutex_lock(&callbacks_is_busy);
        err |= TreapInsert(&callbacks, counter, callback);
        pthread_mutex_unlock(&callbacks_is_busy);
    }

    if (err)
        return -1;
    return counter;
}

int RemoveRegisteredCallback(int callback_id)
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    int err = pthread_mutex_lock(&callbacks_is_busy);
    err |= TreapErase(&callbacks, callback_id);
    pthread_mutex_unlock(&callbacks_is_busy);
    if (err)
        return err;

    static int counter = 0;
    char broadcast_fifo[100] = "/tmp/TelemetryBroadcastNr";
    AppendInt(broadcast_fifo + strlen(broadcast_fifo), GenerateRandomName());

    FifoParser parser;
    counter++;
    err = FifoInit(&parser, broadcast_fifo, 1);
    
    if (err)
        return err;

    err |= PrintInt(&parser, 3);
    err |= PrintInt(&parser, callback_id);
    err |= PrintInt(&parser, personal_fifo_id);
    // err |= FifoClose(&parser); -> TODO: how to fix this

    if (!err)
        err |= PrintString(&daemon_fifo, broadcast_fifo, strlen(broadcast_fifo));

    return err;
    
}

int CloseTelemetry()
{
    if (initialized) {
        int err = FifoClose(&daemon_fifo);
        err |= FifoClose(&personal_cb_fifo);
        err |= FifoClose(&personal_history_fifo);
        
        pthread_cancel(callback_thread);
        err |= ClearTreap(&callbacks);
        unlink(PERSONAL_CB_CHANNEL);
        unlink(PERSONAL_HISTORY_CHANNEL);
        initialized = 0;
        return 0;
    }
    return 1;
}
