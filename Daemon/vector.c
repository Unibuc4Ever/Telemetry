#include <stdlib.h>

#include "vector.h"

struct Vector CreateEmptyVector(int byte_size)
{
    struct Vector v;
    v.v = 0;
    v.size = v.allocated = 0;
    v.byte_size = byte_size;
    return v;
}


struct Vector CreateVector(int byte_size, int size)
{
    struct Vector v;
    v.size = v.allocated = size;
    v.byte_size = byte_size;
    v.v = malloc(size * byte_size);
    return v;
}


void PushBack(struct Vector* v, void* elem)
{
    // Not enough memory, let's double length.
    if (v->allocated == v->size) {
        // New size.
        int new_size = 2 * v->size + 1;
        // Allocate new memory.
        void* new_location = malloc(v->byte_size * new_size);

        // Copy content of old vector.        
        for (int i = 0; i < v->byte_size * v->size; i++)
            ((char*)new_location)[i] = ((char*)v->v)[i];
        
        // Clear old memory.
        free(v->v);

        // Update allocated memory size.
        v->allocated = new_size;
    }

    // Copy v->byte_size bytes from elem to v->v.
    for (int i = 0; i < v->byte_size; i++)
        ((char*)v->v)[v->byte_size * v->size + i] = ((char*)elem)[i];
    v->size++;
}

void PopBack(struct Vector* v)
{
    if (v->size)
        v->size--;
}

void* GetVectorElement(struct Vector* v, int p)
{
    if (p < 0 || p >= v->size)
        return 0;
    return v->v + p * v->byte_size;
}

void DeleteVector(struct Vector* v)
{
    // Clear memory if it exists, and set 0.
    if (v->v)
        free(v->v);
    v->v = NULL;
    v->allocated = v->size = 0;
}
