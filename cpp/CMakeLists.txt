cmake_minimum_required(VERSION 3.14)
project(recon_cpp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# ─── Warnings ─────────────────────────────────────────────────────────────────
# Treat the codebase as something that should compile clean, not just compile.
if(MSVC)
  add_compile_options(/W4)
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
# Catches data races -- a different class of bug than AddressSanitizer's
# memory-safety focus. Do not enable both RECON_ASAN and RECON_TSAN in the
# same build; they are mutually incompatible instrumentation.
option(RECON_TSAN "Build with ThreadSanitizer" OFF)
if(RECON_TSAN AND NOT MSVC)
  add_compile_options(-fsanitize=thread -g)
  add_link_options(-fsanitize=thread)
endif()

find_package(Threads REQUIRED)

# ─── GoogleTest ───────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
# Windows: link against the same CRT GoogleTest expects, avoids a common
# "LNK2038 mismatch detected" build error on MSVC.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# ─── Subprojects ──────────────────────────────────────────────────────────────
add_subdirectory(algorithms)
add_subdirectory(datastructures)
add_subdirectory(memory)
add_subdirectory(moderncpp)
