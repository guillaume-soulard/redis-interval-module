#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "interval.h"

struct AVLnode {
    double key;
    Interval *interval;
    double maxUpper;
    struct AVLnode *left;
    struct AVLnode *right;
    double height;
};
typedef struct AVLnode avlNode;

avlNode *insert(avlNode *node, double key, char *member, Interval *interval);
avlNode *delete(avlNode *node, double queryNum);
void findContains(avlNode *node, double value, struct RedisModuleCtx *ctx, int *len);

#endif