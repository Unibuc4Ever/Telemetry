#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "telemetry.h"
#include "fifo_parser.h"

void sample_f(const char* channel, const char* message)
{
    printf("\n\nHello!\nChannel: %s\nMessage: %s\n", channel, message);
}

int main()
{
    int x = InitializeTelemetry();
    if (x) {
        printf("Error: %d\n", x);
        exit(x);
    }

    RegisterCallback("/my/personal/path/", sample_f);

    return 0;

    while (1) {
        char a[100], b[100];
        memset(a, 0, sizeof a);
        memset(b, 0, sizeof b);
        scanf("%s%s", a, b);
        printf("Status: %d\n", BroadcastTelemetry(a, b));
    }
}