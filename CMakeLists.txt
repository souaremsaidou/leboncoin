cmake_minimum_required(VERSION 3.0.0)
project(server VERSION 0.1.0)

#
# Build Options
#
#option( BUILD_TESTS    "Build unit tests."   ON )
#option( BUILD_SSL      "Build secure socket layer support."  ON )

#
# Configuration
#
set( CMAKE_CXX_STANDARD 14 )

include(CTest)
enable_testing()

#add_executable(server main.cpp)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

# Download and unpack googletest at configure time
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
# settings on Windows//
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)//
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SSL OFF CACHE BOOL "" FORCE)

# Add restbed directly to our build. This defines
# the gtest and gtest_main targets.//
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/restbed-src
                 ${CMAKE_CURRENT_BINARY_DIR}/restbed-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
  #include_directories("${gtest_SOURCE_DIR}/include")
#endif()
set(ARTIFACTS "${CMAKE_CURRENT_BINARY_DIR}/restbed-src/source")
message("[ARTIFACTS] ${ARTIFACTS}")
include_directories("${ARTIFACTS}")

find_package (Threads)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(server main.cpp)
#target_link_libraries(example gtest_main)
target_link_libraries(server ${CMAKE_CURRENT_BINARY_DIR}/restbed-build/librestbed.a ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME server_test COMMAND server)