#include "struct.h"
#include "string.h"
#include "interval-red-black-tree.h"

//int hashAdd(IntervalSet *intervalSet, char *member, Interval *interval);
//void reHash(IntervalSet *intervalSet);
HashTable *createHash(size_t capacity);

IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashTable *hash = createHash(10);
    intervalSet->hash = hash;
    intervalSet->tree = NULL;
    return intervalSet;
}

HashTable *createHash(size_t capacity) {
    HashTable *hash = RedisModule_Alloc(sizeof(struct HashTable));
    hash->len = 0;
    hash->capacity = capacity;
    hash->array = RedisModule_Alloc(capacity * sizeof (Node));
    return hash;
}

//size_t getHashCode(int hashCapacity, char *key) {
//    unsigned int hash = 0;
//    for (int i = 0; i < strlen(key); i++) {
//        hash = 31 * hash + key[i];
//    }
//    return hash % hashCapacity;
//}

int add(IntervalSet *intervalSet, char *member, Interval *interval) {
    insertNode(interval->lowerBound, &intervalSet->tree, member, interval);
    intervalSet->hash->len++;
    return 1;
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

//int hashAdd(IntervalSet *intervalSet, char *member, Interval *interval) {
//    int hashCode = getHashCode(intervalSet->hash->capacity, member);
//    struct AVLnode *existingNode = intervalSet->hash->array[hashCode];
//    if (existingNode == NULL) {
//        struct AVLnode *node = createTreeNode(interval, member);
//        intervalSet->hash->array[hashCode] = node;
//        intervalSet->hash->len++;
//        treeAdd(intervalSet, node);
//        return 1;
//    } else {
//        if (strcmp(existingNode->member, member) == 0) {
//            RedisModule_Free(intervalSet->hash->array[hashCode]->interval);
//            intervalSet->hash->array[hashCode]->interval = interval;
//            return 0;
//        } else {
//            reHash(intervalSet);
//            return hashAdd(intervalSet, member, interval);
//        }
//    }
//}
//
//// TODO implement incremental rehashing instead
//void reHash(IntervalSet *intervalSet) {
//    HashTable *newHash = createHash(intervalSet->hash->capacity * 2);
//    newHash->len = intervalSet->hash->len;
//    for (int i = 0; i < intervalSet->hash->capacity; i++) {
//        struct AVLnode *node = intervalSet->hash->array[i];
//        if (node != NULL) {
//            int newHashCode = getHashCode(newHash->capacity, intervalSet->hash->array[i]->member);
//            newHash->array[newHashCode] = node;
//            intervalSet->hash->array[i] = NULL;
//        }
//    }
//    RedisModule_Free(intervalSet->hash);
//    intervalSet->hash = newHash;
//}