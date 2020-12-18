#ifndef BROADCAST_STORAGE_
#define BROADCAST_STORAGE_

#include <stdlib.h>
#include <stdio.h>

#include "standard.h"

/**
 * This storage allows the daemon to store all the sent messages
 * to answer to audit style queries. This DS can:
 *      1. Add a message in the DS.
 *      3. Request last X messages from a channel.
 */

typedef struct {
    const char * channel;
    const char * message;
} HistoryEntry;

// Adds an entry to the history of broadcasts
int HistoryStorageAdd(const char* channel, const char* message);

// Returns all the messages sent in one channel
int HistoryStorageQuery(const int max_entries, const char* channel, 
                const char*** history_channels, const char*** history_messages, int* nr_entries);

int HistoryDeleteTooOldMessages();

#endif // BROADCAST_STORAGE_
