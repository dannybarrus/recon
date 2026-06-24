# Header-only -- the macro generates everything inline, the same reason
# C++ class templates have no separate .cpp file. No recon_ringbuffer
# library target is needed; demo and test each just include the header
# directly.

add_executable(ringbuffer_demo demo/main.c)
target_include_directories(ringbuffer_demo PRIVATE include ${CMAKE_SOURCE_DIR}/common/include)
target_link_libraries(ringbuffer_demo PRIVATE Threads::Threads)

add_executable(ringbuffer_test test/ringbuffer_test.c)
target_include_directories(ringbuffer_test PRIVATE include ${CMAKE_SOURCE_DIR}/common/include)
target_link_libraries(ringbuffer_test PRIVATE unity Threads::Threads)
unity_discover_tests(ringbuffer_test ${CMAKE_CURRENT_SOURCE_DIR}/test/ringbuffer_test.c)
