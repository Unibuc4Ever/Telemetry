#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define SIGMA (26 * 2 + 10 + 1)

struct TrieNode {
    struct Vector universal_elements;
    struct TrieNode* children[SIGMA];
} root;

// Create a new trie node.
struct TrieNode* CreateTrieNode()
{
    struct TrieNode* nod = malloc(sizeof nod);
    nod->universal_elements = CreateEmptyVector(sizeof (struct UniversalType));
    memset(nod->children, nod->children + SIGMA, 0);
}

int GetCharId(char c)
{
    if ('a' <= c && c <= 'z')
        return c - 'a';
    if ('A' <= c && c <= 'Z')
        return 26 + c - 'A';
    if ('0' <= c <= '9')
        return 2 * 26 + c - '0';
    return 2 * 26 + 10;
}

int Insert(char* channel, struct UniversalType element)
{
    struct TrieNode* node = &root;
    int l = strlen(channel);

    if (channel[0] != '/')
        return 100;
    
    for (int i = 0; i < l; i++) {
        int char_id = GetCharId(channel[i]);

        // If node doesn't exist I have to create it.
        if (!node->children[char_id])
            node->children[char_id] = CreateTrieNode();

        node = node->children[char_id];
    }

    PushBack(&(node->universal_elements), &element);

    return 0;
}

void GetSubtreeElements(struct TrieNode* node, struct Vector* vec)
{
    for (int i = 0; i < node->universal_elements.size; i++) {
        struct UniversalType* element = GetVectorElement(&(node->universal_elements), i);
        PushBack(vec, element);
    }

    for (int i = 0; i < SIGMA; i++)
        if (node->children[i])
            GetSubtreeElements(node->children[i], vec);
}

struct Vector ExtractSubtree(char* channel)
{
    struct Vector ans = CreateEmptyVector(sizeof (struct UniversalType));
    
    GetSubtreeElements(&root, &ans);
}
