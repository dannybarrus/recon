add_library(recon_tokenizer
  src/gcode_tokenizer.c
)
target_include_directories(recon_tokenizer PUBLIC include)

add_executable(tokenizer_demo demo/main.c)
target_link_libraries(tokenizer_demo PRIVATE recon_tokenizer)

add_executable(tokenizer_test test/tokenizer_test.c)
target_link_libraries(tokenizer_test PRIVATE recon_tokenizer unity)
unity_discover_tests(tokenizer_test ${CMAKE_CURRENT_SOURCE_DIR}/test/tokenizer_test.c)
