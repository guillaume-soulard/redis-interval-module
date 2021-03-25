#ifndef INTERVAL_SET_H
#define INTERVAL_SET_H

#include "interval.h"
#include "interval-red-black-tree.h"
#include "hashmap.h"

typedef struct IntervalSet {
    Node *tree;
    HashMap *hash;
} IntervalSet;

IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);
void searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch);
void searchInterval(RedisModuleCtx *ctx, IntervalSet *intervalSet, Interval *intervalToSearch);
#endif