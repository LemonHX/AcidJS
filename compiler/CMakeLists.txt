cmake_minimum_required(VERSION 3.1)
project(quickjs_compiler)
set(quickjs_version 2020-07-05)

add_executable(${PROJECT_NAME} qjsc.c)
target_link_libraries(quickjs_compiler quickjs)
target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        CONFIG_VERSION="${quickjs_version}"
        )