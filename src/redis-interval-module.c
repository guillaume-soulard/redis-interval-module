#include "redismodule.h"
#include "interval-set.h"
#include "io.h"

static RedisModuleType *IntervalSetType;

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
        RedisModule_ReplicateVerbatim(ctx);
        return RedisModule_ReplyWithLongLong(ctx, response);
    }
}

int iCardCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 2) {
        return RedisModule_WrongArity(ctx);
    }
    RedisModuleString *keyName = argv[1];
    RedisModuleKey *key = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != IntervalSetType) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet;
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            return RedisModule_ReplyWithLongLong(ctx, 0);
        } else {
            intervalSet = RedisModule_ModuleTypeGetValue(key);
            int len = intervalSet->hash->len;
            return RedisModule_ReplyWithLongLong(ctx, len);
        }
    }
}

int iContainsCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 3) {
        return RedisModule_WrongArity(ctx);
    }
    RedisModuleString *keyName = argv[1];
    RedisModuleKey *key = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != IntervalSetType) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet;
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            return RedisModule_ReplyWithEmptyArray(ctx);
        } else {
            intervalSet = RedisModule_ModuleTypeGetValue(key);
            double valueToSearch = 0;
            if (RedisModule_StringToDouble(argv[2], &valueToSearch) == REDISMODULE_ERR) {
                return RedisModule_ReplyWithError(ctx, "incorrect value");
            }
            searchValue(ctx, intervalSet, valueToSearch);
            return REDISMODULE_OK;
        }
    }
}

int iOverlapsCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 3) {
        return RedisModule_WrongArity(ctx);
    }
    RedisModuleString *keyName = argv[1];
    RedisModuleKey *key = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != IntervalSetType) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet;
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            return RedisModule_ReplyWithEmptyArray(ctx);
        } else {
            intervalSet = RedisModule_ModuleTypeGetValue(key);
            Interval *intervalToSearch = parseInterval(argv[2]);
            searchInterval(ctx, intervalSet, intervalToSearch);
            return REDISMODULE_OK;
        }
    }
}

int iRemCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);
    if (argc != 3) {
        return RedisModule_WrongArity(ctx);
    }
    RedisModuleString *keyName = argv[1];
    RedisModuleKey *key = RedisModule_OpenKey(ctx, keyName, REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != IntervalSetType) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    } else {
        IntervalSet *intervalSet;
        if (type == REDISMODULE_KEYTYPE_EMPTY) {
            return RedisModule_ReplyWithLongLong(ctx, 0);
        } else {
            intervalSet = RedisModule_ModuleTypeGetValue(key);
            char *member = RedisModule_Strdup(RedisModule_StringPtrLen(argv[2], NULL));
            return RedisModule_ReplyWithLongLong(ctx, removeInterval(intervalSet, member));
        }
    }
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx, "interval", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    RedisModuleTypeMethods tm = {
            .version = REDISMODULE_TYPE_METHOD_VERSION,
            .rdb_load = IntervalSetTypeRdbLoad,
            .rdb_save = IntervalSetTypeRdbSave,
            .aof_rewrite = IntervalSetTypeAofRewrite,
            .free = IntervalSetTypeFree
    };

    IntervalSetType = RedisModule_CreateDataType(ctx,"intertype",0,&tm);
    if (IntervalSetType == NULL) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "iadd", iAddCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "irem", iRemCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "icard", iCardCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "icontains", iContainsCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "ioverlaps", iOverlapsCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    return REDISMODULE_OK;
}