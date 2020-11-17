#ifndef VECTOR_
#define VECTOR_

/**
 * Copy of std::vector
 * but in C :)))))
 */

typedef struct
{
    void* v;
    int size, allocated, byte_size;
} Vector;

/**
 * Create an empty vector, with the sizeof
 * each element being byte_size.
 */
int CreateEmptyVector(Vector** v, int byte_size);

/**
 * Create a vector with size elements, with
 * the sizeof each element being byte_size.
 */
int CreateVector(Vector** v, int byte_size, int size);

/**
 * PushBack an element into the vector v.
 */
int PushBack(Vector* v, void* elem);

/**
 * Removes the last element.
 * TODO: Make sure vector dealocates if too small, for
 * avoiding overusage of memory.
 */
int PopBack(Vector* v);

/**
 * Returns an element at the p-th position.
 */
int GetVectorElement(Vector* v, int p, void** element);

/**
 * Delete a vector, and clear it's memory.
 */
int DeleteVector(Vector** v);

#endif // VECTOR_
