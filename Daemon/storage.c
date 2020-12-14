#include <string.h>
#include <stdlib.h>

#include "storage.h"
#include "standard.h"

struct CallbackNode {
    char* channel;
    Callback callback;
    struct CallbackNode* next;
};

typedef struct CallbackNode* CallbackList;

CallbackList root;

int StorageAdd(Callback callback, const char* channel)
{
    CallbackList node = malloc(sizeof(node));
    node->callback = callback;
    node->channel = CopyString(channel);
    node->next = root;
    root = node;
    return 0;
}

int StorageRemove(Callback callback)
{
    if (root == NULL)
        return -1; // NOT FOUND
    
    if (root->callback.PID == callback.PID &&
            root->callback.token == callback.token) {
        CallbackList new_root = root->next;
        free(root->channel);
        free(root);
        root = new_root;
        return 0;
    }

    for (CallbackList elem = root; elem; elem = elem->next) {
        if (elem->next == NULL)
            return -1; // NOT FOUND
        if (elem->next->callback.PID == callback.PID &&
                elem->next->callback.token == callback.token) {
            CallbackList urm = elem->next->next;
            free(elem->next->channel);
            free(elem->next);
            elem->next = urm;
            return 0;
        }
    }
    return -1;
}

int StorageGetCallbacks(const char* channel, Callback** callbacks, int *number_of_callbacks)
{
    *number_of_callbacks = 0;
    for (CallbackList elem = root; elem; elem = elem->next)
        if (IsPrefixOf(elem->channel, channel))
            (*number_of_callbacks)++;

    // Nothing to return.
    if (*number_of_callbacks == 0) {
        callbacks = NULL;
        return 0;
    }

    *callbacks = malloc(*number_of_callbacks * sizeof(callbacks));

    // Unable to allocate memory.
    if (callbacks == NULL)
        return -1;

    int cnt = 0;
    for (CallbackList elem = root; elem; elem = elem->next)
        if (IsPrefixOf(elem->channel, channel))
            (*callbacks)[cnt++] = elem->callback;

    return 0;
}

