# Core Sysgrab library
add_library(sysgrab_core STATIC art.c)

target_include_directories(sysgrab_core PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
											   $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_link_libraries(sysgrab_core PRIVATE log parson)

# Sysgrab executable
add_executable(sysgrab main.c)

target_link_libraries(sysgrab PRIVATE sysgrab_core log)

add_custom_target(copy_default_files ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/art.txt"
		"$<TARGET_FILE_DIR:sysgrab>/art.txt"
	COMMENT "Copying default files to output directory"
	VERBATIM
)
add_dependencies(sysgrab copy_default_files) 
