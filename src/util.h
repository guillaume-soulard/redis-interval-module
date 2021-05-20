#include "redismodule.h"

#ifndef UTIL_H
#define UTIL_H

int stringMatchLen(const char *pattern, int patternLen,
                   const char *string, int stringLen, int nocase);
int stringToDouble(const char *str, double *value);
int redisModuleStringToDouble(RedisModuleString *str, double *value);

#endif
