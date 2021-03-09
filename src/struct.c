#include "struct.h"
#include "float.h"
#include "string.h"

void treeAdd(IntervalSet *intervalSet, IntervalTreeNode *node);
IntervalTreeNode *createTreeNode(Interval *interval, char *member);
double treeNodeAdd(IntervalTreeNode *node, IntervalTreeNode *newNode);
int hashAdd(IntervalSet *intervalSet, char *member, Interval *interval);
void incrementalReHashIfNeeded(HashTable *hash);
TableNode *createTableNode(HashTable *hashTable, IntervalTreeNode *node);
Table *createTable(size_t capacity);

IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashTable *hash = RedisModule_Alloc(sizeof(struct HashTable));
    hash->arrayIndex = 0;
    hash->tables = RedisModule_Alloc(2 * sizeof (Table));
    hash->nextRehashIndex = 0;
    hash->tables[0] = createTable(10);
    intervalSet->hash = hash;
    intervalSet->tree = RedisModule_Alloc(sizeof(struct IntervalTree));
    intervalSet->tree->head = NULL;
    return intervalSet;
}

Table *createTable(size_t capacity) {
    Table *table = RedisModule_Alloc(sizeof (Table));
    table->capacity = capacity;
    table->array = RedisModule_Alloc(capacity * sizeof(IntervalTreeNode));
    return table;
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
    incrementalReHashIfNeeded(intervalSet->hash);
    int hashCode = getHashCode(intervalSet->hash->tables[intervalSet->hash->arrayIndex]->capacity, member);
    TableNode *existingNode = intervalSet->hash->tables[intervalSet->hash->arrayIndex]->array[hashCode];
    if (existingNode == NULL) {
        IntervalTreeNode *node = createTreeNode(interval, member);
        TableNode *tableNode = createTableNode(intervalSet->hash, node);
        intervalSet->hash->tables[intervalSet->hash->arrayIndex]->array[hashCode] = tableNode;
        intervalSet->hash->tables[intervalSet->hash->arrayIndex]->len++;
        treeAdd(intervalSet, node);
        return 1;
    } else {
        if (strcmp(existingNode->node->member, member) == 0) {
            RedisModule_Free(intervalSet->hash->tables[intervalSet->hash->arrayIndex]->array[hashCode]->node->interval);
            intervalSet->hash->tables[intervalSet->hash->arrayIndex]->array[hashCode]->node->interval = interval;
            return 0;
        } else {
            intervalSet->hash->arrayIndex = 1;
            return hashAdd(intervalSet, member, interval);
        }
    }
}

TableNode *createTableNode(HashTable *hashTable, IntervalTreeNode *node) {
    TableNode *result = RedisModule_Alloc(sizeof (TableNode));
    result->next = hashTable->first;
    result->node = node;
    return result;
}

void incrementalReHashIfNeeded(struct HashTable *hash) {
    if (hash->arrayIndex == 1) {
        hash->tables[1] = createTable(hash->tables[0]->capacity * 2);
        int i = 0;
        for (; hash->nextRehashIndex < hash->tables[0]->capacity && i <= 1000; hash->nextRehashIndex++) {
            if (hash->tables[0]->array[hash->nextRehashIndex] != NULL) {
                int hashCode = getHashCode(hash->tables[0]->capacity, hash->tables[0]->array[hash->nextRehashIndex]->node->member);
                hash->tables[1]->array[hashCode] = hash->tables[0]->array[hash->nextRehashIndex];
                hash->tables[1]->len++;
                hash->tables[0]->array[hash->nextRehashIndex] = NULL;
                hash->tables[0]->len--;
            }
            if (hash->tables[0]->len == 0) {
                hash->arrayIndex = 0;
                hash->tables[0] = hash->tables[1];
                RedisModule_Free(hash->tables[1]);
                break;
            }
            i++;
        }
    }
}