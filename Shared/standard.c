#include <stdlib.h>
#include <string.h>

#include "standard.h"

const char DAEMON_FIFO_CHANNEL[] = "/tmp/TelemetryRequests";

const char PERSONAL_QUERY_CHANNEL[] = "/tmp/TelemetryQueryNr";
const char PERSONAL_RECEIVE_CHANNEL[] = "/tmp/TelemetryReceiveNr";

static int GenerateRandomInt()
{
    static int cnt = 0;
    return cnt++ % 1000 + 1;
}

char* GenerateRandomFifoName()
{
    static char random_fifo_name[MAX_LENGTH_FIFO_NAME];
    strcpy(random_fifo_name, PERSONAL_QUERY_CHANNEL);
    AppendInt(random_fifo_name + strlen(random_fifo_name), GenerateRandomInt());
    return random_fifo_name;
}

char* CopyString(const char* string)
{
    int len = strlen(string);
    char* ans = malloc(len + 1);
    strcpy(ans, string);
    return ans;
}

int IsPrefixOf(const char* big, const char* small)
{
    int l_big = strlen(big);
    int l_small = strlen(small);

    if (l_big < l_small)
        return 0;

    for (int i = 0; i < l_small; i++)
        if (big[i] != small[i])
            return 0;
    
    return 1;
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
    // zero ending, very important
    *s = 0;
}
