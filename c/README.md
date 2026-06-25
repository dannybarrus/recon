# c

C is the one language in this repo that isn't here because of interview
prep. It's just my favorite, and it earned its own showcase on that
basis alone. Where `cpp/` is about what the language gives you beyond
C, this folder is about what you get when none of that exists yet:
raw pointers, manual memory layout, and the discipline that comes from
having no safety net at all.

## What's here

| Folder          | Covers |
|-----------------|--------|
| `ringbuffer/`   | Lock-free single-producer/single-consumer queue -- macro-generated for type safety, C11 atomics with explicit acquire/release ordering |
| `statemachine/` | Function-pointer dispatch table, modeled on a 3D printer motion controller (IDLE/HOMING/READY/PRINTING/PAUSED/ERROR) |
| `matrixrotate/` | In-place 90-degree N x N rotation, true O(1) auxiliary space, cross-checked against a naive reference across hundreds of randomized trials |
| `allocator/`    | Arena (bump) and pool (fixed-block, intrusive free-list) allocators, benchmarked honestly against malloc/free |
| `hashtable/`    | String-interning dictionary -- open addressing for cache locality, cache-line alignment enforced at compile time, O(1) lookup proven empirically |
| `hwregister/`   | Mock memory-mapped UART peripheral; the volatile-vs-not optimizer divergence built as two real, runnable executables, not just described |
| `tokenizer/`    | Zero-copy G-code lexer -- every token a pointer into the original line buffer, numeric values parsed in place with no copying at all |
| `errors/`       | Thread-local error stack (OpenSSL's architecture) and an explicit error-context struct, built side by side so the real tradeoff is visible |

`common/` holds small cross-platform shims (`recon_thread.h`, `recon_time.h`) shared by several topics above, since plain C has no single threading or timing API that behaves identically across MSVC and POSIX.

## Building

```bat
cd c
cmake -S . -B build
cmake --build build
```

Run the full test suite:

```bat
cd build
ctest -C Debug      :: Visual Studio (multi-config) generators
ctest               :: single-config generators (Makefiles, Ninja)
```

Run an individual demo directly, e.g.:

```bat
.\ringbuffer\Debug\ringbuffer_demo.exe
```
```bash
./ringbuffer/ringbuffer_demo
```

## Toolchain notes

- CMake 3.14+
- A C11-capable compiler. On MSVC specifically, a reasonably recent Visual Studio (16.8 / MSVC 14.28 or later) -- `<stdatomic.h>` needs the `/experimental:c11atomics` flag on MSVC, already configured at the project root.
- [Unity](https://github.com/ThrowTheSwitch/Unity) (the de facto embedded C testing framework, the same reasoning that chose GoogleTest for `cpp/` and Maven/JUnit for `java/`) is fetched automatically via CMake `FetchContent` -- no manual install.

## Sanitizers

```bat
cmake -S . -B build_asan -DRECON_ASAN=ON
cmake -S . -B build_tsan -DRECON_TSAN=ON
```

Don't combine `RECON_ASAN` and `RECON_TSAN` in the same build directory -- use separate ones, as above. `RECON_TSAN` matters more in this folder than it does in `cpp/`: several topics here (`ringbuffer`, `hwregister`, `errors`) are genuinely concurrent code, and ThreadSanitizer is the tool that actually verifies that, rather than just trusting it.

## Test discovery

`ctest` shows each test as its own individual entry, not one entry per binary, via `cmake/UnityDiscoverTests.cmake` -- the C analog of GoogleTest's `gtest_discover_tests()`. Unlike that mechanism, Unity has no built-in way to ask a built binary "what tests do you contain," so this works by scanning each test file's `test_cases[]` table for names **at CMake configure time**. That means adding, renaming, or removing a test needs a fresh `cmake -S . -B build` reconfigure before `ctest` picks up the change -- a plain rebuild alone isn't enough, even though running the test binary directly always reflects the current table immediately.

## Cross-topic dependencies

- `hashtable` links against `allocator`'s `Arena` for interned string storage -- every interned string lives for the table's whole lifetime, exactly the access pattern an arena is for.
- `errors`' demo links against `tokenizer` to show realistic G-code parsing failures being aggregated through both error architectures.

## Platform notes worth knowing (found the hard way)

- **`long` is only 32 bits on Windows x64** (the LLP64 data model), but 64 bits on Linux/macOS (LP64). A literal like `3000000000L` silently overflows and wraps negative on Windows specifically, while working fine on Linux. Use `long long` (guaranteed at least 64 bits everywhere) for anything that needs to scale into the billions.
- **Symbol collisions with system headers are real, not theoretical.** `tokenizer`'s public types are prefixed `GCode...` after a bare `TokenType` collided directly with Windows' own `winnt.h`, which typedefs an unrelated Win32 security enum with the exact same name. Plain C has no namespaces -- prefixing public symbols with a project- or module-specific tag is the actual mitigation, not just a style preference.
- **MSVC's `/RTC1` (the default Debug-mode runtime checks) and `/O2` are flatly incompatible** -- the compiler rejects the combination outright (error D8016) rather than letting one flag silently win. `hwregister`'s `volatile_proof` executables, which need real optimization to demonstrate anything at all, skip forcing optimization on MSVC entirely and rely on an actual Release build (`cmake --build build --config Release`) to sidestep this.
- **Unity excludes double-precision float assertions by default** (`TEST_ASSERT_EQUAL_DOUBLE` and similar) -- many embedded targets have no double-precision FPU at all. This project opts in globally via `UNITY_INCLUDE_DOUBLE` at the root, since several topics here genuinely need it.
