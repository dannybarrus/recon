#include <stdio.h>
#include <stdint.h>

#include "recon/ringbuffer.h"
#include "recon/recon_thread.h"
#include "recon/recon_time.h"

RING_BUFFER_DEFINE(int, IntQueue, 1024)

typedef struct {
    int x;
    int y;
} Point;
RING_BUFFER_DEFINE(Point, PointQueue, 16)

static void demo_basic_usage(void) {
    printf("--- basic usage: FIFO order, empty/full boundaries ---\n");

    IntQueue_t q;
    IntQueue_init(&q);

    printf("  is_empty() on a fresh queue: %s   (expected: true)\n",
           IntQueue_is_empty(&q) ? "true" : "false");

    for (int i = 1; i <= 5; i++) {
        IntQueue_push(&q, i);
    }
    printf("  pushed 1..5, size() = %zu   (expected: 5)\n", IntQueue_size(&q));

    for (int i = 0; i < 5; i++) {
        int value;
        IntQueue_pop(&q, &value);
        printf("  popped: %d   (expected: %d)\n", value, i + 1);
    }

    printf("  is_empty() after draining: %s   (expected: true)\n\n",
           IntQueue_is_empty(&q) ? "true" : "false");
}

static void demo_full_buffer(void) {
    printf("--- filling the buffer completely ---\n");

    IntQueue_t q;
    IntQueue_init(&q);

    int pushed = 0;
    for (int i = 0; i < 2000; i++) {
        if (IntQueue_push(&q, i)) {
            pushed++;
        } else {
            break;
        }
    }
    printf("  pushed %d values before push() returned false   (expected: 1024)\n", pushed);
    printf("  is_full(): %s   (expected: true)\n\n", IntQueue_is_full(&q) ? "true" : "false");
}

static void demo_type_safety(void) {
    printf("--- type safety: a second, independent instantiation ---\n");

    PointQueue_t pq;
    PointQueue_init(&pq);

    PointQueue_push(&pq, (Point){.x = 1, .y = 2});
    PointQueue_push(&pq, (Point){.x = 3, .y = 4});

    Point p;
    PointQueue_pop(&pq, &p);
    printf("  popped Point: (%d, %d)   (expected: (1, 2))\n", p.x, p.y);
    printf("  IntQueue_t and PointQueue_t are unrelated types -- passing a\n");
    printf("  Point where IntQueue_push expects an int is a COMPILE error,\n");
    printf("  not a runtime surprise the way a void*-based queue would allow.\n\n");
}

#define STRESS_TEST_ITEM_COUNT 5000000

typedef struct {
    IntQueue_t* queue;
    int item_count;
} ProducerArgs;

typedef struct {
    IntQueue_t* queue;
    int item_count;
    long long checksum;
    int out_of_order_count;
} ConsumerArgs;

static void* producer_thread(void* arg) {
    ProducerArgs* args = (ProducerArgs*)arg;
    for (int i = 0; i < args->item_count; i++) {
        int spins = 0;
        while (!IntQueue_push(args->queue, i)) {
            // Spin a short while first -- on real multi-core hardware,
            // the consumer is likely to make room within a handful of
            // iterations, and yielding immediately would cost more in
            // scheduler overhead than it saves. Past that, yield: this
            // is a pure scheduling hint, not a lock, so it does not
            // compromise the lock-free property -- it just stops this
            // thread from monopolizing a single CPU core while waiting,
            // which matters enormously on a machine with few cores.
            if (++spins > 1000) {
                recon_thread_yield();
                spins = 0;
            }
        }
    }
    return NULL;
}

static void* consumer_thread(void* arg) {
    ConsumerArgs* args = (ConsumerArgs*)arg;
    int expected_next = 0;
    for (int i = 0; i < args->item_count; i++) {
        int value;
        int spins = 0;
        while (!IntQueue_pop(args->queue, &value)) {
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

static void demo_real_concurrent_stress_test(void) {
    printf("--- real two-thread producer/consumer stress test ---\n");
    printf("  %d items pushed by one thread, popped by another, no locks anywhere.\n",
           STRESS_TEST_ITEM_COUNT);

    IntQueue_t q;
    IntQueue_init(&q);

    ProducerArgs producer_args = {.queue = &q, .item_count = STRESS_TEST_ITEM_COUNT};
    ConsumerArgs consumer_args = {.queue = &q, .item_count = STRESS_TEST_ITEM_COUNT,
                                   .checksum = 0, .out_of_order_count = 0};

    double start = recon_monotonic_seconds();

    recon_thread_t producer, consumer;
    recon_thread_create(&producer, producer_thread, &producer_args);
    recon_thread_create(&consumer, consumer_thread, &consumer_args);
    recon_thread_join(producer);
    recon_thread_join(consumer);

    double end = recon_monotonic_seconds();
    double elapsed_seconds = end - start;

    long long expected_checksum = 0;
    for (long long i = 0; i < STRESS_TEST_ITEM_COUNT; i++) {
        expected_checksum += i;
    }

    printf("  elapsed: %.3f seconds   (%.1f million items/sec)\n",
           elapsed_seconds, (STRESS_TEST_ITEM_COUNT / elapsed_seconds) / 1e6);
    printf("  checksum: %lld   (expected: %lld)   %s\n",
           consumer_args.checksum, expected_checksum,
           (consumer_args.checksum == expected_checksum) ? "MATCH" : "*** MISMATCH ***");
    printf("  out-of-order deliveries: %d   (expected: 0 -- SPSC preserves FIFO order)\n",
           consumer_args.out_of_order_count);
    printf("  Note: throughput depends heavily on having genuine multi-core\n");
    printf("  hardware available. On a single-core machine, the producer and\n");
    printf("  consumer can never truly run simultaneously -- the OS has to keep\n");
    printf("  preempting one to let the other proceed, which looks nothing like\n");
    printf("  this queue's real-world throughput on multi-core hardware, where\n");
    printf("  both threads genuinely run in parallel.\n\n");
}

int main(void) {
    demo_basic_usage();
    demo_full_buffer();
    demo_type_safety();
    demo_real_concurrent_stress_test();
    return 0;
}
