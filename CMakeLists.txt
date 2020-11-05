cmake_minimum_required(VERSION 3.0.0)
project(server VERSION 0.1.0)

########################
# Build Options
########################

########################
# Configuration
########################

set( CMAKE_CXX_STANDARD 14 )

include(CTest)
enable_testing()

#add_executable(server main.cpp)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

# Download and unpack restbed at configure time
configure_file(CMakeLists.txt.in restbed-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/restbed-download )
if(result)
  message(FATAL_ERROR "CMake step for restbed failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/restbed-download )
if(result)
  message(FATAL_ERROR "Build step for restbed failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SSL OFF CACHE BOOL "" FORCE)

# Add restbed directly to our build. This defines
# the restbed-shared target.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/restbed-src
                 ${CMAKE_CURRENT_BINARY_DIR}/restbed-build
                 EXCLUDE_FROM_ALL)

# we have to add them here ourselves.
set(ARTIFACTS "${CMAKE_CURRENT_BINARY_DIR}/restbed-src/source")
include_directories("${ARTIFACTS}")

find_package (Threads)

# Now simply link against restbed-shared
add_executable(server main.cpp)
target_link_libraries(server restbed-shared ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME server_test COMMAND server)
