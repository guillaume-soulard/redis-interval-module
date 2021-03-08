#include "struct.h"
#include "float.h"

void treeAdd(IntervalSet *intervalSet, IntervalTreeNode *node);
IntervalTreeNode *createTreeNode(Interval *interval, RedisModuleString *member);
double treeNodeAdd(IntervalTreeNode *node, IntervalTreeNode *newNode);
void hashAdd(IntervalSet *intervalSet, RedisModuleString *member, Interval *interval);

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
    hashAdd(intervalSet, member, interval);
    return 1;
}

void treeAdd(IntervalSet *intervalSet, IntervalTreeNode *node) {
    if (intervalSet->tree->head == NULL) {
        intervalSet->tree->head = node;
    } else {
        treeNodeAdd(intervalSet->tree->head, node);
    }
}

IntervalTreeNode *createTreeNode(Interval *interval, RedisModuleString *member) {
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

void hashAdd(IntervalSet *intervalSet, RedisModuleString *member, Interval *interval) {
    int hashCode = getHashCode(intervalSet->hash->primaryArrayCapacity, member);
    IntervalTreeNode *existingNode = intervalSet->hash->primaryArray[hashCode];
    if (existingNode == NULL) {
        IntervalTreeNode *node = createTreeNode(interval, member);
        node->interval = interval;
        intervalSet->hash->primaryArray[hashCode] = node;
        treeAdd(intervalSet, node);
    } else {
        if (RedisModule_StringCompare(existingNode->member, member) == 0) {
            RedisModule_Free(intervalSet->hash->primaryArray[hashCode]->interval);
            intervalSet->hash->primaryArray[hashCode]->interval = interval;
        } else {
            // TODO redim hash
        }
    }
}

