add_library(recon_matrixrotate
  src/matrix_rotate.c
)
target_include_directories(recon_matrixrotate PUBLIC include)

add_executable(matrixrotate_demo demo/main.c)
target_link_libraries(matrixrotate_demo PRIVATE recon_matrixrotate)

add_executable(matrixrotate_test test/matrixrotate_test.c)
target_link_libraries(matrixrotate_test PRIVATE recon_matrixrotate unity)
unity_discover_tests(matrixrotate_test ${CMAKE_CURRENT_SOURCE_DIR}/test/matrixrotate_test.c)
