#ifndef TELEMETRY_
#define TELEMETRY_

#include "standard.h"
#include "fifo_parser.h"
#include "treap.h"

/**
 * Library connecting an app with the functionalities of the
 * Telemetry Daemon. The Daemon needs to be running in order
 * for the library to work corectly.
 */


// Start the the communication with the daemon.
int InitializeTelemetry();

// Broadcast a message to a given channel.
int BroadcastTelemetry(const char* channel, const char* message);

// Register a callback function.
// -1 if error while registering.
// ID of the callback if not.
int RegisterCallback(const char* channel, void(*callback)(const char* channel, const char* message));

// Remove a registered callback from the list.
int RemoveRegisteredCallback(int callback_id);

// Returns history for a given path.
int GetSyncHistory(const char* channel, int max_entries, 
                   int* found_entries, char*** channels, char*** messages);

// Finish communication with the daemon.
int CloseTelemetry();


#endif // TELEMETRY_