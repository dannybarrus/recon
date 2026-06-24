#include "recon/error_context.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void error_context_init(ErrorContext* ctx) {
    ctx->has_error = 0;
    ctx->code = 0;
    ctx->message[0] = '\0';
    ctx->file = NULL;
    ctx->line = 0;
}

void error_context_set(ErrorContext* ctx, int code, const char* file, int line, const char* format, ...) {
    ctx->has_error = 1;
    ctx->code = code;
    ctx->file = file;
    ctx->line = line;

    va_list args;
    va_start(args, format);
    vsnprintf(ctx->message, sizeof(ctx->message), format, args);
    va_end(args);
}

int error_context_has_error(const ErrorContext* ctx) {
    return ctx->has_error;
}

void error_context_clear(ErrorContext* ctx) {
    error_context_init(ctx);
}
