#ifndef TELEMETRY_
#define TELEMETRY_

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
int RegisterCallback(const char* channel, void(*callback)(const char* channel, const char* message));


// Finish communication with the daemon.
int CloseTelemetry();


#endif // TELEMETRY_