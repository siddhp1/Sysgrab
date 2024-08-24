# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(sysgrab VERSION 1.0)

# Force the use of GCC
set(CMAKE_C_COMPILER gcc)

# Set the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
set(SOURCES src/sysgrab.c src/data.c)

# Specify the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add the executable
add_executable(sysgrab ${SOURCES})

# Install the executable and resource files
install(TARGETS sysgrab DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(FILES resources/art.txt resources/config.txt DESTINATION ${CMAKE_SOURCE_DIR}/bin)
