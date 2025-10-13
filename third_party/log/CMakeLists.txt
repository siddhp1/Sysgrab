add_library(log STATIC log.c log.h)

target_include_directories(log PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                      $<INSTALL_INTERFACE:include/log>)
