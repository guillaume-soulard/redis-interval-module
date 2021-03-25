#ifndef INTERVAL_RED_BLACK_TREE_H
#define INTERVAL_RED_BLACK_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "interval.h"

typedef struct node {
    double val;
    Interval *interval;
    double maxUpper;
    double minLower;
    char *member;
    struct node *par;
    struct node *left;
    struct node *right;
    double height;
    int color;
} Node;

Node *newNode(double val, Node *par, char *member, Interval *interval);
Node *leftRotate(Node *node);
Node *rightRotate(Node *node);
void checkNode(Node *node);
Node *insertNode(double val, Node **root, char *member, Interval *interval);
void checkForCase2(Node *toDelete, int delete, int fromDirection, Node **root);
void deleteNode(Node **root, Node *toDelete);
void checkBlack(Node *temp, int c);
void findContains(Node *node, double value, struct RedisModuleCtx *ctx, int *len);
void findOverlaps(Node *node, Interval *intervalToSearch, struct RedisModuleCtx *ctx, int *len);
void freeIntervalSetTreeNode(Node *node);

#endif