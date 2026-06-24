#include "recon/error_stack.h"

#include <stdarg.h>
#include <stdio.h>

typedef struct {
    ErrorRecord records[ERROR_STACK_MAX_DEPTH];
    size_t count;
    size_t next_write_index;
} ErrorStackState;

static _Thread_local ErrorStackState g_error_stack = {0};

void error_stack_push(int code, const char* file, int line, const char* format, ...) {
    ErrorRecord* record = &g_error_stack.records[g_error_stack.next_write_index];

    record->code = code;
    record->file = file;
    record->line = line;

    va_list args;
    va_start(args, format);
    vsnprintf(record->message, sizeof(record->message), format, args);
    va_end(args);

    g_error_stack.next_write_index = (g_error_stack.next_write_index + 1) % ERROR_STACK_MAX_DEPTH;
    if (g_error_stack.count < ERROR_STACK_MAX_DEPTH) {
        g_error_stack.count++;
    }
}

int error_stack_pop(ErrorRecord* out) {
    if (g_error_stack.count == 0) {
        return 0;
    }

    size_t top_index =
        (g_error_stack.next_write_index + ERROR_STACK_MAX_DEPTH - 1) % ERROR_STACK_MAX_DEPTH;
    *out = g_error_stack.records[top_index];

    g_error_stack.next_write_index = top_index;
    g_error_stack.count--;
    return 1;
}

size_t error_stack_count(void) {
    return g_error_stack.count;
}

void error_stack_clear(void) {
    g_error_stack.count = 0;
    g_error_stack.next_write_index = 0;
}
