#ifndef STRUCT_H
#define STRUCT_H

#include "interval.h"
#include "intervalredblacktree.h"


typedef struct HashTable {
    size_t len;
    size_t capacity;
    struct AVLnode **array;
} HashTable;

typedef struct IntervalSet {
    struct AVLnode *tree;
    HashTable *hash;
} IntervalSet;

IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);
int searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch);

#endif