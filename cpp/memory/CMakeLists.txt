add_library(recon_memory
  src/TrackedResource.cpp
  src/ExceptionSafetyExamples.cpp
  src/GraphNode.cpp
  src/LegacyHandle.cpp
)
target_include_directories(recon_memory PUBLIC include)
target_link_libraries(recon_memory PUBLIC Threads::Threads)

add_executable(memory_demo demo/main.cpp)
target_link_libraries(memory_demo PRIVATE recon_memory)

add_executable(memory_test
  test/TrackedResource_test.cpp
  test/ExceptionSafetyExamples_test.cpp
  test/GraphNode_test.cpp
  test/LegacyHandle_test.cpp
  test/ScopedLock_test.cpp
)
target_link_libraries(memory_test PRIVATE recon_memory GTest::gtest_main)
gtest_discover_tests(memory_test)
