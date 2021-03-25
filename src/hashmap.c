#include "hashmap.h"
#include "redismodule.h"
#include "string.h"

size_t getHashCode(int hashCapacity, char *key);
HashMapArray *initHashMapArray(size_t capacity);
void freeHashMapArray(HashMapArray *array);

size_t getHashCode(int hashCapacity, char *key) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash % hashCapacity;
}

HashMapArray *initHashMapArray(size_t capacity) {
    HashMapArray *array = RedisModule_Alloc(sizeof (HashMapArray));
    array->capacity = capacity;
    array->array = RedisModule_Alloc(capacity * sizeof (Node));
    return array;
}

HashMap *createHashMap() {
    HashMap *hashMap = RedisModule_Alloc(sizeof (HashMap));
    hashMap->len = 0;
    hashMap->arrays = RedisModule_Alloc(2 * sizeof (HashMapArray));
    hashMap->arrays[0] = initHashMapArray(10);
    hashMap->arrays[1] = NULL;
    hashMap->primaryIndex = 0;
    hashMap->secondaryIndex = 1;
    return hashMap;
}

void freeHashMapArray(HashMapArray *array) {
    if (array != NULL) {
        for (int i = 0; i < array->capacity; i++) {
            if (array->array[i] != NULL) {
                array->array[i] = NULL;
            }
        }
        RedisModule_Free(array);
    }
}

void freeHashMap(HashMap *hashMap) {
    freeHashMapArray(hashMap->arrays[0]);
    freeHashMapArray(hashMap->arrays[1]);
    RedisModule_Free(hashMap->arrays);
}

void incrementalRehash(HashMap *hashMap) {
    if (hashMap->rehashing) {
        size_t itemLookup = 0;
        while (hashMap->reHashIndex < hashMap->len && itemLookup <= 100000) {
            itemLookup++;
            Node *item = hashMap->arrays[hashMap->secondaryIndex]->array[hashMap->reHashIndex];
            if (item != NULL) {
                size_t newHashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, item->member);
                hashMap->arrays[hashMap->primaryIndex]->array[newHashIndex] = item;
                hashMap->arrays[hashMap->secondaryIndex]->array[hashMap->reHashIndex] = NULL;
            }
            hashMap->reHashIndex++;
        }
        if (hashMap->reHashIndex >= hashMap->len) {
            freeHashMapArray(hashMap->arrays[hashMap->secondaryIndex]);
            hashMap->arrays[hashMap->secondaryIndex] = hashMap->arrays[hashMap->primaryIndex];
            hashMap->arrays[hashMap->primaryIndex] = NULL;
            hashMap->rehashing = 0;
            hashMap->primaryIndex = 0;
            hashMap->secondaryIndex = 1;
        }
    }
}

int put(HashMap *hashMap, char *key, Node *value) {
    size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
    Node *existingValue = get(hashMap, key);
    if (existingValue != NULL && strcmp(existingValue->member, key) != 0 && hashMap->rehashing == 0) {
        hashMap->arrays[hashMap->secondaryIndex] = initHashMapArray(
                hashMap->arrays[hashMap->primaryIndex]->capacity * 2);
        size_t hashSecondaryIndex = getHashCode(hashMap->arrays[hashMap->secondaryIndex]->capacity, key);
        hashMap->arrays[hashMap->secondaryIndex]->array[hashSecondaryIndex] = value;
        hashMap->len++;
        hashMap->rehashing = 1;
        hashMap->primaryIndex = 1;
        hashMap->secondaryIndex = 0;
        hashMap->reHashIndex = 0;
    } else if (existingValue != NULL && strcmp(existingValue->member, key) != 0 && hashMap->rehashing == 1) {
        // error
        return -1;
    } else {
        hashMap->arrays[hashMap->primaryIndex]->array[hashIndex] = value;
        hashMap->len++;
    }
    incrementalRehash(hashMap);
    return 1;
}

Node *get(HashMap *hashMap, char *key) {
    if (hashMap->rehashing) {
        size_t hashInPrimaryIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        Node *nodeInPrimaryIndex = hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex];
        if (nodeInPrimaryIndex == NULL) {
            size_t hashInSecondaryIndex = getHashCode(hashMap->arrays[hashMap->secondaryIndex]->capacity, key);
            return hashMap->arrays[hashMap->secondaryIndex]->array[hashInSecondaryIndex];
        }
        return nodeInPrimaryIndex;
    } else {
        size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        return hashMap->arrays[hashMap->primaryIndex]->array[hashIndex];
    }
}

void delete(HashMap *hashMap, char *key) {
    Node *toDelete;
    if (hashMap->rehashing) {
        size_t hashInPrimaryIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        Node *nodeInPrimaryIndex = hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex];
        if (nodeInPrimaryIndex == NULL) {
            size_t hashInSecondaryIndex = getHashCode(hashMap->arrays[hashMap->secondaryIndex]->capacity, key);
            toDelete = hashMap->arrays[hashMap->secondaryIndex]->array[hashInSecondaryIndex];
            hashMap->arrays[hashMap->secondaryIndex]->array[hashInSecondaryIndex] = NULL;
        } else {
            toDelete = hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex];
            hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex] = NULL;
        }
    } else {
        size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        toDelete = hashMap->arrays[hashMap->primaryIndex]->array[hashIndex];
        hashMap->arrays[hashMap->primaryIndex]->array[hashIndex] = NULL;
    }
    if (toDelete != NULL) {
        hashMap->len--;
    }
}