#ifndef INTERVAL_H
#define INTERVAL_H

#include "redismodule.h"

#define INTERVAL_OK 0
#define INTERVAL_ERROR 1

typedef struct Interval {
    double lowerBound;
    double upperBound;
    int includeLowerBound;
    int includeUpperBound;
} Interval;

Interval *newInterval();
int parseInterval(RedisModuleString *intervalString, Interval *interval);

#endif