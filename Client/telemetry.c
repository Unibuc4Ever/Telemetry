#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <assert.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

#include "telemetry.h"
#include "fifo_parser.h"
#include "treap.h"

// Wheter InitializeTelemetry was called or not.
static int initialized;

// DS storing all the callbacks.
static Treap* callbacks;
// Flag for not having multhreading issues.
static pthread_mutex_t callbacks_is_busy = PTHREAD_MUTEX_INITIALIZER;

// Parsers.
static FifoParser daemon_fifo, personal_receive_fifo;
static int personal_fifo_id;

// Thread used by the callback checker.
static pthread_t callback_thread;

// Channels used for sending messages to the daemon.
static const char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";
static char PERSONAL_QUERY_CHANNEL[100] = "/tmp/TelemetryQueryNr";
// Channels used to receive messages from the daemon 
static char PERSONAL_RECEIVE_CHANNEL[100] = "/tmp/TelemetryReceiveNr";

static pthread_mutex_t receiving_history = PTHREAD_MUTEX_INITIALIZER;
static int nr_received;
static char** pt_received_channels;
static char** pt_received_messages;

int GenerateRandomInt()
{
    static int cnt = 0;
    return cnt++ % 1000 + 1;
}

char* GenerateRandomFifoName()
{
    static char random_fifo_name[100];
    strcpy(random_fifo_name, PERSONAL_QUERY_CHANNEL);
    AppendInt(random_fifo_name + strlen(random_fifo_name), GenerateRandomInt());
    printf("Generate random fifo name: %s\n", random_fifo_name);
    return random_fifo_name;
}

int HandleCallback()
{
    int token, channel_length, message_length;
    char* channel, *message;
    int err = 0;
    if (!err)
        err |= ParseInt(&personal_receive_fifo, &token);
    if (!err)
        err |= ParseInt(&personal_receive_fifo, &channel_length);
    if (!err)
        channel = malloc((channel_length + 1) * sizeof(char)),
        err |= ParseString(&personal_receive_fifo, channel, channel_length);
    if (!err)
        err |= ParseInt(&personal_receive_fifo, &message_length);
    if (!err)
        message = malloc((message_length + 1) * sizeof(char)),
        err |= ParseString(&personal_receive_fifo, message, message_length);
    if (err) {
        printf("Error while getting data from daemon: %d\n", err);
        return err;
    }
    err = pthread_mutex_lock(&callbacks_is_busy);
    if (err) {
        printf("Unable to lock mutex: ");
        perror(NULL);
        return errno;
    }
    void* data;
    err = TreapFind(callbacks, token, &data);
    pthread_mutex_unlock(&callbacks_is_busy);
    
    if (err) {
        printf("Couldn't find in treap the given token");
        return err;
    }

    printf("Received token %d from cannel %s, with message %s\n",
        token, channel, message);
    fflush(stdout);

    void(*callback)(const char* channel, const char* message) = data;
    (*callback)(channel, message);

    return 0;
}

int HandleHistory()
{
    int err = 0;
    err |= ParseInt(&personal_receive_fifo, &nr_received);
    if (!err)
        pt_received_channels = malloc(nr_received * sizeof(char*)),
        pt_received_messages = malloc(nr_received * sizeof(char*));

    for (int i = 0; i < nr_received; ++i) {
        int lg_ch = 0, lg_message = 0;
        if (!err)
            err |= ParseInt(&personal_receive_fifo, &lg_ch);
        if (!err)
            pt_received_channels[i] = malloc((lg_ch + 1) * sizeof(char)),
            err |= ParseString(&personal_receive_fifo, pt_received_channels[i], lg_ch);
        if (!err)
            err |= ParseInt(&personal_receive_fifo, &lg_message);
        if (!err)
            pt_received_messages[i] = malloc((lg_message + 1) * sizeof(char)),
            err |= ParseString(&personal_receive_fifo, pt_received_messages[i], lg_message);
    }

    printf("Releasing mutex\n");
    err |= pthread_mutex_unlock(&receiving_history);

    return err;
}

// Should see how to not make it exit too fast, and break stuff.
void* ReceiveChecker(void* _)
{
    // not used operation
    (void)(_);
    // Just try to read stuff from personal_fifo.
    while (1) {
        int err, op_code = -1;
        err = ParseInt(&personal_receive_fifo, &op_code);
        if (op_code == 1) {
            // am primit callback
            if (!err)
                err |= HandleCallback();
        }
        else if (op_code == 2) {
            // am primit history
            if (!err)
                err |= HandleHistory();
        }
        else {
            printf("Unknown operation code from Daemon: %d", op_code);
        }
    }
}

