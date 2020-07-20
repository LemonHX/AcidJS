cmake_minimum_required(VERSION 3.1)
project(quickjs)


set(quickjs_version 2020-07-05)
set(quickjs_sources
        quickjs.h
        quickjs-libc.h
        quickjs.c
        libregexp.c
        libunicode.c
        libbf.c
        cutils.c
        quickjs-libc.c)

add_library(${PROJECT_NAME}
        ${quickjs_sources}
        )
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        CONFIG_VERSION="${quickjs_version}"
        )
