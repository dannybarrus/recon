add_library(recon_hwregister
  src/uart_registers.c
)
target_include_directories(recon_hwregister PUBLIC include)
target_include_directories(recon_hwregister PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
target_link_libraries(recon_hwregister PUBLIC Threads::Threads)

add_executable(hwregister_demo demo/main.c)
target_link_libraries(hwregister_demo PRIVATE recon_hwregister)
target_include_directories(hwregister_demo PRIVATE ${CMAKE_SOURCE_DIR}/common/include)

add_executable(hwregister_test test/hwregister_test.c)
target_link_libraries(hwregister_test PRIVATE recon_hwregister unity)
target_include_directories(hwregister_test PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
unity_discover_tests(hwregister_test ${CMAKE_CURRENT_SOURCE_DIR}/test/hwregister_test.c)

# ─── volatile_proof: built twice, forced to a real optimization level ───────
# The volatile-elision bug this demonstrates is invisible at -O0/-Od,
# so building it the normal Debug way would silently fail to show
# anything.
#
# On GCC/Clang, forcing -O2 via target_compile_options works cleanly
# regardless of the overall build's configuration.
#
# On MSVC, the same approach does NOT work: MSVC's Debug configuration
# injects /RTC1 (basic runtime checks), which the compiler flatly
# refuses to combine with /O2 at all (error D8016) -- and simply also
# passing /RTC- to try to override it does not resolve this in
# practice, which was confirmed by actually hitting the error. Rather
# than fight MSVC's flag plumbing further, these two targets are left
# with NO forced optimization flag on MSVC at all; to see the real
# comparison on Windows, build and run them in Release configuration
# specifically, where MSVC does not inject /RTC1 in the first place:
#   cmake --build build --config Release
#   .\hwregister\Release\hwregister_volatile_proof_with.exe
#   .\hwregister\Release\hwregister_volatile_proof_without.exe
if(MSVC)
  set(RECON_FORCE_OPT_FLAG "")
else()
  set(RECON_FORCE_OPT_FLAG -O2)
endif()

add_executable(hwregister_volatile_proof_with demo/volatile_proof.c)
target_compile_definitions(hwregister_volatile_proof_with PRIVATE USE_VOLATILE)
if(RECON_FORCE_OPT_FLAG)
  target_compile_options(hwregister_volatile_proof_with PRIVATE ${RECON_FORCE_OPT_FLAG})
endif()
target_include_directories(hwregister_volatile_proof_with PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
target_link_libraries(hwregister_volatile_proof_with PRIVATE Threads::Threads)

add_executable(hwregister_volatile_proof_without demo/volatile_proof.c)
if(RECON_FORCE_OPT_FLAG)
  target_compile_options(hwregister_volatile_proof_without PRIVATE ${RECON_FORCE_OPT_FLAG})
endif()
target_include_directories(hwregister_volatile_proof_without PRIVATE ${CMAKE_SOURCE_DIR}/common/include)
target_link_libraries(hwregister_volatile_proof_without PRIVATE Threads::Threads)
