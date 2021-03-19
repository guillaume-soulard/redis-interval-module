#include <stdio.h>
#include <stdlib.h>
#include "interval.h"
#include <float.h>

struct AVLnode {
    double key;
    Interval *interval;
    double maxUpper;
    char *member;
    struct AVLnode *left;
    struct AVLnode *right;
    double height;
};
typedef struct AVLnode avlNode;

double max(double a, double b) {
    return (a > b) ? a : b;
}

avlNode *newNode(double key, char *member, Interval *interval) {
    avlNode *node = RedisModule_Alloc(sizeof(avlNode));
    node->key = key;
    node->interval = interval;
    node->member = member;
    node->maxUpper = interval->upperBound;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    return node;
}

double maxUpperBound(avlNode *node) {
    double rightMaxUpperBound = DBL_MIN;
    double leftMaxUpperBound = DBL_MIN;
    if (node->right != NULL) {
        rightMaxUpperBound = maxUpperBound(node->right);
    }
    if (node->left != NULL) {
        leftMaxUpperBound = maxUpperBound(node->left);
    }
    double maxUpper = max(node->interval->upperBound, rightMaxUpperBound);
    maxUpper = max(maxUpper, leftMaxUpperBound);
    return maxUpper;
}

double nodeHeight(avlNode *node) {
    if (node == NULL)
        return -1;
    else
        return (node->height);
}

double heightDiff(avlNode *node) {
    if (node == NULL)
        return 0;
    else
        return (nodeHeight(node->left) - nodeHeight(node->right));
}

avlNode *minNode(avlNode *node) {
    avlNode *temp = node;
    while (temp->left != NULL) temp = temp->left;
    return temp;
}

avlNode *rightRotate(avlNode *z) {
    avlNode *y = z->left;
    avlNode *T3 = y->right;
    y->right = z;
    z->left = T3;
    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);
    z->maxUpper = maxUpperBound(z);
    y->maxUpper = maxUpperBound(y);
    return y;
}

avlNode *leftRotate(avlNode *z) {
    avlNode *y = z->right;
    avlNode *T3 = y->left;
    y->left = z;
    z->right = T3;
    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);
    z->maxUpper = maxUpperBound(z);
    y->maxUpper = maxUpperBound(y);
    return y;
}

avlNode *LeftRightRotate(avlNode *z) {
    z->left = leftRotate(z->left);
    return (rightRotate(z));
}

avlNode *RightLeftRotate(avlNode *z) {
    z->right = rightRotate(z->right);
    return (leftRotate(z));
}

avlNode *insert(avlNode *node, double key, char *member, Interval *interval) {
    if (node == NULL)
        return newNode(key, member, interval);

    if (key <= node->key)
        node->left = insert(node->left, key, member, interval);
    else if (key > node->key)
        node->right = insert(node->right, key, member, interval);

    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);
    node->maxUpper = maxUpperBound(node);
    double balance = heightDiff(node);

    if (balance > 1 && key < (node->left->key)) {
        return rightRotate(node);
    }

    if (balance < -1 && key > (node->right->key)) {
        return leftRotate(node);
    }

    if (balance > 1 && key > (node->left->key)) {
        node = LeftRightRotate(node);
    }
    if (balance < -1 && key < (node->right->key)) {
        node = RightLeftRotate(node);
    }
    return node;
}

avlNode *delete(avlNode *node, double queryNum) {
    if (node == NULL)
        return node;

    if (queryNum <= node->key)
        node->left = delete(node->left, queryNum);
    else if (queryNum > node->key)
        node->right = delete(node->right, queryNum);
    else {
        if ((node->left == NULL) || (node->right == NULL)) {
            avlNode *temp = node->left ? node->left : node->right;

            /* No Child*/
            if (temp == NULL) {
                temp = node;
                node = NULL;
            } else
                *node = *temp;

            free(temp);
        } else {
            avlNode *temp = minNode(node->right);
            node->key = temp->key;
            node->right = delete(node->right, temp->key);
        }
    }

    if (node == NULL)
        return node;


    node->maxUpper = maxUpperBound(node);
    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);

    double balance = heightDiff(node);

    if ((balance > 1) && (heightDiff(node->left) >= 0))
        return rightRotate(node);

    if ((balance > 1) && (heightDiff(node->left) < 0)) {
        node = LeftRightRotate(node);
    }

    if ((balance < -1) && (heightDiff(node->right) >= 0))
        return leftRotate(node);

    if ((balance < -1) && (heightDiff(node->right) < 0)) {
        node = RightLeftRotate(node);
    }

    return node;
}

void findContains(avlNode *node, double value, struct RedisModuleCtx *ctx, int *len) {
    if (node != NULL) {
        if (containsValue(node->interval, value)) {
            outputInterval(ctx, node->member, node->interval);
            (*len)++;
        }
        if (value >= node->key && value <= node->maxUpper) {
            findContains(node->left, value, ctx, len);
        }
        else if (value < node->key && value <= node->maxUpper) {
            findContains(node->right, value, ctx, len);
        }
    }
}
