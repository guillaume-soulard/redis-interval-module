#include "redismodule.h"
#include "stdlib.h"
#include "interval.h"
#include "string.h"

#define doubleBufferSize 512

Interval *createInterval(int includeLowerBound, double lowerBound, double upperBound, int includeUpperBound) {
    Interval *interval = RedisModule_Alloc(sizeof (Interval));
    interval->includeLowerBound = includeLowerBound;
    interval->includeUpperBound = includeUpperBound;
    interval->lowerBound = lowerBound;
    interval->upperBound = upperBound;
    return interval;
}

Interval *parseInterval(RedisModuleString *intervalString) {
    size_t len;
    const char *str = RedisModule_StringPtrLen(intervalString, &len);
    char bounds[2][doubleBufferSize];
    int i;
    int partIndex = 0;
    int partIterator = 0;
    int missingBound[2] = { 1, 1 };

    int includeLowerBound = 1;
    int includeUpperBound = 1;
    double lowerBound;
    double upperBound;

    for (i = 0; i < len; i++){
        if (i == 0 && str[i] == ']') {
            includeLowerBound = 0;
        } else if (i == len - 1 && str[i] == '[') {
            includeUpperBound = 0;
        } else if (str[i] == ',') {
            partIndex = 1;
            partIterator = 0;
        } else if (str[i] != '[' && str[i] != ']' && partIterator < doubleBufferSize) {
            bounds[partIndex][partIterator] = str[i];
            missingBound[partIndex] = 0;
            partIterator++;
        }
    }
    if (missingBound[0] || missingBound[1]) {
        return NULL;
    }
    lowerBound = strtod(bounds[0], NULL);
    upperBound = strtod(bounds[1], NULL);
    if (lowerBound > upperBound) {
        return NULL;
    }
    return createInterval(includeLowerBound, lowerBound, upperBound, includeUpperBound);
}

bool containsValue(Interval *interval, double value) {
    int lowerBoundMatch = 0;
    int upperBoundMatch = 0;
    if ((interval->includeLowerBound == 1 && interval->lowerBound <= value) || (interval->includeLowerBound == 0 && interval->lowerBound < value)) {
        lowerBoundMatch = 1;
    }
    if ((interval->includeUpperBound == 1 && interval->upperBound >= value) || (interval->includeUpperBound == 0 && interval->upperBound > value)) {
        upperBoundMatch = 1;
    }
    if (lowerBoundMatch == 1 && upperBoundMatch == 1) {
        return 1;
    } else {
        return 0;
    }
}

void outputInterval(RedisModuleCtx *ctx, char *member, Interval *interval) {
    RedisModule_ReplyWithArray(ctx,REDISMODULE_POSTPONED_ARRAY_LEN);
    RedisModule_ReplyWithCString(ctx, member);
    RedisModule_ReplyWithDouble(ctx, interval->includeLowerBound);
    RedisModule_ReplyWithDouble(ctx, interval->lowerBound);
    RedisModule_ReplyWithDouble(ctx, interval->includeUpperBound);
    RedisModule_ReplyWithDouble(ctx, interval->upperBound);
    RedisModule_ReplySetArrayLength(ctx, 5);
}