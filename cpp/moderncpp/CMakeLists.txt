add_library(recon_moderncpp
  src/Buffer.cpp
  src/MoveSemanticsExamples.cpp
)
target_include_directories(recon_moderncpp PUBLIC include)

add_executable(moderncpp_demo demo/main.cpp)
target_link_libraries(moderncpp_demo PRIVATE recon_moderncpp)

add_executable(moderncpp_test
  test/Buffer_test.cpp
  test/Pair_test.cpp
  test/GenericStack_test.cpp
  test/Clamp_test.cpp
  test/Describe_test.cpp
  test/VariadicSum_test.cpp
  test/TypeBranching_test.cpp
  test/MoveSemanticsExamples_test.cpp
)
target_link_libraries(moderncpp_test PRIVATE recon_moderncpp GTest::gtest_main)
gtest_discover_tests(moderncpp_test)
