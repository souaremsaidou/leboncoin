set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

add_subdirectory(fizzbuzz)
add_subdirectory(server)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME}
	PRIVATE
		${flags}
	)

set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14 )
set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON )
set_target_properties( ${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} )

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		server
	)
