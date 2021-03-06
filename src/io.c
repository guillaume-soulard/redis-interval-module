#include "io.h"
#include "redismodule.h"

void *IntervalSetTypeRdbLoad(RedisModuleIO *rdb, int encver) {
    return NULL;
}

void IntervalSetTypeRdbSave(RedisModuleIO *rdb, void *value) {

}

void IntervalSetTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {

}

size_t IntervalSetTypeMemUsage(const void *value) {
    return 0;
}

void IntervalSetTypeFree(void *value) {

}

void IntervalSetTypeDigest(RedisModuleDigest *md, void *value) {

}
