#pragma once

#include <stddef.h>

// A thread-local error stack -- the same architecture OpenSSL uses
// (ERR_put_error()/ERR_get_error()): any function, anywhere in a call
// chain, can record a detailed error without needing an explicit
// "error context" parameter threaded through every single function
// signature in the codebase. The caller, after seeing a failure
// indication from the top-level call, drains the accumulated history
// to see everything that went wrong and where.
//
// ─── Why thread-local, not a single global stack ───────────────────────────
//
// A single process-wide global stack would be a real, immediate
// problem the moment two threads are both reporting errors at once --
// either a data race on the stack itself, or a lock serializing every
// error report across the whole program. Thread-local storage
// sidesteps this entirely: each thread gets its OWN independent copy
// of this stack, with zero synchronization needed, because there is
// nothing to synchronize -- two threads literally cannot see or touch
// each other's error history. See errors_test.c's
// test_two_threads_have_completely_independent_stacks for this proven
// directly, not just asserted.
//
// ─── Why a fixed-size ring, not a dynamically growing list ─────────────────
//
// Error-reporting infrastructure should be MORE robust than the code
// it's protecting, not introduce a new way to fail -- a dynamically
// growing error stack could itself fail to grow (out of memory) at
// exactly the worst possible moment, while you're already in the
// middle of handling a failure. A fixed-size buffer can never fail to
// accept a push; if it's completely full, the OLDEST recorded error is
// simply evicted to make room for the newest one, on the reasoning
// that the most recent errors in an active failure are usually the
// most actionable ones to see first.
//
// ─── Capturing __FILE__/__LINE__ automatically ─────────────────────────────
//
// ERROR_STACK_PUSH(code, ...) is a macro specifically so it can expand
// __FILE__ and __LINE__ at the ACTUAL call site -- a plain function
// call would only ever see error_stack.c's own location, which is
// useless for tracking down where in the calling code something
// actually went wrong.

#define ERROR_STACK_MAX_DEPTH 16
#define ERROR_MESSAGE_MAX_LEN 128

typedef struct {
    int code;
    char message[ERROR_MESSAGE_MAX_LEN];
    const char* file;
    int line;
} ErrorRecord;

// Pushes a formatted error onto the CURRENT THREAD's stack. Prefer the
// ERROR_STACK_PUSH macro below over calling this directly, so
// file/line are captured at the real call site.
void error_stack_push(int code, const char* file, int line, const char* format, ...);

#define ERROR_STACK_PUSH(code, ...) error_stack_push((code), __FILE__, __LINE__, __VA_ARGS__)

// Pops the most recently pushed error (LIFO) into *out. Returns 1 if
// there was one, 0 if the current thread's stack is empty. Calling
// this repeatedly until it returns 0 is how a caller fully drains
// everything that accumulated during a failing call chain.
int error_stack_pop(ErrorRecord* out);

size_t error_stack_count(void);
void error_stack_clear(void);
