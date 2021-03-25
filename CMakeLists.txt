cmake_minimum_required(VERSION 3.17)
project(redis_interval_module C)

set(CMAKE_C_STANDARD 99)

add_library(
        redis-interval-module
        SHARED
        src/interval-set.h
        src/interval-set.c
        src/redis-interval-module.c
        src/interval.h
        src/interval.c
        src/io.h
        src/io.c
        src/interval-red-black-tree.c src/hashmap.c)
