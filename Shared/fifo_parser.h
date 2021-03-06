#ifndef FIFO_PARSER_
#define FIFO_PARSER_

/**
 * Lib able to parse / write stuff to FIFO pipes.
 * Parsed elements always add a new line after them.
 */

typedef struct
{
    int fd;
    char buffer[100];
    int p;
} FifoParser;

// Initialize a parser.
int FifoInit(FifoParser* parser, const char* path, int reset);

// Close a parser.
int FifoClose(FifoParser* parser);

// Parse an integer.
int ParseInt(FifoParser* parser, int* x);

// Parse a string.
int ParseString(FifoParser* parser, char* s, int length);

// Parse a word (until a space or an enter).
// Returns parsed length.
int ParseWord(FifoParser* parser, char* s, int length_max);

// Write an integer.
int PrintInt(FifoParser* parser, int x);

// Write a string.
int PrintString(FifoParser* parser, const char* s, int length);

// Appends an integer to a string.
void AppendInt(char* s, int nr);

#endif // FIFO_PARSER
