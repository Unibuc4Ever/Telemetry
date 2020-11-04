#include "linked_list.h"

#include <stdlib.h>

int DeleteNode(struct Node *node)
{
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    free(node);
    return 0;
}
