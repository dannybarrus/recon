add_library(recon_algorithms
  src/bitcounting.cpp
  src/fibonacci.cpp
  src/reversestring.cpp
  src/reversewords.cpp
  src/custom_strchr.cpp
  src/removeduplicates.cpp
  src/contigarray.cpp
  src/comparestr.cpp
  src/expressionmatch.cpp
  src/strintintstr.cpp
)
target_include_directories(recon_algorithms PUBLIC include)

add_executable(algorithms_demo demo/main.cpp)
target_link_libraries(algorithms_demo PRIVATE recon_algorithms)

add_executable(algorithms_test
  test/bitcounting_test.cpp
  test/fibonacci_test.cpp
  test/reversestring_test.cpp
  test/reversewords_test.cpp
  test/custom_strchr_test.cpp
  test/removeduplicates_test.cpp
  test/contigarray_test.cpp
  test/comparestr_test.cpp
  test/expressionmatch_test.cpp
  test/strintintstr_test.cpp
)
target_link_libraries(algorithms_test PRIVATE recon_algorithms GTest::gtest_main)
gtest_discover_tests(algorithms_test)
