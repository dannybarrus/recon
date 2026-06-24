add_library(recon_errors
  src/error_stack.c
  src/error_context.c
)
target_include_directories(recon_errors PUBLIC include)
target_link_libraries(recon_errors PUBLIC Threads::Threads)

add_executable(errors_demo demo/main.c)
target_link_libraries(errors_demo PRIVATE recon_errors recon_tokenizer)
target_include_directories(errors_demo PRIVATE ${CMAKE_SOURCE_DIR}/common/include)

add_executable(errors_test test/errors_test.c)
target_link_libraries(errors_test PRIVATE recon_errors unity)
target_include_directories(errors_test PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
unity_discover_tests(errors_test ${CMAKE_CURRENT_SOURCE_DIR}/test/errors_test.c)
