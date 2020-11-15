include(${PROJECT_SOURCE_DIR}/cmake/fetch_git_repo.cmake)

fetch_git_repo(
  googletest
  ${CMAKE_BINARY_DIR}/_deps
  https://github.com/google/googletest.git
  release-1.8.0
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Prevent GoogleTest from using PThreads
set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

# adds the targers: gtest, gtest_main, gmock, gmock_main
add_subdirectory(
  ${googletest_SOURCE_DIR}
  ${googletest_BINARY_DIR}
  )

# Silence std::tr1 warning on MSVC
if(MSVC)
  foreach(_tgt gtest gtest_main gmock gmock_main)
    target_compile_definitions(${_tgt}
      PRIVATE
        "_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING"
      )
  endforeach()
endif()

list(APPEND _tests test_fizzbuzz test_api test_helpers test_server)
foreach(_test IN LISTS _tests)
  add_executable(${_test} "")
  target_sources(${_test}
    PRIVATE
      ${_test}.cpp
    )

  target_compile_options(${_test}
    PRIVATE
      ${flags}
    )

  target_link_libraries(${_test}
    PRIVATE
      server
      fizzbuzz
      gtest_main
      gcov
    )

  add_test(
    NAME ${_test}_unit
    COMMAND $<TARGET_FILE:${_test}>
    )
endforeach()