#include "struct.h"
#include "float.h"

void treeAdd(IntervalTree *tree, RedisModuleString *member, Interval *interval);
IntervalTreeNode *createTreeNode(Interval *interval, RedisModuleString *member);
double treeNodeAdd(IntervalTreeNode *node, RedisModuleString *member, Interval *interval);
void hashAdd(HashTable *hash, RedisModuleString *member, Interval *interval);

IntervalSet *createIntervalSet() {
    IntervalSet *intervalSet = RedisModule_Alloc(sizeof(IntervalSet));
    HashTable *table = RedisModule_Alloc(sizeof(struct HashTable));
    table->primaryArrayCapacity = 10;
    table->primaryArray = RedisModule_Alloc(table->primaryArrayCapacity * sizeof(IntervalTreeNode));
    intervalSet->hash = table;
    intervalSet->tree = RedisModule_Alloc(sizeof(struct IntervalTree));
    intervalSet->tree->head = NULL;
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

int add(IntervalSet *intervalSet, RedisModuleString *member, Interval *interval) {
    hashAdd(intervalSet->hash, member, interval);
    treeAdd(intervalSet->tree, member, interval);
    return 1;
}

void treeAdd(IntervalTree *tree, RedisModuleString *member, Interval *interval) {
    if (tree->head == NULL) {
        tree->head = createTreeNode(interval, member);
    } else {
        treeNodeAdd(tree->head, member, interval);
    }
}

IntervalTreeNode *createTreeNode(Interval *interval, RedisModuleString *member) {
    IntervalTreeNode *node = RedisModule_Alloc(sizeof (IntervalTreeNode));
    node->interval = interval;
    node->member = member;
    node->maxUpperBound = interval->upperBound;
    return node;
}

double treeNodeAdd(IntervalTreeNode *node, RedisModuleString *member, Interval *interval) {
    double maxBound = DBL_MIN;
    if (node->interval->lowerBound < interval->lowerBound) {
        if (node->left == NULL) {
            node->left = createTreeNode(interval, member);
            maxBound = node->left->maxUpperBound;
        } else {
            maxBound = treeNodeAdd(node->left, member, interval);
        }
    } else {
        if (node->right == NULL) {
            node->right = createTreeNode(interval, member);
            maxBound = node->right->maxUpperBound;
        } else {
            maxBound = treeNodeAdd(node->right, member, interval);
        }
    }
    if (node->maxUpperBound > maxBound) {
        maxBound = node->maxUpperBound;
    }
    return maxBound;
}

void hashAdd(HashTable *hash, RedisModuleString *member, Interval *interval) {
    int hashCode = getHashCode(hash->primaryArrayCapacity, member);
    if (hash->primaryArray[hashCode] == NULL) {
        IntervalTreeNode *node = RedisModule_Alloc(sizeof(struct IntervalTreeNode));
        node->interval = interval;
        hash->primaryArray[hashCode] = node;
    } else {
        RedisModule_Free(hash->primaryArray[hashCode]->interval);
        hash->primaryArray[hashCode]->interval = interval;
    }
}

