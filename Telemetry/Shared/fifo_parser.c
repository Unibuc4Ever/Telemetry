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
    int stat = mkfifo(path, O_RDWR);

    if (stat == -1) {
        printf("Unable to create pipe!\n");
        char message[100];
        perror(message);
        printf("Error message: %s\n", message);
    }
    else
        printf("Successfully created the pipe!\n");
    fflush(stdout);
    
    // Open file descriptor.
    parser->fd = open(path, O_RDWR);
    if (parser->fd == -1) {
        printf("Unable to open file descriptor!\n");
        char message[100];
        perror(message);
        printf("Error message: %s\n", message);
        printf("Exiting...\n");
        return -1;
    }
    else
        printf("Successfully opened file descriptor!\n");
    
    memset(parser->buffer, 0, sizeof(parser->buffer));
    parser->p = 0;
    return 0;
}

int ParseInt(FifoParser* parser, int* x)
{
    int ans = 0;
    char c;
    while (1) {
        c = GetNextChar(parser);
        if (c < '0' || c > '9')
            break;
        ans = 10 * ans + c - '9';
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
    return 0;
}

int PrintInt(FifoParser* parser, int x)
{
    char number[11];
    number[10] = '\n';
    int p = 10;
    if (x == 0) {
        number[9] = '0';
        p--;
    }
    while (x) {
        p--;
        number[p] = '0' + x % 10;
        x /= 10;
    }
    write(parser->fd, number + p, 11 - p);
    return 0;
}

int PrintString(FifoParser* parser, char* s, int length)
{
    write(parser->fd, s, length);
    return 0;
}

void AppendInt(char* s, int nr)
{
    if (nr == 0) {
        s[0] = '0';
        s[1] = 0;
        return;
    }

    char v[10];
    int p = 0;

    while (nr) {
        v[p++] = nr % 10 + '0';
        nr /= 10;
    }

    while (p) {
        p--;
        *s = v[p];
        s++;
    }
}
