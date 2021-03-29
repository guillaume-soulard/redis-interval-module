#include "hashmap.h"
#include "redismodule.h"
#include "string.h"
#include "util.h"
#include "linked-list.h"

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
    for (int i = 0; i < array->capacity; i++) {
        array->array[i] = NULL;
    }
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
    Node *existingValue = hashMap->arrays[hashMap->primaryIndex]->array[hashIndex];
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
    Node *foundNode;
    if (hashMap->rehashing) {
        size_t hashInPrimaryIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        Node *nodeInPrimaryIndex = hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex];
        if (nodeInPrimaryIndex == NULL) {
            size_t hashInSecondaryIndex = getHashCode(hashMap->arrays[hashMap->secondaryIndex]->capacity, key);
            foundNode = hashMap->arrays[hashMap->secondaryIndex]->array[hashInSecondaryIndex];
        } else {
            foundNode = nodeInPrimaryIndex;
        }
    } else {
        size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        foundNode = hashMap->arrays[hashMap->primaryIndex]->array[hashIndex];
    }
    if (foundNode != NULL && foundNode->member != NULL && strcmp(foundNode->member, key) == 0) {
        return foundNode;
    } else {
        return NULL;
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
            if (toDelete != NULL && toDelete->member != NULL && strcmp(toDelete->member, key) == 0) {
                hashMap->arrays[hashMap->secondaryIndex]->array[hashInSecondaryIndex] = NULL;
            }
        } else {
            toDelete = hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex];
            if (toDelete != NULL && toDelete->member != NULL && strcmp(toDelete->member, key) == 0) {
                hashMap->arrays[hashMap->primaryIndex]->array[hashInPrimaryIndex] = NULL;
            }
        }
    } else {
        size_t hashIndex = getHashCode(hashMap->arrays[hashMap->primaryIndex]->capacity, key);
        toDelete = hashMap->arrays[hashMap->primaryIndex]->array[hashIndex];
        if (toDelete != NULL && toDelete->member != NULL && strcmp(toDelete->member, key) == 0) {
            hashMap->arrays[hashMap->primaryIndex]->array[hashIndex] = NULL;
        }
    }
    if (toDelete != NULL && toDelete->member != NULL && strcmp(toDelete->member, key) == 0) {
        hashMap->len--;
    }
}

void outputIfMatch(Node *node, const char *match, LinkedList *list) {
    if (node != NULL) {
        if (stringMatchLen(match, strlen(match), node->member, strlen(node->member), 0)) {
            push(list, node);
        }
    }
}

LinkedList *scanHash(HashMap *hashMap, long long int *cursor, char *match, long long int count) {
    long long iteration = 0;
    int read = 1;
    LinkedList *list = newList();
    while (read && iteration <= count && list->len <= count) {
        read = 0;
        if (hashMap->arrays[0] != NULL && *cursor < hashMap->arrays[0]->capacity) {
            outputIfMatch(hashMap->arrays[0]->array[*cursor], match, list);
            if (hashMap->arrays[0]->array[*cursor] != NULL) {
                iteration++;
            }
            read = 1;
        }
        if (hashMap->arrays[1] != NULL && *cursor < hashMap->arrays[1]->capacity) {
            outputIfMatch(hashMap->arrays[1]->array[*cursor], match, list);
            if (hashMap->arrays[0]->array[*cursor] != NULL) {
                iteration++;
            }
            read = 1;
        }
        (*cursor)++;
    }
    if (read == 0) {
        *cursor = 0;
    }
    return list;
}