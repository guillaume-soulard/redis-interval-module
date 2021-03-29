#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "interval-red-black-tree.h"
#include "linked-list.h"

typedef struct HashMapArray {
    size_t capacity;
    Node **array;
} HashMapArray;

typedef struct HashMap {
    size_t len;
    HashMapArray **arrays;
    size_t arraysLen;
    size_t primaryArray;
    size_t arraysCapacity;
    size_t reHashItemIndex;
    size_t reHashArrayIndex;
} HashMap;

HashMap *createHashMap();
void freeHashMap(HashMap *hashMap);
int put(HashMap *hashMap, char *key, Node *value);
Node *get(HashMap *hashMap, char *key);
void delete(HashMap *hashMap, char *key);
LinkedList *scanHash(HashMap *hashMap, long long int *cursor, char *match, long long int count);
#endif