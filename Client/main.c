#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "telemetry.h"
#include "fifo_parser.h"

#define flush fflush(stdout)

void sample_f(const char* channel, const char* message)
{
    printf("\n\n\nHello! I'm a callback function!!\n"
           "I was called from:\n"
           "    Channel: %s\n"
           "    Message: %s\n", channel, message);
    printf("\n\n $ ");
    fflush(stdout);
}

int main()
{
    InitializeTelemetry();

    printf("Hello there!\n\
            Actions are:\n\
            1. Broadcast message\n\
            2. Register Callback\n\
            3. Delete Callback\n\
            4. Request History\n\
            5. Close");
    
    while (1) {
        printf(" $ ");
        flush;
        int x;
        scanf("%d", &x);
        if (x == 1) {
            char channel[100], message[100];
            printf("What channel?\n $ ");
            flush;
            scanf("%s", channel);
            printf("What message?\n $ ");
            flush;
            scanf("%s", message);
            int err = BroadcastTelemetry(channel, message);
            printf("Action finished with status %d!\n", err);
        }
        else if (x == 2) {
            char channel[100];
            printf("What channel?\n $ ");
            flush;
            scanf("%s", channel);
            int err = RegisterCallback(channel, sample_f);
            printf("Callback registered with token %d\n", err);
        }
        else if (x == 3) {
            printf("What token do you want to revoke?\n $ ");
            int x;
            scanf("%d", &x);
            int err = RemoveRegisteredCallback(x);
            printf("Action finished with status %d\n", err);
        }
        else if (x == 4) {
            printf("What channel to query for history?\n $ ");
            char channel[100];
            int entries;
            scanf("%s", channel);
            printf("How many messages?\n $ ");
            scanf("%d", &entries);

            int nr_found = 0;
            char** f_channels, **f_messages;
            int err = GetSyncHistory(channel, entries, 
                                     &nr_found, &f_channels, &f_messages);
            printf("\n Found %d messages:\n", nr_found);
            for (int i = 0; i < nr_found; ++i) {
                printf("channel: %s\nmessage: %s", f_channels[i], f_messages[i]);
            }
            printf("Action finished with status %d\n", err);
        }
        else if (x == 5) {
            CloseTelemetry();
            break;
        }
        else
            printf("Action not recognized!\n");
    }

    return 0;
}