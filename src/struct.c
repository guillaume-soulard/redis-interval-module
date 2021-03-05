#include "struct.h"


IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashTable *table = RedisModule_Alloc(sizeof(struct HashTable));
    table->primaryArrayCapacity = 10;
    table->primaryArray = RedisModule_Alloc(table->primaryArrayCapacity * sizeof(IntervalTreeNode));
    intervalSet->hash = table;
    intervalSet->tree = RedisModule_Alloc(sizeof(struct IntervalTree));
    return intervalSet;
}

size_t getHashCode(int hashCapacity, RedisModuleString *key) {
    size_t len;
    const char *str = RedisModule_StringPtrLen(key, &len);
    unsigned int hash;
    int i;
    for (i = 0; i < len; i++) {
        hash = hash << 8;
        hash += str[i];
    }
    return hash % hashCapacity;
}

int add(IntervalSet *intervalSet, RedisModuleString *key, Interval *interval) {
    int hashCode = getHashCode(intervalSet->hash->primaryArrayCapacity, key);
    if (intervalSet->hash->primaryArray[hashCode] == NULL) {
        IntervalTreeNode *node = RedisModule_Alloc(sizeof(struct IntervalTreeNode));
        node->interval = interval;
        intervalSet->hash->primaryArray[hashCode] = node;
    } else {
        RedisModule_Free(intervalSet->hash->primaryArray[hashCode]->interval);
        intervalSet->hash->primaryArray[hashCode]->interval = interval;
    }
    return 0;
}

