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
int isLeaf(Node *n);
Node *leftRotate(Node *node);
Node *rightRotate(Node *node);
void checkNode(Node *node);
void insertNode(double val, Node **root, char *member, Interval *interval);
void checkForCase2(Node *toDelete, int delete, int fromDirection, Node **root);
void deleteNode(double val, Node **root);
void checkBlack(Node *temp, int c);
void findContains(Node *node, double value, struct RedisModuleCtx *ctx, int *len, int *read);
void freeIntervalSetTreeNode(Node *node);

#endif