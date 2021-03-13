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

typedef struct HashTable {
    size_t len;
    size_t capacity;
    IntervalTreeNode **array;
} HashTable;

typedef struct IntervalSet {
    IntervalTree *tree;
    HashTable *hash;
} IntervalSet;

IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);
int searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch);

#endif