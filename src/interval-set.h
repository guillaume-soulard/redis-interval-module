#ifndef INTERVAL_SET_H
#define INTERVAL_SET_H

#include "interval.h"
#include "treap-tree.h"
#include "hashmap.h"

typedef struct IntervalSet {
    TreeNode *tree;
    HashMap *hash;
} IntervalSet;

IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, char *value, Interval *interval);
void searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch, long long count);
void searchInterval(RedisModuleCtx *ctx, IntervalSet *intervalSet, Interval *intervalToSearch, long long count);
int removeInterval(IntervalSet *intervalSet, char *member);
void scanIntervalSet(RedisModuleCtx *ctx, IntervalSet *intervalSet, long long cursor, const char *match, long long count);
#endif