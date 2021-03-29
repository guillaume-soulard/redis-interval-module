#include "hashmap.h"
#include "redismodule.h"
#include "string.h"
#include "util.h"
#include "linked-list.h"

#define initialHashMapCapacity 10
#define hashMapGrowthFactor 2
#define hashMapResizeCapacity 100
#define maxItemsToLookup 100000

size_t getHashCode(int hashCapacity, char *key);
HashMapArray *initHashMapArray(size_t capacity);

size_t getHashCode(int hashCapacity, char *key) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash % hashCapacity;
}

HashMapArray *initHashMapArray(size_t capacity) {
    HashMapArray *array = RedisModule_Alloc(sizeof(HashMapArray));
    array->capacity = capacity;
    array->array = RedisModule_Alloc(capacity * sizeof(Node));
    for (int i = 0; i < array->capacity; i++) {
        array->array[i] = NULL;
    }
    return array;
}

HashMap *createHashMap() {
    HashMap *hashMap = RedisModule_Alloc(sizeof(HashMap));
    hashMap->len = 0;
    hashMap->arraysCapacity = hashMapResizeCapacity;
    hashMap->arrays = RedisModule_Alloc(hashMap->arraysCapacity * sizeof(HashMapArray));
    for (int i = 0; i <= hashMap->arraysCapacity; i++) {
        hashMap->arrays[i] = NULL;
    }
    hashMap->arrays[0] = initHashMapArray(initialHashMapCapacity);
    hashMap->arraysLen = 1;
    hashMap->primaryArray = 0;
    hashMap->reHashArrayIndex = 0;
    hashMap->reHashItemIndex = 0;
    return hashMap;
}

void freeHashMapArray(HashMap *hashMap, size_t arrayIndexToFree) {
    if (hashMap->arrays[arrayIndexToFree] != NULL) {
        for (int i = 0; i < hashMap->arrays[arrayIndexToFree]->capacity; i++) {
            if (hashMap->arrays[arrayIndexToFree]->array[i] != NULL) {
                hashMap->arrays[arrayIndexToFree]->array[i] = NULL;
            }
        }
        RedisModule_Free(hashMap->arrays[arrayIndexToFree]->array);
        RedisModule_Free(hashMap->arrays[arrayIndexToFree]);
        hashMap->arrays[arrayIndexToFree] = NULL;
        hashMap->arraysLen--;
    }
}

void freeHashMap(HashMap *hashMap) {
    for (int i = 0; i < hashMap->arraysCapacity; i++) {
        freeHashMapArray(hashMap, i);
    }
    RedisModule_Free(hashMap->arrays);
}

void newHashMapPrimaryArray(HashMap *hashMap) {
    for (int i = 0; i < hashMap->arraysCapacity; i++) {
        if (hashMap->arrays[i] == NULL) {
            hashMap->arrays[i] = initHashMapArray(hashMap->arrays[hashMap->primaryArray]->capacity * hashMapGrowthFactor);
            hashMap->primaryArray = i;
            hashMap->arraysLen++;
            break;
        }
    }
}

int putInPrimary(HashMap *hashMap, char *key, Node *value) {
    size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryArray]->capacity, key);
    Node *existingValue = hashMap->arrays[hashMap->primaryArray]->array[hashIndex];
    if (existingValue != NULL && strcmp(existingValue->member, key) != 0) {
        newHashMapPrimaryArray(hashMap);
        putInPrimary(hashMap, key, value);
    } else {
        hashMap->arrays[hashMap->primaryArray]->array[hashIndex] = value;
    }
    return 1;
}

void moveItemToPrimaryArray(HashMap *hashMap, size_t *itemLookup) {
    while (hashMap->reHashItemIndex < hashMap->len && *itemLookup <= maxItemsToLookup) {
        (*itemLookup)++;
        Node *item = hashMap->arrays[hashMap->reHashArrayIndex]->array[hashMap->reHashItemIndex];
        if (item != NULL) {
            putInPrimary(hashMap, item->member, item);
            hashMap->arrays[hashMap->reHashArrayIndex]->array[hashMap->reHashItemIndex] = NULL;
        }
        hashMap->reHashItemIndex++;
    }
    if (hashMap->reHashItemIndex >= hashMap->arrays[hashMap->reHashArrayIndex]->capacity) {
        freeHashMapArray(hashMap, hashMap->reHashArrayIndex);
    }
}

void incrementalRehash(HashMap *hashMap) {
    size_t itemLookup = 0;
    while (hashMap->arraysLen > 1 && hashMap->reHashArrayIndex < hashMap->arraysCapacity && itemLookup <= maxItemsToLookup) {
        if (hashMap->arrays[hashMap->reHashArrayIndex] != NULL && hashMap->reHashArrayIndex != hashMap->primaryArray) {
            moveItemToPrimaryArray(hashMap, &itemLookup);
        }
        hashMap->reHashArrayIndex++;
    }
}

int put(HashMap *hashMap, char *key, Node *value) {
    putInPrimary(hashMap, key, value);
    hashMap->len++;
    incrementalRehash(hashMap);
    return 1;
}

Node *getOnArray(HashMapArray *array, char *key) {
    if (array != NULL) {
        size_t hashIndex = getHashCode(array->capacity, key);
        Node *foundNode = array->array[hashIndex];
        if (foundNode != NULL && foundNode->member != NULL && strcmp(foundNode->member, key) == 0) {
            return foundNode;
        } else {
            return NULL;
        }
    }
    return NULL;
}

Node *get(HashMap *hashMap, char *key) {
    incrementalRehash(hashMap);
    if (hashMap->arraysLen > 1) {
        Node *foundNode;
        for (int i = 0; i < hashMap->arraysCapacity; i++) {
            foundNode = getOnArray(hashMap->arrays[i], key);
            if (foundNode != NULL) {
                break;
            }
        }
        return foundNode;
    } else {
        return getOnArray(hashMap->arrays[hashMap->primaryArray], key);
    }
}

void delete(HashMap *hashMap, char *key) {
    Node *toDelete;
    if (hashMap->arraysLen > 1) {
        for (int i = 0; i < hashMap->arraysCapacity; i++) {
            toDelete = getOnArray(hashMap->arrays[i], key);
            if (toDelete != NULL) {
                break;
            }
        }
    } else {
        toDelete = getOnArray(hashMap->arrays[hashMap->primaryArray], key);
    }
    if (toDelete != NULL && toDelete->member != NULL && strcmp(toDelete->member, key) == 0) {
        hashMap->len--;
    }
    incrementalRehash(hashMap);
}

void outputIfMatch(Node *node, const char *match, LinkedList *list) {
    if (node != NULL) {
        if (stringMatchLen(match, strlen(match), node->member, strlen(node->member), 0)) {
            push(list, node);
        }
    }
}

LinkedList *scanHash(HashMap *hashMap, long long int *cursor, char *match, long long int count) {
    incrementalRehash(hashMap);
    long long iteration = 0;
    int read = 1;
    LinkedList *list = newList();
    while (read && iteration <= count && list->len <= count) {
        read = 0;
        for (int i = 0; i < hashMap->reHashArrayIndex; i++) {
            HashMapArray *array = hashMap->arrays[i];
            if (array != NULL && *cursor < array->capacity) {
                outputIfMatch(array->array[*cursor], match, list);
                if (array->array[*cursor] != NULL) {
                    iteration++;
                }
                read = 1;
            }
        }
        (*cursor)++;
    }
    if (read == 0) {
        *cursor = 0;
    }
    return list;
}