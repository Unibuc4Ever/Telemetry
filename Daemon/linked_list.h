#ifndef LINKED_LIST_
#define LINKED_LIST_

struct Node {
    struct Node *prev, *next;
    int val;
};

// Delete a node.
int DeleteNode(struct Node *node);

#endif // LINKED_LIST_