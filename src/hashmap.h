#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "interval-red-black-tree.h"

typedef struct HashMapArray {
    size_t capacity;
    Node **array;
} HashMapArray;

typedef struct HashMap {
    size_t len;
    HashMapArray **arrays;
    size_t primaryIndex;
    size_t secondaryIndex;
    int rehashing;
    size_t reHashIndex;
} HashMap;

HashMap *createHashMap();
void freeHashMap(HashMap *hashMap);
int put(HashMap *hashMap, char *key, Node *value);
Node *get(HashMap *hashMap, char *key, Node *value);
int delete(HashMap *hashMap, char *key);

#endif