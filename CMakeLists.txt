cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(
    sysgrab 
    VERSION 3.0.0
    DESCRIPTION "Sysgrab"
    LANGUAGES C
)

add_subdirectory(third_party)
add_subdirectory(src)
