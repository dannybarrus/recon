add_library(recon_datastructures
  src/DoublyLinkedList.cpp
  src/SinglyLinkedList.cpp
)
target_include_directories(recon_datastructures PUBLIC include)

add_executable(datastructures_demo demo/main.cpp)
target_link_libraries(datastructures_demo PRIVATE recon_datastructures)

add_executable(datastructures_test
  test/DoublyLinkedList_test.cpp
  test/SinglyLinkedList_test.cpp
)
target_link_libraries(datastructures_test PRIVATE recon_datastructures GTest::gtest_main)
gtest_discover_tests(datastructures_test)
