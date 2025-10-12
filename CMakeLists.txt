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
set(SOURCES src/main.c src/sysgrab_art.c src/sysgrab_config.c src/sysgrab_data.c src/sysgrab_log.c src/sysgrab_number.c src/sysgrab_path.c src/sysgrab_print.c src/sysgrab_string.c src/sysgrab_time.c)

# Specify the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add the executable
add_executable(sysgrab ${SOURCES})

# Link the yaml library
target_link_libraries(sysgrab yaml)

# Copy resource files to the bin directory
add_custom_command(TARGET sysgrab POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/resources/art.txt
        ${CMAKE_SOURCE_DIR}/resources/config.yaml
        $<TARGET_FILE_DIR:sysgrab>
)

# Install the executable and resource files
install(TARGETS sysgrab DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(FILES resources/art.txt resources/config.yaml DESTINATION ${CMAKE_SOURCE_DIR}/bin)
