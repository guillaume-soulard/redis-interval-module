#include "interval-set.h"
#include "string.h"
#include "interval-red-black-tree.h"


IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashMap *hash = createHashMap();
    intervalSet->hash = hash;
    intervalSet->tree = NULL;
    return intervalSet;
}

int add(IntervalSet *intervalSet, char *member, Interval *interval) {
    Node *newNode = insertNode(interval->lowerBound, &intervalSet->tree, member, interval);
    return put(intervalSet->hash, member, newNode);
}

void searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch) {
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    int len = 0;
    findContains(intervalSet->tree, valueToSearch, ctx, &len);
    RedisModule_ReplySetArrayLength(ctx, len);
}

void searchInterval(RedisModuleCtx *ctx, IntervalSet *intervalSet, Interval *intervalToSearch) {
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    int len = 0;
    findOverlaps(intervalSet->tree, intervalToSearch, ctx, &len);
    RedisModule_ReplySetArrayLength(ctx, len);
}
