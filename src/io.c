#include "io.h"
#include "redismodule.h"
#include "struct.h"

void *IntervalSetTypeRdbLoad(RedisModuleIO *rdb, int encver) {
    return NULL;
}

void IntervalSetTypeRdbSave(RedisModuleIO *rdb, void *value) {

}

void IntervalSetTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {

}

void IntervalSetTypeFree(void *value) {
    freeIntervalSet(value);
}
