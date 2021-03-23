#ifndef STRUCT_H
#define STRUCT_H

#include "interval.h"
#include "interval-red-black-tree.h"

typedef struct HashTable {
    size_t len;
    size_t capacity;
    Node **array;
} HashTable;

typedef struct IntervalSet {
    Node *tree;
    HashTable *hash;
} IntervalSet;

IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);
void searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch);
void searchInterval(RedisModuleCtx *ctx, IntervalSet *intervalSet, Interval *intervalToSearch);
#endif