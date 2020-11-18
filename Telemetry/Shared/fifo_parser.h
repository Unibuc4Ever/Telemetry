#ifndef FIFO_PARSER_
#define FIFO_PARSER_

/**
 * Lib able to parse / write stuff to FIFO pipes.
 * Integers always add a new line after them.
 * Strings never do that.
 */

typedef struct
{
    int fd;
    char buffer[100];
    int p;
} FifoParser;

// Initialize a parser.
int FifoInit(FifoParser* parser, const char* path, int reset);

// Parse an integer.
int ParseInt(FifoParser* parser, int* x);

// Parse a string.
int ParseString(FifoParser* parser, char* s, int length);

// Write an integer.
int PrintInt(FifoParser* parser, int x);

// Write a string.
int PrintString(FifoParser* parser, char* s, int length);

// Appends an integer to a string.
void AppendInt(char* s, int nr);

#endif // FIFO_PARSER
