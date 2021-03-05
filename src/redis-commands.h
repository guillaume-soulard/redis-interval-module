#ifndef REDIS_COMMAND_H
#define REDIS_COMMAND_H

#include "redismodule.h"

static RedisModuleType *IntervalSetType;

int iAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

#endif