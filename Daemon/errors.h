#ifndef ERRORS_
#define ERRORS_

/**
 * File storing documentation and definition
 * for error types included in the project.
 */

// Internal error, which should not be caused by the user.
#define INTERNAL_ERROR 1

// General error, unknown reason.
#define GENERAL_ERROR -1

// Received channel is empty
#define EMPTY_CHANNEL_ERROR 101

// Received channel isn't valid
#define INVALID_CHANNEL_NAME 100

// Invalid operation made on a Vector
#define INVALID_VECTOR_OPERATION 200

#endif // ERRORS_