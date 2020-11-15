cmake_minimum_required(VERSION 3.0.0)
project(WebServer VERSION 0.1.0)

message( "                                                              " )
message( "##############################################################" )
message( "#                                                            #" )
message( "#  leboncoin is MIT License                                  #" )
message( "#  Copyright (c) 2020 souare.saidou@gmail.com                #" )
message( "#                                                            #" )
message( "##############################################################" )
message( "                                                              " )

set( CMAKE_CXX_STANDARD 14 )

option(BUILD_COVERAGE "Build with Code-Coverage" OFF)
list(APPEND flags "-fPIC" "-Wall")
if(BUILD_COVERAGE)
  if(NOT WIN32)
    list(APPEND flags "-g" "-O0" "-Wall" "-fprofile-arcs" "-ftest-coverage")
  endif()
endif()

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/" )

# Download and unpack restbed at configure time
configure_file("${PROJECT_SOURCE_DIR}/cmake/CMakeLists.txt.in" restbed-download/CMakeLists.txt)
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

########################
# Configure Threads
########################
find_package (Threads)

########################
# Configure Boost
########################
find_package(Boost 1.58 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

########################
# Configure PostgreSQL
########################
find_package(PostgreSQL REQUIRED)
include_directories({PostgreSQL_INCLUDE_DIRS})

find_package(PQXX REQUIRED)
include_directories({PQXX_INCLUDE_DIRECTORIES})

########################
# Configure RapidJSON
########################
include("${PROJECT_SOURCE_DIR}/cmake/rapidjson_fetch_git_repo.cmake")
rapidjson_fetch_git_repo(
  rapidjson
  ${CMAKE_BINARY_DIR}/_rapidjson_deps
  https://github.com/Tencent/rapidjson.git #https://github.com/google/googletest.git
  v1.1.0 #release-1.8.0
)

# Add rapidjson directly to our build.
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_rapidjson_deps/rapidjson-src/include/)

########################
# Configure GNUInstallDirs
########################
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

if(BUILD_COVERAGE)
  set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
endif()

# defines targets and sources
add_subdirectory(src)

# enable testing and define tests
enable_testing()
add_subdirectory(tests)
