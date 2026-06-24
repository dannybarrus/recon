cmake_minimum_required(VERSION 3.14)
project(recon_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# ─── Warnings ─────────────────────────────────────────────────────────────────
if(MSVC)
  add_compile_options(/W4)
  # MSVC gates C11 <stdatomic.h> support behind this experimental flag --
  # without it, including <stdatomic.h> fails with:
  #   error C1189: #error: "C atomic support is not enabled"
  # even when CMAKE_C_STANDARD is set to 11. Requires a reasonably
  # recent Visual Studio (this flag was added in VS 16.8 / MSVC 14.28).
  add_compile_options(/experimental:c11atomics)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ─── AddressSanitizer (optional, off by default) ─────────────────────────────
# Enable with: cmake -S . -B build -DRECON_ASAN=ON
option(RECON_ASAN "Build with AddressSanitizer" OFF)
if(RECON_ASAN AND NOT MSVC)
  add_compile_options(-fsanitize=address -g)
  add_link_options(-fsanitize=address)
endif()

# ─── ThreadSanitizer (optional, off by default) ──────────────────────────────
# Enable with: cmake -S . -B build -DRECON_TSAN=ON
# This matters far more here than it did in cpp/ -- several topics in
# this folder (the ring buffer especially) are genuinely lock-free
# concurrent code, and TSan is the tool that actually verifies that
# claim rather than just trusting it. Do not combine with RECON_ASAN in
# the same build; use separate build directories.
option(RECON_TSAN "Build with ThreadSanitizer" OFF)
if(RECON_TSAN AND NOT MSVC)
  add_compile_options(-fsanitize=thread -g)
  add_link_options(-fsanitize=thread)
endif()

find_package(Threads REQUIRED)

# ─── Unity (embedded C testing framework) ────────────────────────────────────
# Chosen over a generic C test runner because it's the de facto
# standard for embedded/firmware C testing specifically -- the same
# reasoning that chose Maven/JUnit for the Java module (match what real
# shops actually use) and GoogleTest for the C++ module.
include(FetchContent)
FetchContent_Declare(
  unity
  URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz
)
FetchContent_MakeAvailable(unity)

# Our G-code tokenizer (and likely other future topics) deal in doubles,
# not just floats -- Unity excludes double-precision comparison macros
# by default (many embedded targets have no double-precision FPU at
# all), so this needs to be explicitly opted into. Applied to the
# `unity` target itself (PUBLIC) so it's consistently defined both when
# compiling unity.c and in every test file that links against it.
target_compile_definitions(unity PUBLIC UNITY_INCLUDE_DOUBLE)

enable_testing()

# Provides unity_discover_tests() -- see cmake/UnityDiscoverTests.cmake
# for what it does and why it works the way it does.
include(${CMAKE_SOURCE_DIR}/cmake/UnityDiscoverTests.cmake)

# ─── Subprojects ──────────────────────────────────────────────────────────────
add_subdirectory(ringbuffer)
add_subdirectory(statemachine)
add_subdirectory(matrixrotate)
add_subdirectory(allocator)
add_subdirectory(hashtable)
add_subdirectory(hwregister)
add_subdirectory(tokenizer)
add_subdirectory(errors)
