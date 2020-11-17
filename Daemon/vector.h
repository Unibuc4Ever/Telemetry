#ifndef VECTOR_
#define VECTOR_

/**
 * Copy of std::vector
 * but in C :)))))
 */

struct Vector
{
    void* v;
    int size, allocated, byte_size;
};

/**
 * Create an empty vector, with the sizeof
 * each element being byte_size.
 */
struct Vector CreateEmptyVector(int byte_size);

/**
 * Create a vector with size elements, with
 * the sizeof each element being byte_size.
 */
struct Vector CreateVector(int byte_size, int size);

/**
 * PushBack an element into the vector v.
 */
void PushBack(struct Vector* v, void* elem);

/**
 * Removes the last element.
 * TODO: Make sure vector dealocates if too small, for
 * avoiding overusage of memory.
 */
void PopBack(struct Vector* v);

/**
 * Returns an element at the p-th position.
 */
void* GetVectorElement(struct Vector* v, int p);

/**
 * Delete a vector, and clear it's memory.
 */
void DeleteVector(struct Vector* v);

#endif // VECTOR_
