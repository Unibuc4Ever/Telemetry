#include <stdlib.h>
#include <string.h>

#include "universal_type.h"


UniversalType* CreateUniversalType(void* data, int size)
{
    UniversalType* type = (UniversalType*)malloc(sizeof type);
    type->size = size;
    type->data = malloc(size);
    memcpy(type->data, data, size);
}

void DelteUniversalType(UniversalType* type)
{
    free(type->data);
    type->data = NULL;
    type->size = 0;
}