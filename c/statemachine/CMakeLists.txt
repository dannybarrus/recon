add_library(recon_statemachine
  src/printer_controller.c
)
target_include_directories(recon_statemachine PUBLIC include)

add_executable(statemachine_demo demo/main.c)
target_link_libraries(statemachine_demo PRIVATE recon_statemachine)

add_executable(statemachine_test test/statemachine_test.c)
target_link_libraries(statemachine_test PRIVATE recon_statemachine unity)
unity_discover_tests(statemachine_test ${CMAKE_CURRENT_SOURCE_DIR}/test/statemachine_test.c)
