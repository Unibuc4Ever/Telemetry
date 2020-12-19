#ifndef SENDERS_
#define SENDERS_

#include <string.h>
#include <stdio.h>

#include "fifo_parser.h"
#include "standard.h"

void SendHistory(int PID, int entries_found, const char** channels, const char** messages);

void SendCallback(int PID, int token, char* channel, char* message);

#endif // SENDERS_