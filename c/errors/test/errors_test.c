#include "unity.h"
#include "recon/error_context.h"
#include "recon/error_stack.h"
#include "recon/recon_thread.h"

#include <string.h>

void setUp(void) {
    error_stack_clear();
}
void tearDown(void) {}

void test_push_then_pop_returns_same_values(void) {
    ERROR_STACK_PUSH(404, "not found: %s", "thing");

    ErrorRecord record;
    int got = error_stack_pop(&record);

    TEST_ASSERT_EQUAL_INT(1, got);
    TEST_ASSERT_EQUAL_INT(404, record.code);
    TEST_ASSERT_EQUAL_STRING("not found: thing", record.message);
}

void test_pop_on_empty_stack_returns_zero(void) {
    ErrorRecord record;
    int got = error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(0, got);
}

void test_pop_order_is_lifo(void) {
    ERROR_STACK_PUSH(1, "first");
    ERROR_STACK_PUSH(2, "second");
    ERROR_STACK_PUSH(3, "third");

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(3, record.code);
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(2, record.code);
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(1, record.code);
}

void test_count_tracks_pushes_and_pops(void) {
    TEST_ASSERT_EQUAL_UINT(0, error_stack_count());
    ERROR_STACK_PUSH(1, "a");
    ERROR_STACK_PUSH(2, "b");
    TEST_ASSERT_EQUAL_UINT(2, error_stack_count());

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_UINT(1, error_stack_count());
}

void test_clear_empties_the_stack(void) {
    ERROR_STACK_PUSH(1, "a");
    ERROR_STACK_PUSH(2, "b");
    error_stack_clear();
    TEST_ASSERT_EQUAL_UINT(0, error_stack_count());
}

void test_exceeding_max_depth_evicts_oldest_not_newest(void) {
    for (int i = 0; i < ERROR_STACK_MAX_DEPTH + 3; i++) {
        ERROR_STACK_PUSH(i, "error %d", i);
    }

    TEST_ASSERT_EQUAL_UINT(ERROR_STACK_MAX_DEPTH, error_stack_count());

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(ERROR_STACK_MAX_DEPTH + 2, record.code);

    int last_code = -1;
    while (error_stack_pop(&record)) {
        last_code = record.code;
    }
    TEST_ASSERT_EQUAL_INT(3, last_code);
}

void test_message_formatting_supports_multiple_arguments(void) {
    ERROR_STACK_PUSH(1, "value=%d name=%s ratio=%.2f", 42, "test", 3.14);

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_STRING("value=42 name=test ratio=3.14", record.message);
}

