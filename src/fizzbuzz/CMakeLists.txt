########################
# libray fizzbuzz
########################
add_library(fizzbuzz "")

set_target_properties(fizzbuzz PROPERTIES LINKER_LANGUAGE CXX)

target_sources(fizzbuzz
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/fizzbuzz.cpp
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/fizzbuzz.hpp
  )

target_include_directories(fizzbuzz
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )
