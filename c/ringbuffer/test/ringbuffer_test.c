#include "unity.h"
#include "recon/ringbuffer.h"
#include "recon/recon_thread.h"

RING_BUFFER_DEFINE(int, TestQueue, 16)

void setUp(void) {}
void tearDown(void) {}

void test_new_queue_is_empty(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    TEST_ASSERT_TRUE(TestQueue_is_empty(&q));
    TEST_ASSERT_EQUAL_UINT(0, TestQueue_size(&q));
}

void test_push_then_pop_returns_same_value(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    TEST_ASSERT_TRUE(TestQueue_push(&q, 42));

    int value;
    TEST_ASSERT_TRUE(TestQueue_pop(&q, &value));
    TEST_ASSERT_EQUAL_INT(42, value);
}

void test_preserves_fifo_order(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    for (int i = 1; i <= 5; i++) {
        TestQueue_push(&q, i);
    }
    for (int i = 1; i <= 5; i++) {
        int value;
        TestQueue_pop(&q, &value);
        TEST_ASSERT_EQUAL_INT(i, value);
    }
}

void test_pop_on_empty_queue_returns_false(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    int value;
    TEST_ASSERT_FALSE(TestQueue_pop(&q, &value));
}

void test_push_until_full_then_fails(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    for (int i = 0; i < 16; i++) {
        TEST_ASSERT_TRUE(TestQueue_push(&q, i));
    }
    TEST_ASSERT_TRUE(TestQueue_is_full(&q));
    TEST_ASSERT_FALSE(TestQueue_push(&q, 999));
}

void test_size_tracks_pushes_and_pops(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    TestQueue_push(&q, 1);
    TestQueue_push(&q, 2);
    TEST_ASSERT_EQUAL_UINT(2, TestQueue_size(&q));

    int value;
    TestQueue_pop(&q, &value);
    TEST_ASSERT_EQUAL_UINT(1, TestQueue_size(&q));
}

void test_capacity_returns_compile_time_value(void) {
    TestQueue_t q;
    TestQueue_init(&q);
    TEST_ASSERT_EQUAL_UINT(16, TestQueue_capacity(&q));
}

void test_wraps_around_correctly_past_capacity_boundary(void) {
    TestQueue_t q;
    TestQueue_init(&q);

    for (int cycle = 0; cycle < 100; cycle++) {
        TEST_ASSERT_TRUE(TestQueue_push(&q, cycle));
        int value;
        TEST_ASSERT_TRUE(TestQueue_pop(&q, &value));
        TEST_ASSERT_EQUAL_INT(cycle, value);
    }
    TEST_ASSERT_TRUE(TestQueue_is_empty(&q));
}

#define TEST_STRESS_ITEM_COUNT 200000

typedef struct {
    TestQueue_t* queue;
    int item_count;
} StressProducerArgs;

typedef struct {
    TestQueue_t* queue;
    int item_count;
    long long checksum;
    int out_of_order_count;
} StressConsumerArgs;

static void* stress_producer(void* arg) {
    StressProducerArgs* args = (StressProducerArgs*)arg;
    for (int i = 0; i < args->item_count; i++) {
        int spins = 0;
        while (!TestQueue_push(args->queue, i)) {
            if (++spins > 1000) {
                recon_thread_yield();
                spins = 0;
            }
        }
    }
    return NULL;
}

static void* stress_consumer(void* arg) {
    StressConsumerArgs* args = (StressConsumerArgs*)arg;
    int expected_next = 0;
    for (int i = 0; i < args->item_count; i++) {
        int value;
        int spins = 0;
        while (!TestQueue_pop(args->queue, &value)) {
            if (++spins > 1000) {
                recon_thread_yield();
                spins = 0;
            }
        }
        args->checksum += value;
        if (value != expected_next) {
            args->out_of_order_count++;
        }
        expected_next++;
    }
    return NULL;
}

void test_concurrent_producer_consumer_delivers_every_item_in_order(void) {
    TestQueue_t q;
    TestQueue_init(&q);

    StressProducerArgs producer_args = {.queue = &q, .item_count = TEST_STRESS_ITEM_COUNT};
    StressConsumerArgs consumer_args = {
        .queue = &q, .item_count = TEST_STRESS_ITEM_COUNT, .checksum = 0, .out_of_order_count = 0};

    recon_thread_t producer, consumer;
    recon_thread_create(&producer, stress_producer, &producer_args);
    recon_thread_create(&consumer, stress_consumer, &consumer_args);
    recon_thread_join(producer);
    recon_thread_join(consumer);

    long long expected_checksum = 0;
    for (long long i = 0; i < TEST_STRESS_ITEM_COUNT; i++) {
        expected_checksum += i;
    }

    TEST_ASSERT_EQUAL_INT(0, consumer_args.out_of_order_count);
    TEST_ASSERT_TRUE(consumer_args.checksum == expected_checksum);
}

#include <string.h>

// A table of every test, mapping name to function. This is the single
// source of truth both for running everything (the default, no
// command-line argument) and for running just one named test (passed
// as argv[1]) -- which is what lets CMake register each test as its
// own separate ctest entry, the same outcome gtest_discover_tests()
// gives the C++ and Java test suites elsewhere in this repo. See
// cmake/UnityDiscoverTests.cmake for how the names in this table get
// turned into individual add_test() registrations.
typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_new_queue_is_empty", test_new_queue_is_empty},
    {"test_push_then_pop_returns_same_value", test_push_then_pop_returns_same_value},
    {"test_preserves_fifo_order", test_preserves_fifo_order},
    {"test_pop_on_empty_queue_returns_false", test_pop_on_empty_queue_returns_false},
    {"test_push_until_full_then_fails", test_push_until_full_then_fails},
    {"test_size_tracks_pushes_and_pops", test_size_tracks_pushes_and_pops},
    {"test_capacity_returns_compile_time_value", test_capacity_returns_compile_time_value},
    {"test_wraps_around_correctly_past_capacity_boundary", test_wraps_around_correctly_past_capacity_boundary},
    {"test_concurrent_producer_consumer_delivers_every_item_in_order",
     test_concurrent_producer_consumer_delivers_every_item_in_order},
};
#define TEST_CASE_COUNT (sizeof(test_cases) / sizeof(test_cases[0]))

// Calling UnityDefaultTestRun() directly (rather than the RUN_TEST()
// macro) is deliberate: RUN_TEST stringizes its argument via the
// preprocessor (`#func`), so RUN_TEST(test_cases[i].func) would print
// the literal text "test_cases[i].func" as the test name in Unity's
// own console output, not the real underlying test name. Calling
// UnityDefaultTestRun directly with test_cases[i].name as the name
// string keeps Unity's own output correctly labeled while still
// dispatching through the table.
int main(int argc, char** argv) {
    UNITY_BEGIN();

    if (argc > 1) {
        // Run exactly one named test -- this is the path CMake's
        // generated ctest entries actually invoke.
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
        // No argument -- run everything, exactly like before. Running
        // this binary directly with no arguments is unaffected by any
        // of this; it behaves exactly as it always has.
        for (size_t i = 0; i < TEST_CASE_COUNT; i++) {
            UnityDefaultTestRun(test_cases[i].func, test_cases[i].name, __LINE__);
        }
    }

    return UNITY_END();
}
