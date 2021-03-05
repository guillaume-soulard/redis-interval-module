#ifndef STRUCT_H
#define STRUCT_H

#include "redismodule.h"

typedef struct IntervalTree {
    struct IntervalTreeNode *head;
} IntervalTree;

typedef struct Interval {
    double lowerBound;
    double upperBound;
    int includeLowerBound;
    int includeUpperBound;
    double maxValueInSubTree;
} Interval;

typedef struct IntervalTreeNode {
    Interval* interval;
    struct IntervalTreeNode *left;
    struct IntervalTreeNode *right;
} IntervalTreeNode;

typedef struct HashTable {
    int primaryArrayCapacity;
    int secondaryArrayCapacity;
    IntervalTreeNode **primaryArray;
    IntervalTreeNode **secondaryArray;
} HashTable;

typedef struct IntervalSet {
    IntervalTree *tree;
    HashTable *hash;
} IntervalSet;


IntervalSet *createIntervalSet();
int add(IntervalSet *intervalSet, RedisModuleString *value, Interval *interval);

#endif