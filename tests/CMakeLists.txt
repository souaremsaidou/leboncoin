add_executable(fizzbuzz_test fizzbuzz.cpp)

#target_link_libraries(fizzbuzz_test fuzzbuzz)
target_link_libraries(fizzbuzz_test fizzbuzz)

add_test(
  NAME
    fizzbuzz_unit_test
  COMMAND
    $<TARGET_FILE:fizzbuzz_test>
  )
