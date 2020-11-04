#ifndef TRIE_
#define TRIE_

#include "linked_list.h"

// Adds the node `node` to the chanel `channel`.
void AddNode(const char* channel, struct Node* node);

// Returns a linked list with all the node influenced by a channel.
struct Node* FindSubtree(const char* channel);

#endif // TRIE_