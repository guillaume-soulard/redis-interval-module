cmake_minimum_required(VERSION 3.17)
project(redis_interval_module C)

set(CMAKE_C_STANDARD 99)

add_library(redis-interval-module SHARED src/struct.c src/redis-interval-module.c src/redis-commands.c)