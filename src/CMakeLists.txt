# Now simply link against gtest or gtest_main as needed. Eg
add_executable(server main.cpp helpers.hpp helpers.cpp server.hpp server.cpp api.hpp api.cpp)

# contains an "external" library we will link to
add_subdirectory(fizzbuzz)

target_link_libraries(server
	PRIVATE
		restbed-shared 
		${CMAKE_THREAD_LIBS_INIT} 
		${PostgreSQL_LIBRARIES} 
		${PQXX_LIBRARIES} 
		fizzbuzz
		Boost::program_options
	)
