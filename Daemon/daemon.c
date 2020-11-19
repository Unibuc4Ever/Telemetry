/**
 * This is not yet a full daemon, as it's still missing some important features.
 * It's used for development purpuses.
 */
#include <stdio.h>

#include "sniffer.h"

int main()
{
    int err = StartRuntime();

    if (err)
        printf("Failed to start daemon: error %d\n", err);

    return 0; 
} 
