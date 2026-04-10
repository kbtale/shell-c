cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell)

set(CMAKE_C_STANDARD 23)

include_directories(include)

file(GLOB_RECURSE SOURCE_FILES src/*.c)

add_executable(shell ${SOURCE_FILES})

target_link_libraries(shell PRIVATE readline)
