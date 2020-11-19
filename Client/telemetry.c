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
FifoParser daemon_fifo, personal_fifo;
int personal_fifo_id;

// Thread used by the callback checker.
pthread_t callback_thread;


// Should see how to not make it exit too fast, and break stuff.
void* CallbackTelemetryChecker(void* _)
{
    (void)(_);
    // Just try to read stuff from personal_fifo.
    while (1) {
        int err, token, channel_length, message_length;
        char channel[1000], message[1000];
        err = ParseInt(&personal_fifo, &token);
        if (!err)
            err |= ParseInt(&personal_fifo, &channel_length);
        if (!err)
            err |= ParseString(&personal_fifo, channel, channel_length);
        if (!err)
            err |= ParseInt(&personal_fifo, &message_length);
        if (!err)
            err |= ParseString(&personal_fifo, message, message_length);
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
    char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";
    char PERSONAL_FIFO_CHANNEL[100] = "/tmp/TelemetryClientNr";

    int err = FifoInit(&daemon_fifo, DAEMON_FIFO_CHANNEL, 0);
    
    if (err)
        return err;

    // Create and open FIFO.
    personal_fifo_id = getpid();
    AppendInt(PERSONAL_FIFO_CHANNEL + strlen(PERSONAL_FIFO_CHANNEL), personal_fifo_id);
    err = FifoInit(&personal_fifo, PERSONAL_FIFO_CHANNEL, 1);

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
    int err = pthread_mutex_lock(&callbacks_is_busy);
    err |= TreapErase(&callbacks, callback_id);
    pthread_mutex_unlock(&callbacks_is_busy);
    return err;
}

int CloseTelemetry()
{
    if (initialized) {
        int err = FifoClose(&daemon_fifo);
        err |= FifoClose(&personal_fifo);
        err |= ClearTreap(&callbacks);
        pthread_cancel(callback_thread);
        initialized = 0;
        return 0;
    }
    return 1;
}
