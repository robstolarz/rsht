cmake_minimum_required(VERSION 3.6)

if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

project(RSHT LANGUAGES C VERSION 0.1.0)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
  message( FATAL_ERROR "In-source builds aren't allowed.\nDo `rm -f CMakeCache.txt && cd build && cmake ..` instead." )
endif()

set(SOURCE_FILES
  src/rsht.c
)
add_library(rsht ${SOURCE_FILES})
target_include_directories(rsht PUBLIC .)

set(RSHT_TEST_SOURCE_FILES
  test/rsht_test.c
)
if (NOT_SUBPROJECT)
  enable_testing()
  add_executable(rsht_test ${RSHT_TEST_SOURCE_FILES})
  target_link_libraries(rsht_test rsht)
  add_test(the_test rsht_test)
endif()
