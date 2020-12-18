#ifndef CALLBACK_STORAGE_
#define CALLBACK_STORAGE_

/**
 * This storage allows the daemon to store all the registered callbacks
 * made by users. This storage can be seen as a DS with the following
 * posibilities:
 *      1. Bind a callback (pair of type PID/callbackID) to a path
 *      2. Add a message in the DS. This returns all the affected callbacks
 *      3. Request top X messages sent at a certain path
 */

// Used to storing callbacks.
typedef struct {
    int PID, token;
} Callback;

// Adds a callback.
int StorageAdd(Callback callback, const char* channel);

// Removes a callback.
int StorageRemove(Callback callback);

// Returns all the callbacks at a certain path.
int StorageGetCallbacks(const char* channel, Callback** callbacks, int *number_of_callbacks);



#endif // CALLBACK_STORAGE_
