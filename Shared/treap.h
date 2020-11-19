#ifndef TREAP_
#define TREAP_

/**
 * Self-Balanced BST, used for matching integer keys with values.
 * An empty Treap is considered to be NULL.
 */

typedef struct __Treap Treap;

struct __Treap
{
    Treap *st, *dr;
    void* data;
    int val, g, priority;
};

// Find a value in a treap.
int TreapFind(const Treap* t, int val, void** ans);

// Remove a value from a treap.
int TreapErase(Treap** t, int val);

// Insert a value in a treap.
int TreapInsert(Treap** t, int val, void* data);

// Clears all the treap.
int ClearTreap(Treap** t);

#endif // TREAP_
