cmake_minimum_required(VERSION 3.1)
project(quickjs_interpreter)
set(quickjs_version 2020-07-05)

add_executable(${PROJECT_NAME} qjs.c)
target_link_libraries(quickjs_interpreter quickjs)
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        CONFIG_VERSION="${quickjs_version}"
        )