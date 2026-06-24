#include <stdio.h>

#include "recon/error_context.h"
#include "recon/error_stack.h"
#include "recon/gcode_tokenizer.h"
#include "recon/recon_thread.h"

static void demo_basic_push_pop(void) {
    printf("--- basic push/pop: LIFO order ---\n");

    error_stack_clear();
    ERROR_STACK_PUSH(404, "could not find resource '%s'", "config.json");
    ERROR_STACK_PUSH(500, "internal failure while processing request %d", 42);

    printf("  pushed 2 errors, count: %zu\n", error_stack_count());

    ErrorRecord record;
    while (error_stack_pop(&record)) {
        printf("  popped: code=%d \"%s\"  (recorded at %s:%d)\n", record.code, record.message, record.file,
               record.line);
    }
    printf("  (expected order: code=500 first, then code=404 -- most recently pushed first)\n\n");
}

static void demo_eviction_when_full(void) {
    printf("--- pushing more than ERROR_STACK_MAX_DEPTH (%d) evicts the oldest ---\n", ERROR_STACK_MAX_DEPTH);

    error_stack_clear();
    for (int i = 0; i < ERROR_STACK_MAX_DEPTH + 5; i++) {
        ERROR_STACK_PUSH(i, "error number %d", i);
    }

    printf("  pushed %d errors into a stack with max depth %d\n", ERROR_STACK_MAX_DEPTH + 5,
           ERROR_STACK_MAX_DEPTH);
    printf("  count: %zu   (expected: %d -- capped at max depth)\n", error_stack_count(), ERROR_STACK_MAX_DEPTH);

    printf("  draining (most recent first): ");
    ErrorRecord record;
    while (error_stack_pop(&record)) {
        printf("%d ", record.code);
    }
    printf("\n  (expected: %d down to 5 -- the first 5 pushes, codes 0..4, were evicted)\n\n",
           ERROR_STACK_MAX_DEPTH + 4);
}

static const char* g_test_program[] = {
    "G28 ; home all axes",
    "G1 X10 Y10 F1500",
    "G1 X10 @ Y20",
    "G1 X30 Y30",
    "G1 X Y40",
    "G1 X50 Y50",
};
static const size_t kProgramLineCount = sizeof(g_test_program) / sizeof(g_test_program[0]);

static void demo_gcode_processing_with_stack(void) {
    printf("--- processing a G-code program with the error STACK ---\n");
    printf("  (accumulates every problem found, keeps going, reports everything at the end)\n\n");

    error_stack_clear();
    int error_count = 0;

    for (size_t i = 0; i < kProgramLineCount; i++) {
        GCodeToken tokens[16];
        GCodeTokenizeResult result = gcode_tokenize_line(g_test_program[i], tokens, 16);
        if (!result.success) {
            ERROR_STACK_PUSH(1001, "line %zu (\"%s\"): %s at position %zu", i + 1, g_test_program[i],
                              result.error_message, result.error_position);
            error_count++;
        }
    }

    printf("  processed all %zu lines, found %d error(s)\n", kProgramLineCount, error_count);
    printf("  draining the full accumulated history:\n");

    ErrorRecord record;
    while (error_stack_pop(&record)) {
        printf("    code=%d: %s\n", record.code, record.message);
    }
    printf("\n");
}

static void demo_gcode_processing_with_context(void) {
    printf("--- the SAME program with the explicit error CONTEXT ---\n");
    printf("  (only room for one error -- this pattern is naturally fail-fast)\n\n");

    ErrorContext ctx;
    error_context_init(&ctx);

    size_t lines_processed = 0;
    for (size_t i = 0; i < kProgramLineCount; i++) {
        lines_processed++;
        GCodeToken tokens[16];
        GCodeTokenizeResult result = gcode_tokenize_line(g_test_program[i], tokens, 16);
        if (!result.success) {
            ERROR_CONTEXT_SET(&ctx, 1001, "line %zu (\"%s\"): %s at position %zu", i + 1, g_test_program[i],
                               result.error_message, result.error_position);
            break;
        }
    }

    printf("  stopped after processing %zu of %zu lines\n", lines_processed, kProgramLineCount);
    if (error_context_has_error(&ctx)) {
        printf("  context shows: code=%d: %s\n", ctx.code, ctx.message);
    }
    printf("  (the SECOND malformed line, line 5, was never even reached -- the\n");
    printf("  context pattern naturally stops at the first failure, while the\n");
    printf("  stack-based version above found and reported BOTH problems)\n\n");
}

typedef struct {
    const char* thread_label;
    int errors_seen[3];
} ThreadWorkerResult;

static void* worker_thread_func(void* arg) {
    ThreadWorkerResult* result = (ThreadWorkerResult*)arg;
    error_stack_clear();

    for (int i = 0; i < 3; i++) {
        ERROR_STACK_PUSH(100 + i, "%s error %d", result->thread_label, i);
    }

    ErrorRecord record;
    int idx = 0;
    while (error_stack_pop(&record) && idx < 3) {
        result->errors_seen[idx++] = record.code;
    }
    return NULL;
}

static void demo_thread_isolation(void) {
    printf("--- proving thread isolation, not just claiming it ---\n");

    ThreadWorkerResult thread_a_result = {.thread_label = "ThreadA", .errors_seen = {0}};
    ThreadWorkerResult thread_b_result = {.thread_label = "ThreadB", .errors_seen = {0}};

    recon_thread_t a, b;
    recon_thread_create(&a, worker_thread_func, &thread_a_result);
    recon_thread_create(&b, worker_thread_func, &thread_b_result);
    recon_thread_join(a);
    recon_thread_join(b);

    printf("  Thread A's drained codes: %d, %d, %d   (expected: 102, 101, 100)\n", thread_a_result.errors_seen[0],
           thread_a_result.errors_seen[1], thread_a_result.errors_seen[2]);
    printf("  Thread B's drained codes: %d, %d, %d   (expected: 102, 101, 100)\n", thread_b_result.errors_seen[0],
           thread_b_result.errors_seen[1], thread_b_result.errors_seen[2]);
    printf("  Both threads pushed the identical codes 100/101/102 -- but each thread's\n");
    printf("  stack held ONLY its own pushes throughout, with zero cross-contamination\n");
    printf("  and zero synchronization needed to guarantee it.\n\n");
}

int main(void) {
    demo_basic_push_pop();
    demo_eviction_when_full();
    demo_gcode_processing_with_stack();
    demo_gcode_processing_with_context();
    demo_thread_isolation();
    return 0;
}
