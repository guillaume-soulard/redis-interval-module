#include "io.h"
#include "redismodule.h"
#include "struct.h"
#include "interval-red-black-tree.h"
#include "string.h"

void *IntervalSetTypeRdbLoad(RedisModuleIO *rdb, int encver) {
    if (encver != 0) {
        RedisModule_LogIOError(rdb, "warning","Can't load data with version %d", encver);
        return NULL;
    }
    size_t len = RedisModule_LoadUnsigned(rdb);
    IntervalSet *intervalSet = createIntervalSet();
    while (len--) {
        char *member = RedisModule_LoadStringBuffer(rdb, NULL);
        int includeLowerBound = RedisModule_LoadUnsigned(rdb);
        double lowerBound = RedisModule_LoadDouble(rdb);
        double upperBound = RedisModule_LoadDouble(rdb);
        int includeUpperBound = RedisModule_LoadUnsigned(rdb);
        Interval *interval = createInterval(includeLowerBound, lowerBound, upperBound, includeUpperBound);
        add(intervalSet, member, interval);
    }
    return intervalSet;
}

void saveNode(RedisModuleIO *rdb, Node *node) {
    if (node != NULL) {
        saveNode(rdb, node->left);
        saveNode(rdb, node->right);
        RedisModule_SaveStringBuffer(rdb, node->member, strlen(node->member));
        RedisModule_SaveUnsigned(rdb, node->interval->includeLowerBound);
        RedisModule_SaveDouble(rdb, node->interval->lowerBound);
        RedisModule_SaveDouble(rdb, node->interval->upperBound);
        RedisModule_SaveUnsigned(rdb, node->interval->includeUpperBound);
    }
}

void IntervalSetTypeRdbSave(RedisModuleIO *rdb, void *value) {
    IntervalSet *intervalSet = value;
    RedisModule_SaveUnsigned(rdb, intervalSet->hash->len);
    saveNode(rdb, intervalSet->tree);
}

void rewriteNode(RedisModuleIO *aof, RedisModuleString *key, Node *node) {
    if (node != NULL) {
        rewriteNode(aof, key, node->left);
        rewriteNode(aof, key, node->right);
        char intervalStr[203];
        char includeLowerChar = '[';
        char includeUpperChar = ']';
        if (node->interval->includeLowerBound == 0) {
            includeLowerChar = ']';
        }
        if (node->interval->includeUpperBound == 0) {
            includeUpperChar = ']';
        }
        snprintf(intervalStr, 203, "%c%f,%f%c",
                 includeLowerChar,
                 node->interval->lowerBound,
                 node->interval->upperBound,
                 includeUpperChar);
        RedisModule_EmitAOF(aof, "IADD", "scc", key, intervalStr, node->member);
    }
}

void IntervalSetTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
    IntervalSet *intervalSet = value;
    rewriteNode(aof, key, intervalSet->tree);
}

void freeIntervalSet(IntervalSet *intervalSet) {
    RedisModule_Free(intervalSet->hash->array);
    RedisModule_Free(intervalSet->hash);
    freeIntervalSetTreeNode(intervalSet->tree);
    RedisModule_Free(intervalSet);
}

void IntervalSetTypeFree(void *value) {
    freeIntervalSet(value);
}
