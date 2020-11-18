#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

// Channel used for sending messages to the daemon.
char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";

// Wheter InitializeTelemetry was called or not.
int initialized;

// File descriptor of the daemon's communication pipe.
int daemon_fd;

int InitializeTelemetry()
{
    if (initialized)
        return 0;

    daemon_fd = open(DAEMON_FIFO_CHANNEL, O_WRONLY);
    if (daemon_fd == -1)
        return -1; // TODO: see what happends

    initialized = 1;
    return 0;
}

int BroadcastTelemetry(const char* channel, const char* message)
{
    if (!initialized) {
        int x = InitializeTelemetry();
        if (x)
            return x;
    }

    // Now I'm just throwing all the content to the pipe
    char space[] = " ";
    write(daemon_fd, channel, strlen(channel));
    write(daemon_fd, space, strlen(space));
    write(daemon_fd, message, strlen(message));
    write(daemon_fd, space, strlen(space));

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
