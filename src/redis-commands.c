#include "redismodule.h"
#include "redis-commands.h"
#include "struct.h"

Interval *parseInterval(RedisModuleString *pString);

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
        add(intervalSet, argv[3], interval);
        return RedisModule_ReplyWithCString(ctx, "ok");
    }
}

Interval *parseInterval(RedisModuleString *intervalString) {
    Interval *interval = RedisModule_Alloc(sizeof (Interval));
    size_t len;
    const char *str = RedisModule_StringPtrLen(intervalString, &len);
    interval->includeLowerBound = 1;
    if (str != NULL && len > 0 && str[0] == ']') {
        interval->includeLowerBound = 0;
    }
    interval->includeUpperBound = 1;
    if (str != NULL && len > 0 && str[len - 1] == '[') {
        interval->includeUpperBound = 0;
    }
//    interval->lowerBound = 1;
//    interval->upperBound = 5;
    return interval;
}

