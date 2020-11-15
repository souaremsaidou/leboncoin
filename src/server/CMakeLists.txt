set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

add_library(server helpers.hpp helpers.cpp server.hpp server.cpp api.hpp api.cpp)

target_compile_options(server
	PRIVATE
		${flags}
	)

set_property(TARGET server PROPERTY CXX_STANDARD 14)
set_property(TARGET server PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(server
	PUBLIC
		restbed-shared 
		${CMAKE_THREAD_LIBS_INIT} 
		${PostgreSQL_LIBRARIES} 
		${PQXX_LIBRARIES} 
		fizzbuzz
		Boost::program_options
		gcov
	)