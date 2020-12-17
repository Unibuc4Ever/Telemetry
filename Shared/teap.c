#include <stdlib.h>

#include "treap.h"

typedef struct {
    Treap *st, *dr;
} TPair;

void Recalc(Treap* t)
{
    t->g = 1;
    if (t->st)
        t->g += t->st->g;
    if (t->dr)
        t->g += t->dr->g;
}

Treap* Join(Treap* a, Treap* b)
{
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;
    
    if (a->priority > b->priority) {
        a->dr = Join(a->dr, b);
        Recalc(a);
        return a;
    }

    b->st = Join(a, b->st);
    Recalc(b);
    return b;
}

// { <= val, > val }
TPair Split(Treap* t, int val)
{
    if (!t)
        return (TPair) { NULL, NULL };
    
    if (t->val <= val) {
        TPair s = Split(t->dr, val);
        t->dr = s.st;
        Recalc(t);
        return (TPair) { t, s.dr };
    }

    TPair s = Split(t->st, val);
    t->st = s.dr;
    Recalc(t);
    return (TPair) { s.st, t };
}


int TreapFind(const Treap* t, int val, void** ans)
{
    while (t != NULL) {
        if (t->val == val) {
            *ans = t->data;
            return 0;
        }
        if (t->val > val)
            t = t->st;
        else
            t = t->dr;
    }
    return -1;
}

int TreapInsert(Treap** t, int val, void* data)
{
    void* ans = NULL;
    if (!TreapFind(*t, val, &ans))
        return -1; // Already exists.
    
    TPair pair = Split(*t, val);
    Treap* nod = malloc(sizeof(*nod));
    nod->st = nod->dr = NULL;
    nod->data = data;
    nod->val = val;
    nod->g = 1;
    nod->priority = rand() | (rand() >> 20);

    *t = Join(Join(pair.st, nod), pair.dr);

    return 0;
}

int TreapErase(Treap** t, int val)
{
    Treap* nod = *t;
    void* ans = NULL;
    if (TreapFind(nod, val, &ans))
        return -1; // Already exists.
    
    TPair pair1 = Split(nod, val);
    TPair pair2 = Split(pair1.st, val - 1);

    *t = Join(pair2.st, pair1.dr);

    if (!pair2.dr)
        return -1;

    free(pair2.dr);

    return 0;
}

int ClearTreap(Treap** t)
{
    if (*t == NULL)
        return 0;
    ClearTreap(&((*t)->st));
    ClearTreap(&((*t)->dr));
    free(*t);
    return 0;
}
