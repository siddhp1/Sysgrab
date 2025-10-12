cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CPM_DOWNLOAD_VERSION 0.40.8)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME libyaml
    GITHUB_REPOSITORY yaml/libyaml
    GIT_TAG 0.2.5
    VERSION 0.2.5
    OPTIONS
        "BUILD_TESTING OFF"
        "BUILD_SHARED_LIBS OFF"
)

project(
    sysgrab 
    VERSION 3.0.0
    DESCRIPTION "Sysgrab"
    LANGUAGES C
)

add_subdirectory(src)
