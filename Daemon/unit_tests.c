#include <stdio.h>

#include "vector.h"
#include "universal_type.h"
#include "trie.h"

char messages[2][100] = { "FAIL\n", "PASS\n" };

void TestUniversalType()
{
    int pass = 1;

    UniversalType *ut;
    long long x = 1e7 + 1e16;
    long long y = x;

    if (CreateUniversalType(&ut, &x, sizeof x)) 
        pass = 0;
    
    if (ut->size != sizeof x)
        pass = 0;
    
    if (*(long long*)ut->data != y)
        pass = 0;
    
    if (DeleteUniversalType(&ut))
        pass = 0;
        
    printf("TestUniversalType: %s", messages[pass]);
}

void TestVector()
{
    int pass = 1;

    Vector *v;
    pass &= !CreateEmptyVector(&v, sizeof (int));

    for (int i = 10; i < 20; i++) {
        int x = i;
        pass &= !PushBack(v, &x);
    }

    for (int i = 0; i < 10; i++) {
        int* x;
        pass &= !GetVectorElement(v, i, (void**)&x);
        pass &= (*x == i + 10);
    }
    pass &= !DeleteVector(&v);

    pass &= !CreateVector(&v, sizeof (int), 10);

    for (int i = 0; i < 10; i++) {
        int *x;
        pass &= !GetVectorElement(v, i, (void**)&x);
        *x = i + 10;
    }

    for (int i = 0; i < 10; i++) {
        int* x;
        pass &= !GetVectorElement(v, i, (void**)&x);
        pass &= (*x == i + 10);
    }
    pass &= !DeleteVector(&v);

    printf("TestVector: %s", messages[pass]);
}   

void TestTrie()
{
    int pass = 1;

    UniversalType *a, *b, *c;
    int x = 10;
    pass &= !CreateUniversalType(&a, &x, sizeof x);
    x++;
    pass &= !CreateUniversalType(&b, &x, sizeof x);
    x++;
    pass &= !CreateUniversalType(&c, &x, sizeof x);

    pass &= TrieInsert("/channel/", *a);
}

void StartUnitTests()
{
    printf("Starting unit tests:\n");
    TestUniversalType();
    TestVector();
    TestTrie();
    printf("Tests finished!\n");
}