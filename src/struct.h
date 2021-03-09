#ifndef STRUCT_H
#define STRUCT_H

#include "interval.h"

typedef struct IntervalTree {
    struct IntervalTreeNode *head;
} IntervalTree;


typedef struct IntervalTreeNode {
    Interval* interval;
    double maxUpperBound;
    char *member;
    struct IntervalTreeNode *left;
    struct IntervalTreeNode *right;
} IntervalTreeNode;

typedef struct TableNode {
    IntervalTreeNode *node;
    struct TableNode *next;
} TableNode;

typedef struct Table {
    size_t len;
    size_t capacity;
    TableNode **array;
} Table;

typedef struct HashTable {
    int arrayIndex;
    TableNode *first;
    int nextRehashIndex;
    Table **tables;
} HashTable;

typedef struct IntervalSet {
    IntervalTree *tree;
    HashTable *hash;
} IntervalSet;


IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);

#endif