#include "redismodule.h"
#include "redis-commands.h"
#include "struct.h"

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
        Interval *interval = RedisModule_Alloc(sizeof (Interval));
        interval->lowerBound = 1;
        interval->includeLowerBound = 1;
        interval->upperBound = 5;
        interval->includeUpperBound = 1;
        add(intervalSet, RedisModule_CreateString(ctx, "test", 4), interval);

        return RedisModule_ReplyWithCString(ctx, "ok");
    }
}

