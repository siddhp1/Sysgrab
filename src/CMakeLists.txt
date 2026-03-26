set(SOURCES main.c sysgrab_art.c sysgrab_config.c sysgrab_data.c sysgrab_log.c sysgrab_number.c sysgrab_path.c sysgrab_print.c sysgrab_string.c sysgrab_time.c)

add_executable(sysgrab ${SOURCES})

target_include_directories(sysgrab PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
										  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_link_libraries(sysgrab PRIVATE yaml)

add_custom_target(copy_art_file ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/art.txt"
		"$<TARGET_FILE_DIR:sysgrab>/art.txt"
	COMMENT "Copying art.txt to output directory"
	VERBATIM
)

add_custom_target(copy_config_file ALL 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/config.yaml"
		"$<TARGET_FILE_DIR:sysgrab>/config.yaml"
	COMMENT "Copying config.yaml to output directory"
	VERBATIM
)

add_dependencies(sysgrab copy_art_file copy_config_file)
