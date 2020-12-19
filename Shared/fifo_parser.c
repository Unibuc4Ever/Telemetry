#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>

#include "fifo_parser.h"

char GetNextChar(FifoParser* parser)
{
    if (!parser->buffer[parser->p]) {
        memset(parser->buffer, 0, sizeof parser->buffer);
        parser->p = 0;
        read(parser->fd, parser->buffer, sizeof(parser->buffer) - 1);
    }
    return parser->buffer[parser->p++];
}

int FifoInit(FifoParser* parser, const char* path, int reset)
{
    // Delete the file if requested.
    if (reset)
        unlink(path);

    // Try to create the file pipe.
    int stat = mkfifo(path, S_IRUSR | S_IWUSR);

    if (stat == -1) {
#ifdef DEBUG
        printf("Unable to create pipe %s: Code: %d\n", path, errno);
        char message[] = "Error message: ";
        perror(message);
#endif
    }
    else {
 #ifdef DEBUG
        printf("Successfully created the pipe!\n");
        fflush(stdout);
 #endif
    }

    // Open file descriptor.
    parser->fd = open(path, O_RDWR);
    if (parser->fd == -1) {
#ifdef DEBUG
        printf("Unable to open file descriptor!\n");
        char message[] = "Error message: ";
        perror(message);
#endif
        return -1;
    }
    else {
#ifdef DEBUG
        printf("Successfully opened file descriptor!\n");
#endif
    }

    memset(parser->buffer, 0, sizeof(parser->buffer));
    parser->p = 0;
    return 0;
}

int FifoClose(FifoParser* parser)
{
    if (parser->fd) {
        close(parser->fd);
        parser->fd = 0;
        return 0;
    }
    return 1;
}

int ParseInt(FifoParser* parser, int* x)
{
    int ans = 0;
    char c;
    while (1) {
        c = GetNextChar(parser);
        if (c < '0' || c > '9')
            break;
        ans = 10 * ans + c - '0';
    }
    *x = ans;
    return 0;
}

int ParseString(FifoParser* parser, char* s, int length)
{
    for (int i = 0; i < length; i++) {
        *s = GetNextChar(parser);
        s++;
    }
    *s = 0;
    int c = GetNextChar(parser);
    return !(c == ' ' || c == '\n');
}

int ParseWord(FifoParser* parser, char* s, int length_max)
{
    for (int i = 0; i < length_max; i++) {
        char c = GetNextChar(parser);
        if (c == ' ' || c == '\n') {
            s[i] = 0;
            return i;
        }
        s[i] = c;
    }
    return length_max;
}

int PrintInt(FifoParser* parser, int x)
{
    char number[11];
    int p = 11;
    if (x == 0) {
        number[10] = '0';
        p--;
    }
    while (x) {
        p--;
        number[p] = '0' + x % 10;
        x /= 10;
    }
    return PrintString(parser, number + p, 11 - p);
}

int PrintString(FifoParser* parser, const char* s, int length)
{
    int wr;
    while (length) {
        wr = write(parser->fd, s, length);
        if (wr < 0)
            return wr;
        s += wr;
        length -= wr;
    }
    char enter[] = "\n";
    wr = write(parser->fd, enter, 1);
    return (wr == 1 ? 0 : wr);
}

