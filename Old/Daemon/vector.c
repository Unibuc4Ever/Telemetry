#include <stdlib.h>

#include "vector.h"
#include "errors.h"

int CreateEmptyVector(Vector** v, int byte_size)
{
    if (byte_size <= 0)
        return INVALID_VECTOR_OPERATION;

    *v = malloc(sizeof **v);
    (*v)->v = 0;
    (*v)->size = (*v)->allocated = 0;
    (*v)->byte_size = byte_size;
    return 0;
}


int CreateVector(Vector** v, int byte_size, int size)
{
    if (size <= 0 || byte_size <= 0)
        return INVALID_VECTOR_OPERATION;

    *v = malloc(sizeof **v);
    (*v)->size = (*v)->allocated = size;
    (*v)->byte_size = byte_size;
    (*v)->v = malloc(size * byte_size);
    return 0;
}


int PushBack(Vector* v, void* elem)
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

        // Update memory.
        v->v = new_location;
        
        // Update allocated memory size.
        v->allocated = new_size;
    }

    // Copy v->byte_size bytes from elem to v->v.
    for (int i = 0; i < v->byte_size; i++)
        ((char*)v->v)[v->byte_size * v->size + i] = ((char*)elem)[i];
    v->size++;

    return 0;
}

int PopBack(Vector* v)
{
    if (v->size) {
        v->size--;
        return 0;
    }
    return INVALID_VECTOR_OPERATION;
}

int GetVectorElement(Vector* v, int p, void** element)
{
    if (p < 0 || p >= v->size)
        return INVALID_VECTOR_OPERATION;
    *element = v->v + p * v->byte_size;
    return 0;
}

int DeleteVector(Vector** v)
{
    // Clear memory if it exists, and set 0.
    if ((*v)->v)
        free((*v)->v);
    free(*v);
    *v = NULL;
    
    return 0;
}
