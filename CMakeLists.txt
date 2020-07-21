cmake_minimum_required(VERSION 3.1)
project(quickjs)

set(quickjs_version 2020-07-05)
set(quickjs_sources
        src/quickjs.c
        src/utils/libregexp.c
        src/utils/libunicode.c
        src/utils/libbf.c
        src/utils/cutils.c
        src/libc/quickjs-libc.c
        )

add_library(${PROJECT_NAME}
        ${quickjs_sources}
        )

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        CONFIG_VERSION="${quickjs_version}"
        )

add_subdirectory(src/compiler)
add_subdirectory(src/interpreter)