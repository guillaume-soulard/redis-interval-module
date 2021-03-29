#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "linked-list.h"
#include "interval-red-black-tree.h"

typedef struct LinkedListNode {
    Node *item;
    struct LinkedListNode *next;
} LinkedListNode;

typedef struct LinkedList {
    size_t len;
    LinkedListNode *head;
    LinkedListNode *tail;
} LinkedList;

LinkedList *newList();
void push(LinkedList *list, Node *item);

#endif