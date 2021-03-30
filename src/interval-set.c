#include "interval-set.h"
#include "string.h"
#include "interval-red-black-tree.h"
#include "linked-list.h"

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
        freeIntervalSetTreeNode(existingNode);
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
    LinkedList *list = scanHash(intervalSet->hash, &cursor, match, count);
    RedisModule_ReplyWithLongLong(ctx, cursor);
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    LinkedListNode *listNode = list->head;
    while (listNode != NULL) {
        outputInterval(ctx, listNode->item->member, listNode->item->interval);
        listNode = listNode->next;
    }
    RedisModule_ReplySetArrayLength(ctx, list->len);
}