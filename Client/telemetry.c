#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

#include "fifo_parser.h"

// Wheter InitializeTelemetry was called or not.
int initialized;

FifoParser daemon_fifo, personal_fifo;

int GenerateRandomName()
{
    static int cnt = 1;
    return getpid() + cnt++;
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
    AppendInt(PERSONAL_FIFO_CHANNEL + strlen(PERSONAL_FIFO_CHANNEL), getpid());
    err = FifoInit(&personal_fifo, PERSONAL_FIFO_CHANNEL, 1);

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

    err = PrintInt(&parser, 1);
    err |= PrintInt(&parser, strlen(channel));
    err |= PrintString(&parser, channel, strlen(channel));
    err |= PrintInt(&parser, strlen(message));
    err |= PrintString(&parser, message, strlen(message));

    err |= FifoClose(&parser);

    if (!err)
        err |= PrintString(&daemon_fifo, broadcast_fifo, strlen(broadcast_fifo));
    return err;
}

int RegisterCallback(const char* channel, void(*callback)(const char* channel, const char* message))
{
    int x = channel || callback;
    x++;
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }
    return 0;
}

int CloseTelemetry()
{
    if (initialized) {
        FifoClose(&daemon_fifo);
        FifoClose(&personal_fifo);
        initialized = 0;
        return 0;
    }
    return 1;
}
