#ifndef TRIE_
#define TRIE_

#include "universal_type.h"
#include "vector.h"

/**
 * Allowed characters in the channel's name:
 * [a-z][A-Z][0-9]/
 * 
 * Channel has to ALWAYS END WITH '/'!!
 */

/**
 * Add an UniversalType at a certain path within the Trie.
 */
int TrieInsert(char* channel, UniversalType element);

/**
 * Returns all the UniversalTypes from within the Trie.
 */
int TrieExtractSubtree(char* channel, Vector* answer);

#endif // TRIE_