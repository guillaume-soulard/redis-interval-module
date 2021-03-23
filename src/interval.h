#ifndef INTERVAL_H
#define INTERVAL_H

#include "redismodule.h"

typedef struct Interval {
    double lowerBound;
    double upperBound;
    int includeLowerBound;
    int includeUpperBound;
} Interval;

Interval *createInterval(int includeLowerBound, double lowerBound, double upperBound, int includeUpperBound);
Interval *parseInterval(RedisModuleString *intervalString);
int containsValue(Interval *interval, double value, int includeValue);
int overlaps(Interval *interval1, Interval *interval2);
void outputInterval(RedisModuleCtx *ctx, char *member, Interval *interval);

#endif