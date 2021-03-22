//#include <stdio.h>
//#include "interval.h"
//#include <float.h>
//#include "intervalavltree.h"
//
//double max(double a, double b) {
//    return (a > b) ? a : b;
//}
//
//double min(double a, double b) {
//    return (a < b) ? a : b;
//}
//
//avlNode *newNode(double key, char *member, Interval *interval) {
//    avlNode *node = RedisModule_Alloc(sizeof(avlNode));
//    node->key = key;
//    node->interval = interval;
//    node->member = member;
//    node->maxUpper = interval->upperBound;
//    node->minLower = interval->lowerBound;
//    node->left = NULL;
//    node->right = NULL;
//    node->height = 0;
//    return node;
//}
//
//double maxUpperBound(avlNode *node) {
//    double rightMaxUpperBound = DBL_MIN;
//    double leftMaxUpperBound = DBL_MIN;
//    if (node->right != NULL) {
//        rightMaxUpperBound = maxUpperBound(node->right);
//    }
//    if (node->left != NULL) {
//        leftMaxUpperBound = maxUpperBound(node->left);
//    }
//    double maxUpper = max(node->interval->upperBound, rightMaxUpperBound);
//    maxUpper = max(maxUpper, leftMaxUpperBound);
//    return maxUpper;
//}
//
//double minLowerBound(avlNode *node) {
//    double rightMinLowerBound = DBL_MAX;
//    double leftMinLowerBound = DBL_MAX;
//    if (node->right != NULL) {
//        rightMinLowerBound = minLowerBound(node->right);
//    }
//    if (node->left != NULL) {
//        leftMinLowerBound = minLowerBound(node->left);
//    }
//    double minLower = min(node->interval->lowerBound, rightMinLowerBound);
//    minLower = min(minLower, leftMinLowerBound);
//    return minLower;
//}
//
//double nodeHeight(avlNode *node) {
//    if (node == NULL)
//        return -1;
//    else
//        return (node->height);
//}
//
//double heightDiff(avlNode *node) {
//    if (node == NULL)
//        return 0;
//    else
//        return (nodeHeight(node->left) - nodeHeight(node->right));
//}
//
//avlNode *minNode(avlNode *node) {
//    avlNode *temp = node;
//    while (temp->left != NULL) temp = temp->left;
//    return temp;
//}
//
//avlNode *rightRotate(avlNode *z) {
//    avlNode *y = z->left;
//    avlNode *T3 = y->right;
//    y->right = z;
//    z->left = T3;
//
////    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
////    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);
////    z->maxUpper = maxUpperBound(z);
////    y->maxUpper = maxUpperBound(y);
////    z->minLower = minLowerBound(z);
////    y->minLower = minLowerBound(y);
//    return y;
//}
//
//avlNode *leftRotate(avlNode *z) {
//    avlNode *y = z->right;
//    avlNode *T3 = y->left;
//    y->left = z;
//    z->right = T3;
//    z->height -= 2;
////    z->height = (max(nodeHeight(z->left), nodeHeight(z->right)) + 1);
////    y->height = (max(nodeHeight(y->left), nodeHeight(y->right)) + 1);
////    z->maxUpper = maxUpperBound(z);
////    y->maxUpper = maxUpperBound(y);
////    z->minLower = minLowerBound(z);
////    y->minLower = minLowerBound(y);
//    return y;
//}
//
//avlNode *LeftRightRotate(avlNode *z) {
//    z->left = leftRotate(z->left);
//    return (rightRotate(z));
//}
//
//avlNode *RightLeftRotate(avlNode *z) {
//    z->right = rightRotate(z->right);
//    return (leftRotate(z));
//}
//
//avlNode *insert(avlNode *node, double key, char *member, Interval *interval) {
//    if (node == NULL) {
//        avlNode *new = newNode(key, member, interval);
//        return new;
//    }
//
//    if (key <= node->key)
//        node->left = insert(node->left, key, member, interval);
//    else if (key > node->key)
//        node->right = insert(node->right, key, member, interval);
//
//    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);
//    node->maxUpper = maxUpperBound(node);
//    node->minLower = minLowerBound(node);
//    double balance = heightDiff(node);
//
//    if (balance > 1 && key < (node->left->key)) {
//        return rightRotate(node);
//    }
//
//    if (balance < -1 && key > (node->right->key)) {
//        return leftRotate(node);
//    }
//
//    if (balance > 1 && key > (node->left->key)) {
//        node = LeftRightRotate(node);
//    }
//    if (balance < -1 && key < (node->right->key)) {
//        node = RightLeftRotate(node);
//    }
//    return node;
//}
//
//avlNode *delete(avlNode *node, double queryNum) {
//    if (node == NULL)
//        return node;
//
//    if (queryNum <= node->key)
//        node->left = delete(node->left, queryNum);
//    else if (queryNum > node->key)
//        node->right = delete(node->right, queryNum);
//    else {
//        if ((node->left == NULL) || (node->right == NULL)) {
//            avlNode *temp = node->left ? node->left : node->right;
//            if (temp == NULL) {
//                temp = node;
//                node = NULL;
//            } else
//                *node = *temp;
//
//            freeIntervalSetTreeNode(temp);
//        } else {
//            avlNode *temp = minNode(node->right);
//            node->key = temp->key;
//            node->right = delete(node->right, temp->key);
//        }
//    }
//
//    if (node == NULL)
//        return node;
//
//    node->maxUpper = maxUpperBound(node);
//    node->minLower = minLowerBound(node);
//    node->height = (max(nodeHeight(node->left), nodeHeight(node->right)) + 1);
//
//    double balance = heightDiff(node);
//
//    if ((balance > 1) && (heightDiff(node->left) >= 0))
//        return rightRotate(node);
//
//    if ((balance > 1) && (heightDiff(node->left) < 0)) {
//        node = LeftRightRotate(node);
//    }
//
//    if ((balance < -1) && (heightDiff(node->right) >= 0))
//        return leftRotate(node);
//
//    if ((balance < -1) && (heightDiff(node->right) < 0)) {
//        node = RightLeftRotate(node);
//    }
//
//    return node;
//}
//
//void freeIntervalSetTreeNode(avlNode *node) {
//    if (node != NULL) {
//        if (node->interval != NULL) {
//            RedisModule_Free(node->interval);
//        }
//        freeIntervalSetTreeNode(node->left);
//        freeIntervalSetTreeNode(node->right);
//        RedisModule_Free(node);
//    }
//}
//
//void findContains(avlNode *node, double value, struct RedisModuleCtx *ctx, int *len, int *read) {
//    if (node != NULL) {
//        (*read)++;
//        if (containsValue(node->interval, value)) {
//            outputInterval(ctx, node->member, node->interval);
//            (*len)++;
//        }
//        if (node->left != NULL && node->left->minLower <= value && value <= node->left->maxUpper) {
//            findContains(node->left, value, ctx, len, read);
//        }
//        if (node->right != NULL && node->right->minLower <= value && value <= node->right->maxUpper) {
//            findContains(node->right, value, ctx, len, read);
//        }
//    }
//}
