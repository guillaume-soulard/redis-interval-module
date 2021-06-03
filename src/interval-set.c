#include "interval-set.h"
#include "string.h"
#include "treap-tree.h"
#include "linked-list.h"

IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashMap *hash = createHashMap();
    intervalSet->hash = hash;
    intervalSet->tree = NULL;
    return intervalSet;
}

int add(IntervalSet *intervalSet, char *member, Interval *interval) {
    Item *insertedNode = put(intervalSet->hash, member, interval);
    if (insertedNode != NULL) {
        insertNode(&intervalSet->tree, insertedNode);
    }
    return insertedNode != NULL;
}

int removeInterval(IntervalSet *intervalSet, char *member) {
    Item *existingItem = get(intervalSet->hash, member);
    if (existingItem != NULL) {
        deleteNode(&intervalSet->tree, existingItem);
        delete(intervalSet->hash, member);
        return 1;
    }
    return 0;
}

void searchValue(RedisModuleCtx *ctx, IntervalSet *intervalSet, double valueToSearch, long long count) {
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    int len = 0;
    findContains(intervalSet->tree, valueToSearch, ctx, &len, count);
    RedisModule_ReplySetArrayLength(ctx, len);
}

void searchInterval(RedisModuleCtx *ctx, IntervalSet *intervalSet, Interval *intervalToSearch, long long count) {
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    int len = 0;
    findOverlaps(intervalSet->tree, intervalToSearch, ctx, &len, count);
    RedisModule_ReplySetArrayLength(ctx, len);
}

void scanIntervalSet(RedisModuleCtx *ctx, IntervalSet *intervalSet, long long cursor, const char *match, long long count) {
    char *matchPattern = strdup(match);
    LinkedList *list = scanHash(intervalSet->hash, &cursor, matchPattern, count);
    RedisModule_ReplyWithArray(ctx, 2);
    RedisModule_ReplyWithLongLong(ctx, cursor);
    RedisModule_ReplyWithArray(ctx, list->len);
    LinkedListNode *listNode = list->head;
    while (listNode != NULL) {
        outputInterval(ctx, listNode->item->member, listNode->item->interval);
        listNode = listNode->next;
    }
}