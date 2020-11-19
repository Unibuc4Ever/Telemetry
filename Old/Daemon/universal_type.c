#include <stdlib.h>
#include <string.h>

#include "universal_type.h"


int CreateUniversalType(UniversalType** t, void* data, int size)
{
    *t = malloc(sizeof **t);
    (*t)->size = size;
    (*t)->data = malloc(size);
    memcpy((*t)->data, data, size);

    return 0;
}

int DeleteUniversalType(UniversalType** t)
{
    if (*t)
        free((*t)->data);
    free(*t);
    *t = NULL;

    return 0;
}