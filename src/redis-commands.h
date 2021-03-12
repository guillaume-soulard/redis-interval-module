#ifndef REDIS_COMMAND_H
#define REDIS_COMMAND_H

#include "redismodule.h"

static RedisModuleType *IntervalSetType;

int iAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int iCardCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
int iContainsCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

#endif