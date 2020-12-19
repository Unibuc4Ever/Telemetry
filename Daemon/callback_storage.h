#ifndef CALLBACK_STORAGE_
#define CALLBACK_STORAGE_

#include "standard.h"

/**
 * This storage allows the daemon to store all the registered callbacks
 * made by users. This storage can be seen as a DS with the following
 * posibilities:
 *      1. Bind a callback (pair of type PID/callbackID) to a path
 *      2. Add a message in the DS. This returns all the affected callbacks
 *      3. Request top X messages sent at a certain path
 */

// Used to storing callbacks.
typedef struct __Callback__ {
    int PID, token;
    char* channel;
} Callback;

typedef struct __dummy_cb__ {
    int PID, token;
} dummy_cb;

// Adds a callback.
int StorageAdd(Callback callback);

// Removes a callback.
int StorageRemove(Callback callback);

// Returns all the callbacks at a certain path.
int StorageGetCallbacks(const char* channel, Callback** callbacks, int *number_of_callbacks);

// Deletes callbacks for non existent PIDs
int CallbackDeleteForNonexistentPID();

#endif // CALLBACK_STORAGE_
