#include <stdlib.h>
#include <string.h>

#include "standard.h"

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

