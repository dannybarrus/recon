#pragma once

// A minimal, just-enough-for-this-folder cross-platform thread
// abstraction.
//
// C11 added <threads.h>, but in practice support is inconsistent --
// notably, MSVC has historically not implemented it at all, which
// would make code depending on it fail to build on Windows
// specifically, the primary platform this repo is built and verified
// on. There is no single standard threading API guaranteed to exist
// and behave identically on both POSIX and Windows in plain C, so a
// thin #ifdef shim over each platform's own native, always-available
// threading API is the more honest, more portable choice -- this is a
// completely ordinary thing for real embedded/systems C code to do.
//
// VERIFICATION NOTE: the POSIX/pthreads path below was compiled, run,
// and verified under both AddressSanitizer and ThreadSanitizer. The
// Windows path is written carefully against documented Win32 API
// behavior but was NOT able to be compiled or run in the environment
// that wrote it -- there is no Windows compiler available there. Build
// and run this on your own machine as the first real verification of
// that path; if anything is off, that's the place to look first.

#ifdef _WIN32

#include <windows.h>
#include <stdlib.h>

typedef HANDLE recon_thread_t;

// Windows' CreateThread requires a function of signature
// `DWORD WINAPI (*)(LPVOID)`, not the POSIX `void* (*)(void*)` this
// header's API uses. Rather than cast between incompatible function
// pointer types (a common but technically undefined-behavior shortcut
// in real Windows C code), this trampoline adapts the call properly:
// it stores the real function and argument in a small heap-allocated
// struct, and CreateThread calls the trampoline, which then calls the
// real function with the correct signature.
typedef struct {
    void* (*func)(void*);
    void* arg;
} recon_thread_trampoline_args_t;

static inline DWORD WINAPI recon_thread_trampoline(LPVOID param) {
    recon_thread_trampoline_args_t* args = (recon_thread_trampoline_args_t*)param;
    args->func(args->arg);
    free(args);
    return 0;
}

static inline int recon_thread_create(recon_thread_t* thread, void* (*func)(void*), void* arg) {
    recon_thread_trampoline_args_t* args =
        (recon_thread_trampoline_args_t*)malloc(sizeof(recon_thread_trampoline_args_t));
    if (!args) {
        return -1;
    }
    args->func = func;
    args->arg = arg;

    *thread = CreateThread(NULL, 0, recon_thread_trampoline, args, 0, NULL);
    if (*thread == NULL) {
        free(args);
        return -1;
    }
    return 0;
}

static inline void recon_thread_join(recon_thread_t thread) {
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

static inline void recon_thread_yield(void) {
    SwitchToThread();
}

#else

#include <pthread.h>
#include <sched.h>

typedef pthread_t recon_thread_t;

static inline int recon_thread_create(recon_thread_t* thread, void* (*func)(void*), void* arg) {
    return pthread_create(thread, NULL, func, arg);
}

static inline void recon_thread_join(recon_thread_t thread) {
    pthread_join(thread, NULL);
}

static inline void recon_thread_yield(void) {
    sched_yield();
}

#endif
