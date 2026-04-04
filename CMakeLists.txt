cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  "Sysgrab"
  VERSION 3.0.0
  DESCRIPTION "Customizable system information fetching tool for Unix-like systems"
  LANGUAGES CXX)

add_subdirectory(src)
