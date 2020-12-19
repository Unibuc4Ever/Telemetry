#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

#include "callback_storage.h"

typedef struct __CallbackNode__ {
    Callback callback;
    struct __CallbackNode__* next;
} CallbackNode;

CallbackNode* root = 0;

static int isAlivePID(int pid)
{
    return kill(pid, 0) == 0;
}

int StorageAdd(Callback callback)
{
    CallbackNode* node = malloc(sizeof(*node));
    node->callback = callback;
    node->callback.channel = CopyString(callback.channel);
    node->next = root;
    root = node;
    return 0;
}

int StorageRemove(Callback callback)
{
    /// Here the callback.channel pointer is NULL most probably
    if (root == NULL)
        return -1; // NOT FOUND
    
    if (root->callback.PID == callback.PID &&
            root->callback.token == callback.token) {
        CallbackNode* new_root = root->next;
        free(root->callback.channel);
        free(root);
        root = new_root;
        return 0;
    }

    for (CallbackNode* elem = root; elem; elem = elem->next) {
        if (elem->next == NULL)
            return -1; // NOT FOUND
        if (elem->next->callback.PID == callback.PID &&
                elem->next->callback.token == callback.token) {
            CallbackNode* urm = elem->next->next;
            free(elem->next->callback.channel);
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
    for (CallbackNode* elem = root; elem; elem = elem->next)
        if (IsPrefixOf(elem->callback.channel, channel))
            (*number_of_callbacks)++;

    // Nothing to return.
    if (*number_of_callbacks == 0) {
        callbacks = NULL;
        return 0;
    }

    *callbacks = malloc(*number_of_callbacks * sizeof(*callbacks));

    // Unable to allocate memory.
    if (callbacks == NULL)
        return -1;

    dummy_cb* dum_pt = malloc(1 * sizeof(*dum_pt));

    int cnt = 0;
    for (CallbackNode* elem = root; elem; elem = elem->next)
        if (IsPrefixOf(elem->callback.channel, channel))
            (*callbacks)[cnt++] = elem->callback;

    free(dum_pt);

    return 0;
}

int CallbackDeleteForNonexistentPID()
{
    // delete non root elements first
    if (root) {
        CallbackNode* curr = root->next, *previous = root;
        while (curr) {
            if (!isAlivePID(curr->callback.PID)) {
                printf(" --- Deleted (dead) callback PID/token:   %d | %d", 
                       curr->callback.PID, curr->callback.token);
                previous->next = curr->next;
                free(curr->callback.channel);
                free(curr);
                curr = previous->next;
            }
            else {
                previous = curr;
                curr = curr->next;
            }
        }
    }
    // try to delete the root if necessary
    if (root && !isAlivePID(root->callback.PID)) {
        printf(" --- Deleted (dead) callback PID/token:   %d | %d", 
                root->callback.PID, root->callback.token);
        CallbackNode* next = root->next;
        free(root->callback.channel);
        free(root);
        root = next;
    }
    return 0;
}