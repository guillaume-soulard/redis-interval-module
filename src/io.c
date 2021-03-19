#include "io.h"
#include "redismodule.h"
#include "struct.h"
//#include "string.h"

void *IntervalSetTypeRdbLoad(RedisModuleIO *rdb, int encver) {
//    if (encver != 0) {
//        RedisModule_LogIOError(rdb, "warning","Can't load data with version %d", encver);
//        return NULL;
//    }
//    size_t len = RedisModule_LoadUnsigned(rdb);
//    IntervalSet *intervalSet = createIntervalSet();
//    while (len--) {
//        char *member = RedisModule_LoadStringBuffer(rdb, NULL);
//        int includeLowerBound = RedisModule_LoadUnsigned(rdb);
//        double lowerBound = RedisModule_LoadDouble(rdb);
//        double upperBound = RedisModule_LoadDouble(rdb);
//        int includeUpperBound = RedisModule_LoadUnsigned(rdb);
//        Interval *interval = createInterval(includeLowerBound, lowerBound, upperBound, includeUpperBound);
//        add(intervalSet, member, interval);
//    }
//    return intervalSet;
}

void IntervalSetTypeRdbSave(RedisModuleIO *rdb, void *value) {
//    IntervalSet *intervalSet = value;
//    RedisModule_SaveUnsigned(rdb, intervalSet->hash->len);
//    for (int i = 0; i< intervalSet->hash->capacity; i++) {
//        IntervalTreeNode *existingValue = intervalSet->hash->array[i];
//        if (existingValue != NULL) {
//            RedisModule_SaveStringBuffer(rdb, existingValue->member, strlen(existingValue->member));
//            RedisModule_SaveUnsigned(rdb, existingValue->interval->includeLowerBound);
//            RedisModule_SaveDouble(rdb, existingValue->interval->lowerBound);
//            RedisModule_SaveDouble(rdb, existingValue->interval->upperBound);
//            RedisModule_SaveUnsigned(rdb, existingValue->interval->includeUpperBound);
//        }
//    }
}

void IntervalSetTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
//    IntervalSet *intervalSet = value;
//    for (int i = 0; i< intervalSet->hash->capacity; i++) {
//        IntervalTreeNode *existingValue = intervalSet->hash->array[i];
//        if (existingValue != NULL) {
//            char intervalStr[203];
//            char includeLowerChar = '[';
//            char includeUpperChar = ']';
//            if (existingValue->interval->includeLowerBound == 0) {
//                includeLowerChar = ']';
//            }
//            if (existingValue->interval->includeUpperBound == 0) {
//                includeUpperChar = ']';
//            }
//            snprintf(intervalStr, 203, "%c%f,%f%c",
//                     includeLowerChar,
//                     existingValue->interval->lowerBound,
//                     existingValue->interval->upperBound,
//                     includeUpperChar);
//            RedisModule_EmitAOF(aof, "IADD", "scc", key, intervalStr, existingValue->member);
//        }
//    }
}

//void freeIntervalNode(IntervalTreeNode *node) {
//    if (node != NULL) {
//        RedisModule_Free(node->member);
//        RedisModule_Free(node->interval);
//        freeIntervalNode(node->left);
//        freeIntervalNode(node->right);
//        RedisModule_Free(node);
//    }
//}

void freeIntervalSet(IntervalSet *intervalSet) {
//    for (int i = 0; i < intervalSet->hash->capacity; i++){
//        intervalSet->hash->array[i] = NULL;
//    }
//    RedisModule_Free(intervalSet->hash->array);
//    RedisModule_Free(intervalSet->hash);
//    freeIntervalNode(intervalSet->tree->head);
//    RedisModule_Free(intervalSet->tree);
//    RedisModule_Free(intervalSet);
}

void IntervalSetTypeFree(void *value) {
    freeIntervalSet(value);
}
