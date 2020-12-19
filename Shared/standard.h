#ifndef STANDARD_
#define STANDARD_

/**
 * Defines a bunch of "standard" functions and constants.
 */

#define MAX_LENGTH_FIFO_NAME  256


extern const char DAEMON_FIFO_CHANNEL[];

extern const char PERSONAL_QUERY_CHANNEL[];
extern const char PERSONAL_RECEIVE_CHANNEL[];


char* GenerateRandomFifoName();

// Create a new copy of the string.
char* CopyString(const char* string);

// Checks if small is a prefix of big.
int IsPrefixOf(const char* big, const char* small);

// Ads string representation of int in string s
void AppendInt(char* s, int nr);

// Checks if the path is like ^(/\w+)+/$
// example: /First_needed/perhaps/numbers777/
int isValidPath(const char* path);

#endif // STANDARD_