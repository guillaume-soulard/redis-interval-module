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
    Node *existingNode = get(intervalSet->hash, member);
    if (existingNode != NULL) {
        delete(intervalSet->hash, member);
        deleteNode(&intervalSet->tree, existingNode);
    }
    Node *newNode = insertNode(interval->lowerBound, &intervalSet->tree, member, interval);
    return put(intervalSet->hash, member, newNode);
}

int removeInterval(IntervalSet *intervalSet, char *member) {
    Node *existingNode = get(intervalSet->hash, member);
    if (existingNode != NULL) {
        deleteNode(&intervalSet->tree, existingNode);
        delete(intervalSet->hash, member);
        return 1;
    }
    return 0;
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

void scanIntervalSet(RedisModuleCtx *ctx, IntervalSet *intervalSet, long long cursor, char *match, long long count) {
    size_t len = 0;
    long long stopCursor = scanHash(ctx, intervalSet->hash, cursor, match, count, &len);
    RedisModule_ReplyWithLongLong(ctx, stopCursor);
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    RedisModule_ReplySetArrayLength(ctx, len);
}