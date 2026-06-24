#pragma once

#include <stddef.h>

#include "recon/error_stack.h"

// The alternative architecture to error_stack.h's thread-local
// approach: an explicit context struct the CALLER owns and passes a
// pointer to into every API call. Any function that can fail writes
// its error details into THAT struct, rather than into hidden global
// (even if thread-local) state.
//
// ─── The actual tradeoff against the thread-local stack ───────────────────
//
// What this gains: zero reliance on thread-local storage at all (some
// very minimal embedded targets -- bare-metal, no RTOS -- may not have
// TLS support in the first place), fully explicit ownership (there is
// no hidden global to forget to clear between uses, which matters a
// lot for writing clean, isolated tests -- every test here gets its
// own fresh ErrorContext on the stack, with nothing to reset), and a
// context that's trivially passable across boundaries a thread-local
// can't cross at all, like into a different thread that needs to see
// the SAME error state, or serialized out for inspection elsewhere.
//
// What this gives up: there's only room for ONE error at a time, not
// a history. A failing call chain doesn't accumulate -- each new
// error simply overwrites whatever was there before. If you need to
// know about every problem found while processing many independent
// units of work (see the demo's multi-line G-code processing,
// compared against the same scenario handled with the stack-based
// API instead), the thread-local stack's history is what genuinely
// gets you that; this does not, by design.
//
// Real systems use both, for different reasons in different places --
// neither is strictly better, which is the actual point of building
// both here side by side rather than picking one.

typedef struct {
    int has_error;
    int code;
    char message[ERROR_MESSAGE_MAX_LEN];
    const char* file;
    int line;
} ErrorContext;

void error_context_init(ErrorContext* ctx);

void error_context_set(ErrorContext* ctx, int code, const char* file, int line, const char* format, ...);

#define ERROR_CONTEXT_SET(ctx, code, ...) error_context_set((ctx), (code), __FILE__, __LINE__, __VA_ARGS__)

int error_context_has_error(const ErrorContext* ctx);
void error_context_clear(ErrorContext* ctx);
