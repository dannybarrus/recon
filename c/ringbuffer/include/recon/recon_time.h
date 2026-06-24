#pragma once

// A minimal cross-platform monotonic timer, returning elapsed seconds
// as a double.
//
// POSIX provides clock_gettime(CLOCK_MONOTONIC, ...) via <time.h>, but
// MSVC does not implement it -- the same category of gap as
// <threads.h> in recon_thread.h. Windows' native equivalent is the
// QueryPerformanceCounter/QueryPerformanceFrequency pair. This shim
// wraps both behind one consistent function so calling code never has
// to branch on platform itself.
//
// Intended for single-threaded timing measurement (call it before and
// after the work being measured, subtract). The lazy frequency-cache
// inside the Windows path is not safe to call concurrently from
// multiple threads the first time -- that's fine for how this is
// actually used in this folder's demos and tests (always from the
// single thread doing the timing, never from the producer/consumer
// worker threads themselves), but worth knowing if it's ever reused
// somewhere that calls it from multiple threads at once.

#ifdef _WIN32

#include <windows.h>

static inline double recon_monotonic_seconds(void) {
    static LARGE_INTEGER frequency = {0};
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
}

#else

#include <time.h>

static inline double recon_monotonic_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

#endif
