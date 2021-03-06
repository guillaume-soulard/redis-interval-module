#include "redismodule.h"
#include "stdlib.h"
#include "interval.h"

Interval *newInterval() {
    return RedisModule_Alloc(sizeof (Interval));
}

int parseInterval(RedisModuleString *intervalString, Interval *interval) {
    size_t len;
    const char *str = RedisModule_StringPtrLen(intervalString, &len);
    char bounds[2][40] = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int i;
    int partIndex = 0;
    int partIterator = 0;
    int missingBound[2] = { 1, 1 };

    interval->includeLowerBound = 1;
    interval->includeUpperBound = 1;
    for (i = 0; i < len; i++){
        if (i == 0 && str[i] == ']') {
            interval->includeLowerBound = 0;
        } else if (i == len - 1 && str[i] == '[') {
            interval->includeUpperBound = 0;
        } else if (str[i] == ',') {
            partIndex = 1;
            partIterator = 0;
        } else if (str[i] != '[' && str[i] != ']' && partIterator < 40) {
            bounds[partIndex][partIterator] = str[i];
            missingBound[partIndex] = 0;
            partIterator++;
        }
    }
    if (missingBound[0] || missingBound[1]) {
        return INTERVAL_ERROR;
    }
    interval->lowerBound = strtod(bounds[0], NULL);
    interval->upperBound = strtod(bounds[1], NULL);
    if (interval->lowerBound > interval->upperBound) {
        return INTERVAL_ERROR;
    }
    return INTERVAL_OK;
}