void test_message_longer_than_buffer_is_truncated_not_overflowed(void) {
    ERROR_STACK_PUSH(1, "%s", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                              "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_TRUE(strlen(record.message) < ERROR_MESSAGE_MAX_LEN);
}

void test_file_and_line_are_captured_at_call_site(void) {
    int expected_line = __LINE__ + 1;
    ERROR_STACK_PUSH(1, "test");

    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(expected_line, record.line);
    TEST_ASSERT_EQUAL_STRING(__FILE__, record.file);
}

void test_context_starts_with_no_error(void) {
    ErrorContext ctx;
    error_context_init(&ctx);
    TEST_ASSERT_EQUAL_INT(0, error_context_has_error(&ctx));
}

void test_context_set_records_the_error(void) {
    ErrorContext ctx;
    error_context_init(&ctx);

    ERROR_CONTEXT_SET(&ctx, 500, "failure: %s", "reason");

    TEST_ASSERT_EQUAL_INT(1, error_context_has_error(&ctx));
    TEST_ASSERT_EQUAL_INT(500, ctx.code);
    TEST_ASSERT_EQUAL_STRING("failure: reason", ctx.message);
}

void test_context_second_set_overwrites_the_first(void) {
    ErrorContext ctx;
    error_context_init(&ctx);

    ERROR_CONTEXT_SET(&ctx, 1, "first");
    ERROR_CONTEXT_SET(&ctx, 2, "second");

    TEST_ASSERT_EQUAL_INT(2, ctx.code);
    TEST_ASSERT_EQUAL_STRING("second", ctx.message);
}

void test_context_clear_resets_to_no_error(void) {
    ErrorContext ctx;
    error_context_init(&ctx);
    ERROR_CONTEXT_SET(&ctx, 1, "something");

    error_context_clear(&ctx);

    TEST_ASSERT_EQUAL_INT(0, error_context_has_error(&ctx));
}

typedef struct {
    const char* label;
    int codes_seen[3];
} WorkerResult;

static void* isolated_worker(void* arg) {
    WorkerResult* result = (WorkerResult*)arg;
    error_stack_clear();

    for (int i = 0; i < 3; i++) {
        ERROR_STACK_PUSH(100 + i, "%s-%d", result->label, i);
    }

    ErrorRecord record;
    int idx = 0;
    while (error_stack_pop(&record) && idx < 3) {
        result->codes_seen[idx++] = record.code;
    }
    return NULL;
}

void test_two_threads_have_completely_independent_stacks(void) {
    WorkerResult a = {.label = "A", .codes_seen = {0}};
    WorkerResult b = {.label = "B", .codes_seen = {0}};

    recon_thread_t thread_a, thread_b;
    recon_thread_create(&thread_a, isolated_worker, &a);
    recon_thread_create(&thread_b, isolated_worker, &b);
    recon_thread_join(thread_a);
    recon_thread_join(thread_b);

    TEST_ASSERT_EQUAL_INT(102, a.codes_seen[0]);
    TEST_ASSERT_EQUAL_INT(101, a.codes_seen[1]);
    TEST_ASSERT_EQUAL_INT(100, a.codes_seen[2]);

    TEST_ASSERT_EQUAL_INT(102, b.codes_seen[0]);
    TEST_ASSERT_EQUAL_INT(101, b.codes_seen[1]);
    TEST_ASSERT_EQUAL_INT(100, b.codes_seen[2]);
}

void test_main_thread_stack_unaffected_by_worker_threads(void) {
    ERROR_STACK_PUSH(999, "main thread marker");

    WorkerResult worker = {.label = "Worker", .codes_seen = {0}};
    recon_thread_t thread;
    recon_thread_create(&thread, isolated_worker, &worker);
    recon_thread_join(thread);

    TEST_ASSERT_EQUAL_UINT(1, error_stack_count());
    ErrorRecord record;
    error_stack_pop(&record);
    TEST_ASSERT_EQUAL_INT(999, record.code);
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_push_then_pop_returns_same_values", test_push_then_pop_returns_same_values},
    {"test_pop_on_empty_stack_returns_zero", test_pop_on_empty_stack_returns_zero},
    {"test_pop_order_is_lifo", test_pop_order_is_lifo},
    {"test_count_tracks_pushes_and_pops", test_count_tracks_pushes_and_pops},
    {"test_clear_empties_the_stack", test_clear_empties_the_stack},
    {"test_exceeding_max_depth_evicts_oldest_not_newest", test_exceeding_max_depth_evicts_oldest_not_newest},
    {"test_message_formatting_supports_multiple_arguments", test_message_formatting_supports_multiple_arguments},
    {"test_message_longer_than_buffer_is_truncated_not_overflowed",
     test_message_longer_than_buffer_is_truncated_not_overflowed},
    {"test_file_and_line_are_captured_at_call_site", test_file_and_line_are_captured_at_call_site},
    {"test_context_starts_with_no_error", test_context_starts_with_no_error},
    {"test_context_set_records_the_error", test_context_set_records_the_error},
    {"test_context_second_set_overwrites_the_first", test_context_second_set_overwrites_the_first},
    {"test_context_clear_resets_to_no_error", test_context_clear_resets_to_no_error},
    {"test_two_threads_have_completely_independent_stacks", test_two_threads_have_completely_independent_stacks},
    {"test_main_thread_stack_unaffected_by_worker_threads", test_main_thread_stack_unaffected_by_worker_threads},
};
#define TEST_CASE_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

int main(int argc, char** argv) {
    UNITY_BEGIN();

    if (argc > 1) {
        int found = 0;
        for (size_t i = 0; i < TEST_CASE_COUNT; i++) {
            if (strcmp(argv[1], test_cases[i].name) == 0) {
                UnityDefaultTestRun(test_cases[i].func, test_cases[i].name, __LINE__);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Unknown test name: %s\n", argv[1]);
            return 1;
        }
    } else {
        for (size_t i = 0; i < TEST_CASE_COUNT; i++) {
            UnityDefaultTestRun(test_cases[i].func, test_cases[i].name, __LINE__);
        }
    }

    return UNITY_END();
}
