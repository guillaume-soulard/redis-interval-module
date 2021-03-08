#include "redismodule.h"
#include "redis-commands.h"
#include "struct.h"
#include "interval.h"

int iAddCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 4) {
        return RedisModule_WrongArity(ctx);
    }
    RedisModuleString *keyName = argv[1];
    RedisModuleKey *key = RedisModule_OpenKey(ctx, keyName, REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != IntervalSetType) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet = RedisModule_ModuleTypeGetValue(key);
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            intervalSet = createIntervalSet();
            RedisModule_ModuleTypeSetValue(key, IntervalSetType, intervalSet);
        }
        Interval *interval = parseInterval(argv[2]);
        if (interval == NULL) {
            return RedisModule_ReplyWithError(ctx, "incorrect interval");
        }
        int response = add(intervalSet, argv[3], interval);
        return RedisModule_ReplyWithLongLong(ctx, response);
    }
}

