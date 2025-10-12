add_library(sysgrab_core STATIC sysgrab_art.c sysgrab_config.c sysgrab_data.c sysgrab_log.c sysgrab_number.c sysgrab_path.c sysgrab_print.c sysgrab_string.c sysgrab_time.c)

target_include_directories(sysgrab_core PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
											   $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_link_libraries(sysgrab_core yaml)

add_executable(sysgrab main.c)

target_link_libraries(sysgrab PRIVATE sysgrab_core)

add_custom_target(copy_default_files ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/art.txt"
		"$<TARGET_FILE_DIR:sysgrab>/art.txt"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/config.yaml"
		"$<TARGET_FILE_DIR:sysgrab>/config.yaml"
	COMMENT "Copying default files to output directory"
	VERBATIM
)
add_dependencies(sysgrab copy_default_files) 
