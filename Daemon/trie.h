#ifndef TRIE_
#define TRIE_

#include "universal_type.h"
#include "vector.h"

/**
 * Allowed characters in the channel's name:
 * [a-z][A-Z][0-9]/
 */

/**
 * Add an UniversalType at a certain path within the Trie.
 */
int Insert(char* channel, UniversalType element);

/**
 * Returns all the UniversalTypes from within the Trie.
 */
int ExtractSubtree(char* channel, Vector* answer);

#endif // TRIE_