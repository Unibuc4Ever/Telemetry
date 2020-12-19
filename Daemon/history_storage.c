#include "history_storage.h"

typedef struct __HistoryNode__
{
    HistoryEntry data;
    struct __HistoryNode__* prev;
    struct __HistoryNode__* next;
} HistoryNode;

HistoryNode* start = 0;
HistoryNode* final = 0;

// Add a generated HistoryNode ot the end of the list
int HistoryStorageAdd(const char* channel, const char* message)
{
    HistoryNode* node = malloc(sizeof(HistoryNode));
    node->data = (HistoryEntry) {CopyString(channel), CopyString(message), time(NULL)};
    node->next = 0;
    node->prev = final;

    if (!start || !final) { 
        // daca nu am nimic deocamndata
        start = final = node;
        return 0;        
    }

    final->next = node;
    final = node;

    return 0;
}

int HistoryStorageQuery(const int max_entries, const char* channel, 
                const char*** history_channels, const char*** history_messages, int* nr_entries)
{
    *nr_entries = 0;
    for(HistoryNode* current = final; 
        current && *nr_entries < max_entries; 
        current = current->prev) if (IsPrefixOf(channel, current->data.channel)) {
            
            (*nr_entries) += 1;
    }   

    printf("First run of query found %d entries\n", *nr_entries);

    if (*nr_entries == 0)
        return 0;

    *history_channels = malloc(*nr_entries * sizeof(char*));
    *history_messages = malloc(*nr_entries * sizeof(char*));

    if (!history_channels || !history_messages) {
        free(history_channels);
        free(history_messages);
        history_channels = history_messages = NULL;
        *nr_entries = 0;
        return -1;
    }
    
    *nr_entries = 0;
    for(HistoryNode* current = final; 
        current && *nr_entries < max_entries; 
        current = current->prev) if (IsPrefixOf(channel, current->data.channel)) {
            
            (*history_channels)[*nr_entries] = current->data.channel;
            (*history_messages)[*nr_entries] = current->data.message;
            (*nr_entries) += 1;
    }

    return 0;
}

int HistoryDeleteTooOldMessages()
{
    int last_valid_timestamp = time(NULL) - HISTORY_TIMESPAN;
    
    int nr_deleted = 0;
    while(start && start->data.timestamp < last_valid_timestamp) {
        HistoryNode* next = start->next;
        free(start);
        start = next;
        start->prev = NULL;
        ++nr_deleted;
    }
    return nr_deleted;
}