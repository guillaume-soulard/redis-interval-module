#ifndef INTERVAL_H
#define INTERVAL_H

#include "redismodule.h"

typedef struct Interval {
    double lowerBound;
    double upperBound;
    int includeLowerBound;
    int includeUpperBound;
} Interval;

Interval *parseInterval(RedisModuleString *intervalString);

#endif