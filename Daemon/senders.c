#include "senders.h"

void SendHistory(int PID, int entries_found, const char** channels, const char** messages)
{
    char personal_fifo_channel[MAX_LENGTH_FIFO_NAME];
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
    char personal_fifo_channel[MAX_LENGTH_FIFO_NAME];
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
