#ifndef TELEMETRY__
#define TELEMETRY__

// Broadcast function, sends a message to all registered programms.
// Return value: error code.
int Broadcast(const char* message, const char* channel);

// Register function, registers a callback funtion to a certain channel.
// Return value: Callback ID / error code.
int RegisterCallback(
        const char* channel,
        void (*callback)(
                const char* channel,
                const char* message
        )
);

// Unregister function, disables a callback function.
int UnRegisterCallback(int callback_id);

// Initialize library.
void Init();

// Clear used memory.
void Close();


#endif // TELEMETRY__