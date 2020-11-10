cmake_minimum_required(VERSION 3.0.0)
project(server VERSION 0.1.0)

########################
# Build Options
########################

########################
# Configuration
########################

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )

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

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# defines targets and sources
add_subdirectory(src)

# enable testing and define tests
enable_testing()
add_subdirectory(tests)
