#include "struct.h"
#include "float.h"
#include "string.h"

void treeAdd(IntervalSet *intervalSet, IntervalTreeNode *node);
IntervalTreeNode *createTreeNode(Interval *interval, char *member);
double treeNodeAdd(IntervalTreeNode *node, IntervalTreeNode *newNode);
int hashAdd(IntervalSet *intervalSet, char *member, Interval *interval);
void reHash(IntervalSet *intervalSet);
HashTable *createHash(size_t capacity);

IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashTable *hash = createHash(10);
    intervalSet->hash = hash;
    intervalSet->tree = RedisModule_Alloc(sizeof(struct IntervalTree));
    intervalSet->tree->head = NULL;
    return intervalSet;
}

HashTable *createHash(size_t capacity) {
    HashTable *hash = RedisModule_Alloc(sizeof(struct HashTable));
    hash->len = 0;
    hash->capacity = capacity;
    hash->array = RedisModule_Alloc(capacity * sizeof (IntervalTreeNode));
    return hash;
}


size_t getHashCode(int hashCapacity, char *key) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash % hashCapacity;
}

int add(IntervalSet *intervalSet, char *member, Interval *interval) {
    return hashAdd(intervalSet, member, interval);
}

void treeAdd(IntervalSet *intervalSet, IntervalTreeNode *node) {
    if (intervalSet->tree->head == NULL) {
        intervalSet->tree->head = node;
    } else {
        treeNodeAdd(intervalSet->tree->head, node);
    }
}

IntervalTreeNode *createTreeNode(Interval *interval, char *member) {
    IntervalTreeNode *node = RedisModule_Alloc(sizeof (IntervalTreeNode));
    node->interval = interval;
    node->member = member;
    node->left = NULL;
    node->right = NULL;
    node->maxUpperBound = interval->upperBound;
    return node;
}

double treeNodeAdd(IntervalTreeNode *node, IntervalTreeNode *newNode) {
    double maxBound = DBL_MIN;
    if (node->interval->lowerBound < newNode->interval->lowerBound) {
        if (node->left == NULL) {
            node->left = newNode;
            maxBound = node->left->maxUpperBound;
        } else {
            maxBound = treeNodeAdd(node->left, newNode);
        }
    } else {
        if (node->right == NULL) {
            node->right = newNode;
            maxBound = node->right->maxUpperBound;
        } else {
            maxBound = treeNodeAdd(node->right, newNode);
        }
    }
    if (node->maxUpperBound > maxBound) {
        maxBound = node->maxUpperBound;
    } else {
        node->maxUpperBound = maxBound;
    }
    return maxBound;
}

int hashAdd(IntervalSet *intervalSet, char *member, Interval *interval) {
    int hashCode = getHashCode(intervalSet->hash->capacity, member);
    IntervalTreeNode *existingNode = intervalSet->hash->array[hashCode];
    if (existingNode == NULL) {
        IntervalTreeNode *node = createTreeNode(interval, member);
        intervalSet->hash->array[hashCode] = node;
        intervalSet->hash->len++;
        treeAdd(intervalSet, node);
        return 1;
    } else {
        if (strcmp(existingNode->member, member) == 0) {
            RedisModule_Free(intervalSet->hash->array[hashCode]->interval);
            intervalSet->hash->array[hashCode]->interval = interval;
            return 0;
        } else {
            reHash(intervalSet);
            return hashAdd(intervalSet, member, interval);
        }
    }
}

void reHash(IntervalSet *intervalSet) {
    HashTable *newHash = createHash(intervalSet->hash->capacity * 2);
    for (int i = 0; i < intervalSet->hash->capacity; i++) {
        IntervalTreeNode *node = intervalSet->hash->array[i];
        if (node != NULL) {
            int newHashCode = getHashCode(newHash->capacity, intervalSet->hash->array[i]->member);
            newHash->array[newHashCode] = node;
            newHash->len++;
            intervalSet->hash->array[i] = NULL;
            intervalSet->hash->len--;
        }
    }
    RedisModule_Free(intervalSet->hash);
    intervalSet->hash = newHash;
}