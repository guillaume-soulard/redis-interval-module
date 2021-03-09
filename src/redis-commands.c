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
        RedisModule_CloseKey(key);
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet;
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            intervalSet = createIntervalSet();
            RedisModule_ModuleTypeSetValue(key, IntervalSetType, intervalSet);
        } else {
            intervalSet = RedisModule_ModuleTypeGetValue(key);
        }
        Interval *interval = parseInterval(argv[2]);
        if (interval == NULL) {
            return RedisModule_ReplyWithError(ctx, "incorrect interval");
        }
        char *member = RedisModule_Strdup(RedisModule_StringPtrLen(argv[3], NULL));
        int response = add(intervalSet, member, interval);
        RedisModule_CloseKey(key);
        return RedisModule_ReplyWithLongLong(ctx, response);
    }
}

