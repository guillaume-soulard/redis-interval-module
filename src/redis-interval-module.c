#include "redismodule.h"
#include "redis-commands.h"
#include "io.h"

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx, "interval", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    RedisModuleTypeMethods tm = {
            .version = REDISMODULE_TYPE_METHOD_VERSION,
            .rdb_load = IntervalSetTypeRdbLoad,
            .rdb_save = IntervalSetTypeRdbSave,
            .aof_rewrite = IntervalSetTypeAofRewrite,
            .mem_usage = IntervalSetTypeMemUsage,
            .free = IntervalSetTypeFree,
            .digest = IntervalSetTypeDigest
    };

    IntervalSetType = RedisModule_CreateDataType(ctx,"intertype",0,&tm);
    if (IntervalSetType == NULL) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "iadd", iAddCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "icard", iCardCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    return REDISMODULE_OK;
}
