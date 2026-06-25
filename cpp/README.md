# cpp

Where `c/` is about what you have when nothing exists yet, this folder
is about what the language gives you on top of that: RAII, smart
pointers, move semantics, templates. Both folders started from the
same original interview-prep problems, but `cpp/` grew well past that
scope into a fully tested reference library in its own right.

## What's here

| Folder            | Covers |
|-------------------|--------|
| `algorithms/`     | Classic interview problems, rebuilt from the original files -- bit counting, Fibonacci, string reversal, wildcard matching, and more |
| `datastructures/` | Singly/doubly linked lists, deliberately non-generic -- genericity is the deferred payoff in `moderncpp/` |
| `memory/`         | RAII, `unique_ptr`/`shared_ptr`, the classic circular-reference leak and its `weak_ptr` fix, exception-safe `ScopedLock` |
| `moderncpp/`      | Move semantics measured (not just claimed), class/function templates, explicit specialization, variadic templates, `if constexpr` |

## Building

```bat
cd cpp
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
.\algorithms\Debug\algorithms_demo.exe
```
```bash
./algorithms/algorithms_demo
```

## Toolchain notes

- CMake 3.14+
- A C++17-capable compiler
- [GoogleTest](https://github.com/google/googletest) v1.14.0 is fetched automatically via CMake `FetchContent` -- no manual install. `gtest_discover_tests()` (GoogleTest's own built-in CMake module, not a custom script) registers each test as its own individual `ctest` entry.
- On MSVC, `gtest_force_shared_crt` is set before fetching GoogleTest -- without it, GoogleTest links against a different C runtime than the rest of the project, producing an `LNK2038` mismatch error at link time. Already configured at the project root.

## Sanitizers

```bat
cmake -S . -B build_asan -DRECON_ASAN=ON
cmake -S . -B build_tsan -DRECON_TSAN=ON
```

Don't combine `RECON_ASAN` and `RECON_TSAN` in the same build directory -- use separate ones, as above.

**`memory/` contains two deliberate, documented leaks** -- the `shared_ptr` circular-reference demo and a raw-pointer exception-safety demo, both intentionally leaking to *prove* the bug they're demonstrating exists, and both independently confirmed by LeakSanitizer with exact file:line provenance when ASan is built with leak detection on. To get a clean (exit code 0) ASan run accounting for these two known cases:

```bash
ASAN_OPTIONS=detect_leaks=0 ctest
```

Everything else in the codebase is genuinely clean under both sanitizers without that flag.

## Cross-topic dependencies

- `moderncpp/`'s `GenericStack<T>` is the deferred payoff from `datastructures/`'s `SinglyLinkedList`/`DoublyLinkedList`, which were deliberately written as concrete, non-generic types for `PhoneRecord` specifically -- genericity was always meant to land here instead.
- `memory/`'s `unique_ptr` move-only semantics are directly contrasted against `datastructures/`'s classes, which had to manually delete their copy constructor and hand-write a move constructor since they predate the smart-pointer treatment.

## Bugs found the hard way

A few of the more substantial findings, each verified by an actual tool rather than taken on faith:

- **`algorithms/contigarray`** -- an AddressSanitizer-confirmed `stack-buffer-overflow`: the original read one element past the array bounds, and that out-of-bounds read was accidentally load-bearing (it was the only thing flushing the final run).
- **`algorithms/expressionmatch`** -- a wildcard matcher that fooled all 8 of its own original test cases. It validated literal text before the first `*` and never checked anything after it; none of the original tests were adversarial enough to catch this.
- **`datastructures/` Rule of Five** -- deleting the copy constructor on `SinglyLinkedList` (correctly, since it owns raw pointers) without also providing a move constructor made the class impossible to return by value at all. A real compiler error forced the fix, rather than reading about why move semantics matter abstractly.
- **`memory/` `shared_ptr` cycle** -- `aliveCount()` proves two nodes in a reference cycle never reach zero after their last external reference goes out of scope, and the same leak is independently confirmed by LeakSanitizer. Swapping one direction of the cycle to `weak_ptr` brings the count back to zero.
- **`moderncpp/` move-vs-copy benchmark** -- the first version measured a misleadingly small ~7x speedup, because the "move" timing included fresh-construction cost inside the timed loop. The corrected version (each phase pre-builds its own sources, frees them before the next phase starts) found a real ~28x speedup -- and an early attempt at the fix also briefly OOM-killed the build sandbox by holding too many large buffers alive simultaneously, which is its own small lesson about benchmark memory footprints.
