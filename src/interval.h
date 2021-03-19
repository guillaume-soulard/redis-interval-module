#ifndef INTERVAL_H
#define INTERVAL_H

#include "redismodule.h"
#include <stdbool.h>

typedef struct Interval {
    double lowerBound;
    double upperBound;
    int includeLowerBound;
    int includeUpperBound;
} Interval;

Interval *createInterval(int includeLowerBound, double lowerBound, double upperBound, int includeUpperBound);
Interval *parseInterval(RedisModuleString *intervalString);
bool containsValue(Interval *interval, double value);
void outputInterval(RedisModuleCtx *ctx, char *member, Interval *interval);

#endif