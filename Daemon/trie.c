#include <stdlib.h>
#include <string.h>

#include "trie.h"
#include "errors.h"

#define SIGMA (26 * 2 + 10 + 1)

struct __TrieNode_;
typedef struct __TrieNode_ TrieNode;

struct __TrieNode_
{
    Vector* universal_elements;
    TrieNode* children[SIGMA];
};
TrieNode root;

// Create a new trie node.
TrieNode* CreateTrieNode()
{
    TrieNode* nod = malloc(sizeof nod);
    CreateEmptyVector(&(nod->universal_elements), sizeof (UniversalType));
    memset(nod->children, 0, sizeof nod->children);
    return nod;
}

int GetCharId(char c)
{
    if ('a' <= c && c <= 'z')
        return c - 'a';
    if ('A' <= c && c <= 'Z')
        return 26 + c - 'A';
    if ('0' <= c && c <= '9')
        return 2 * 26 + c - '0';
    return 2 * 26 + 10;
}

int TrieInsert(char* channel, UniversalType element)
{
    TrieNode* node = &root;
    int l = strlen(channel);

    if (l == 0)
        return EMPTY_CHANNEL_ERROR;
    if (channel[0] != '/')
        return INVALID_CHANNEL_NAME;
    
    for (int i = 0; i < l; i++) {
        int char_id = GetCharId(channel[i]);
        if (char_id >= SIGMA)
            return INVALID_CHANNEL_NAME;

        // If node doesn't exist I have to create it.
        if (!node->children[char_id])
            node->children[char_id] = CreateTrieNode();

        node = node->children[char_id];
    }

    int error = PushBack(node->universal_elements, &element);

    return error;
}

int GetSubtreeElements(TrieNode* node, Vector* vec)
{
    for (int i = 0; i < node->universal_elements->size; i++) {
        UniversalType* element;
        int err = GetVectorElement(node->universal_elements, i, (void**)&element);
        if (!err)
            err = PushBack(vec, element);
        if (err)
            return err;
    }

    for (int i = 0; i < SIGMA; i++) {
        if (node->children[i]) {
            int err = GetSubtreeElements(node->children[i], vec);
            if (err)
                return err;
        }
    }

    return 0;
}

int TrieExtractSubtree(char* channel, Vector* answer)
{
    int l = strlen(channel);
    
    if (l == 0)
        return EMPTY_CHANNEL_ERROR;
    if (channel[0] != '/')
        return INVALID_CHANNEL_NAME;
    
    TrieNode* node = &root;

    for (int i = 0; i < l; i++) {
        int char_id = GetCharId(channel[i]);
        if (char_id >= SIGMA)
            return INVALID_CHANNEL_NAME;

        // If node doesn't exist I have to return.
        if (!node->children[char_id])
            return 0;
        
        node = node->children[char_id];
    }

    int err = GetSubtreeElements(&root, answer);
    return err;
}
