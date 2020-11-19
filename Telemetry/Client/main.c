#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "telemetry.h"
#include "fifo_parser.h"

int main()
{
    int x = InitializeTelemetry();
    if (x) {
        printf("Error: %d\n", x);
        exit(x);
    }

    while (1) {
        char a[100], b[100];
        memset(a, 0, sizeof a);
        memset(b, 0, sizeof b);
        scanf("%s%s", a, b);
        printf("Status: %d\n", BroadcastTelemetry(a, b));

    }
}