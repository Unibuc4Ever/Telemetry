#include "history_storage.h"


#define MAXIMUM_MESSAGES  5

typedef struct __HistoryNode__
{
    HistoryEntry data;
    struct __HistoryNode__* prev;
    struct __HistoryNode__* next;
} HistoryNode;

HistoryNode* start = 0;
HistoryNode* final = 0;

static int total_count_messages = 0;

void free_history_node(HistoryNode* h_node)
{
    if (h_node) {
        free((char*)h_node->data.channel);
        free((char*)h_node->data.message);
    }
    free(h_node);
}

// Add a generated HistoryNode ot the end of the list
int HistoryStorageAdd(const char* channel, const char* message)
{
    total_count_messages += 1;

    HistoryNode* node = malloc(sizeof(HistoryNode));
    node->data = (HistoryEntry) {CopyString(channel), CopyString(message)};
    node->next = 0;
    node->prev = final;

    if (!start || !final) { 
        // daca nu am nimic deocamndata
        start = final = node;
        return 0;        
    }

    final->next = node;
    final = node;

    // if length is too big then delete the oldest message
    if (total_count_messages > MAXIMUM_MESSAGES) {
        node = start;
        start = start->next;
        start->prev = 0;
        free_history_node(node);

        total_count_messages -= 1;
    }

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

    *history_channels = malloc(*nr_entries * sizeof(char*));
    *history_messages = malloc(*nr_entries * sizeof(char*));

    if (!history_channels || !history_messages) {
        free(history_channels);
        free(history_messages);
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