int GetSyncHistory(const char* path_channel, int max_entries, 
                   int* found_entries, char*** channels, char*** messages)
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }
    
    const char* random_fifo_name = GenerateRandomFifoName();

    FifoParser parser;
    int err = FifoInit(&parser, random_fifo_name, 1);
    if (err)
        return err;

    printf("Locking mutex\n");

    // Send request to daemon, then wait for the mutex to signal we have the data
    err |= pthread_mutex_lock(&receiving_history);

    err |= PrintInt(&parser, 4);
    err |= PrintInt(&parser, personal_fifo_id);
    err |= PrintInt(&parser, max_entries);
    err |= PrintInt(&parser, strlen(path_channel));
    err |= PrintString(&parser, path_channel, strlen(path_channel));

    if (!err)
        err |= PrintString(&daemon_fifo, random_fifo_name, strlen(random_fifo_name));

    printf("Locking mutex again\n");

    // it will get unlocked only after HandleHistory finished
    if (!err)
        err |= pthread_mutex_lock(&receiving_history);

    printf("Mutex unlocked again\n");

    *found_entries = nr_received;
    *channels = pt_received_channels;
    *messages = pt_received_messages;

    if (!err)
        err |= pthread_mutex_unlock(&receiving_history);
        
    return err;
}

int BroadcastTelemetry(const char* channel, const char* message)
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    const char* random_fifo_name = GenerateRandomFifoName();

    FifoParser parser;
    int err = FifoInit(&parser, random_fifo_name, 1);
    if (err)
        return err;

    err |= PrintInt(&parser, 1);
    err |= PrintInt(&parser, strlen(channel));
    err |= PrintString(&parser, channel, strlen(channel));
    err |= PrintInt(&parser, strlen(message));
    err |= PrintString(&parser, message, strlen(message));

    if (!err)
        err |= PrintString(&daemon_fifo, random_fifo_name, strlen(random_fifo_name));

    return err;
}

int RegisterCallback(const char* channel, 
                     void(*callback)(const char* channel, const char* message))
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    const char* random_fifo_name = GenerateRandomFifoName();

    FifoParser parser;
    int err = FifoInit(&parser, random_fifo_name, 1);
    if (err)
        return err;
    
    static int counter = 0;
    counter++;

    err |= PrintInt(&parser, 2);
    err |= PrintInt(&parser, counter);
    err |= PrintInt(&parser, personal_fifo_id);
    err |= PrintInt(&parser, strlen(channel));
    err |= PrintString(&parser, channel, strlen(channel));

    if (!err)
        err |= PrintString(&daemon_fifo, random_fifo_name, strlen(random_fifo_name));

    // Save callback in DS
    if (!err) {
        err |= pthread_mutex_lock(&callbacks_is_busy);
        err |= TreapInsert(&callbacks, counter, callback);
        err |= pthread_mutex_unlock(&callbacks_is_busy);
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
    err |= pthread_mutex_unlock(&callbacks_is_busy);
    if (err)
        return err;

    const char* random_fifo_name = GenerateRandomFifoName();

    FifoParser parser;
    err |= FifoInit(&parser, random_fifo_name, 1);
    if (err)
        return err;

    err |= PrintInt(&parser, 3);
    err |= PrintInt(&parser, callback_id);
    err |= PrintInt(&parser, personal_fifo_id);

    if (!err)
        err |= PrintString(&daemon_fifo, random_fifo_name, strlen(random_fifo_name));

    return err;   
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
    // AppendInt(PERSONAL_QUERY_CHANNEL + strlen(PERSONAL_QUERY_CHANNEL), personal_fifo_id);
    // err = FifoInit(&personal_query_fifo, PERSONAL_QUERY_CHANNEL, 1);

    // Create and open history FIFO.
    AppendInt(PERSONAL_RECEIVE_CHANNEL + strlen(PERSONAL_RECEIVE_CHANNEL), personal_fifo_id);
    err |= FifoInit(&personal_receive_fifo, PERSONAL_RECEIVE_CHANNEL, 1);

    if (err)
        return err;

    // Launching new thread.
    err |= pthread_create(&callback_thread, NULL, ReceiveChecker, NULL);

    if (err)
        return err;

    initialized = 1;

    printf("Finished initialization!\n");
    fflush(stdout);
    return 0;
}

int CloseTelemetry()
{
    if (initialized) {
        int err = FifoClose(&daemon_fifo);
        // err |= FifoClose(&personal_query_fifo);
        err |= FifoClose(&personal_receive_fifo);
        
        pthread_cancel(callback_thread);
        err |= ClearTreap(&callbacks);
        // unlink(PERSONAL_QUERY_CHANNEL);
        unlink(PERSONAL_RECEIVE_CHANNEL);
        initialized = 0;
        return 0;
    }
    return 1;
}
