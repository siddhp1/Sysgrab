add_library(parson STATIC parson.c parson.h)

target_include_directories(parson PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                         $<INSTALL_INTERFACE:include/parson>)
