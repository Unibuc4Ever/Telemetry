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
    char PERSONAL_FIFO_CHANNEL[100] = "/tmp/TelemetryClient/";

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

    // char broadcast_fifo[100] = "/tmp/TelemetryBroadcast/";
    // ParseInt(GenerateRandomName(), broadcast_fifo + strlen(broadcast_fifo));

    // int stat = mkfifo(DAEMON_FIFO_CHANNEL, S_IRUSR | S_IWUSR);
    // if (stat == -1) {
    //     printf("Unable to open pipe!");
    //     return -1;
    // }

    // int broadcast_fd = open(DAEMON_FIFO_CHANNEL, O_WRONLY);
    // if (broadcast_fd == -1)
    //     return -1;

    // char number[100];
    // char enter[] = "\n";

    // memset(number, 0, sizeof number);
    // ParseInt(1, number);
    // number[strlen(number)] = '\n';
    // write(broadcast_fd, number, strlen(number));

    // memset(number, 0, sizeof number);
    // ParseInt(strlen(channel), number);
    // number[strlen(number)] = '\n';
    // write(broadcast_fd, number, strlen(number));

    // write(broadcast_fd, channel, strlen(channel));
    // write(broadcast_fd, enter, 1);

    // memset(number, 0, sizeof number);
    // ParseInt(strlen(message), number);
    // number[strlen(number)] = '\n';
    // write(broadcast_fd, number, strlen(number));

    // write(broadcast_fd, message, strlen(message));
    // write(broadcast_fd, enter, 1);

    // close(broadcast_fd);
    
    // broadcast_fifo[strlen(broadcast_fifo)] = '\n';
    // write(daemon_fd, broadcast_fifo, sizeof broadcast_fifo);

    return 0;
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
        close(daemon_fd);
        daemon_fd = initialized = 0;
        return 0;
    }
    return 0;
}
