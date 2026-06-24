add_library(recon_allocator
  src/arena.c
  src/pool.c
)
target_include_directories(recon_allocator PUBLIC include)

add_executable(allocator_demo demo/main.c)
target_link_libraries(allocator_demo PRIVATE recon_allocator)
target_include_directories(allocator_demo PRIVATE ${CMAKE_SOURCE_DIR}/common/include)

add_executable(allocator_test test/allocator_test.c)
target_link_libraries(allocator_test PRIVATE recon_allocator unity)
unity_discover_tests(allocator_test ${CMAKE_CURRENT_SOURCE_DIR}/test/allocator_test.c)
