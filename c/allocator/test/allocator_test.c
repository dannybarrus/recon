#include "unity.h"
#include "recon/arena.h"
#include "recon/pool.h"

#include <stdint.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static int is_aligned(void* ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}

void test_arena_first_allocation_starts_at_buffer_start(void) {
    unsigned char backing[64];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    void* ptr = arena_alloc(&arena, 8);
    TEST_ASSERT_EQUAL_PTR(backing, ptr);
}

void test_arena_sequential_allocations_do_not_overlap(void) {
    unsigned char backing[64];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    char* a = (char*)arena_alloc(&arena, 8);
    char* b = (char*)arena_alloc(&arena, 8);

    memset(a, 0xAA, 8);
    memset(b, 0xBB, 8);

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xAA, (unsigned char)a[i]);
    }
}

void test_arena_returns_null_when_out_of_space(void) {
    unsigned char backing[16];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    void* first = arena_alloc(&arena, 8);
    void* second = arena_alloc(&arena, 8);
    void* third = arena_alloc(&arena, 8);

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NULL(third);
}

void test_arena_respects_requested_alignment(void) {
    unsigned char backing[128];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    arena_alloc(&arena, 3);

    void* aligned16 = arena_alloc_aligned(&arena, 16, 16);
    TEST_ASSERT_TRUE(is_aligned(aligned16, 16));

    void* aligned8 = arena_alloc_aligned(&arena, 8, 8);
    TEST_ASSERT_TRUE(is_aligned(aligned8, 8));
}

void test_arena_reset_reuses_the_same_starting_address(void) {
    unsigned char backing[64];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    void* first = arena_alloc(&arena, 8);
    arena_alloc(&arena, 8);
    arena_alloc(&arena, 8);

    arena_reset(&arena);
    TEST_ASSERT_EQUAL_UINT(0, arena_bytes_used(&arena));

    void* after_reset = arena_alloc(&arena, 8);
    TEST_ASSERT_EQUAL_PTR(first, after_reset);
}

void test_arena_bytes_used_and_remaining_track_correctly(void) {
    unsigned char backing[100];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    arena_alloc(&arena, 10);
    TEST_ASSERT_EQUAL_UINT(10, arena_bytes_used(&arena));
    TEST_ASSERT_EQUAL_UINT(90, arena_bytes_remaining(&arena));
}

#define TEST_POOL_BLOCK_COUNT 8
#define TEST_POOL_BLOCK_SIZE 32

void test_pool_allocates_distinct_non_overlapping_blocks(void) {
    unsigned char backing[TEST_POOL_BLOCK_COUNT * TEST_POOL_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, TEST_POOL_BLOCK_SIZE, TEST_POOL_BLOCK_COUNT);

    void* blocks[TEST_POOL_BLOCK_COUNT];
    for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
        blocks[i] = pool_alloc(&pool);
        TEST_ASSERT_NOT_NULL(blocks[i]);
    }

    for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
        for (int j = i + 1; j < TEST_POOL_BLOCK_COUNT; j++) {
            TEST_ASSERT_NOT_EQUAL(blocks[i], blocks[j]);
        }
    }
}

void test_pool_returns_null_when_exhausted(void) {
    unsigned char backing[TEST_POOL_BLOCK_COUNT * TEST_POOL_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, TEST_POOL_BLOCK_SIZE, TEST_POOL_BLOCK_COUNT);

    for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
        TEST_ASSERT_NOT_NULL(pool_alloc(&pool));
    }
    TEST_ASSERT_NULL(pool_alloc(&pool));
}

void test_pool_free_then_alloc_reuses_the_block(void) {
    unsigned char backing[TEST_POOL_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, TEST_POOL_BLOCK_SIZE, 1);

    void* first = pool_alloc(&pool);
    pool_free(&pool, first);
    void* second = pool_alloc(&pool);

    TEST_ASSERT_EQUAL_PTR(first, second);
}

void test_pool_blocks_free_tracks_correctly(void) {
    unsigned char backing[TEST_POOL_BLOCK_COUNT * TEST_POOL_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, TEST_POOL_BLOCK_SIZE, TEST_POOL_BLOCK_COUNT);

    TEST_ASSERT_EQUAL_UINT(TEST_POOL_BLOCK_COUNT, pool_blocks_free(&pool));

    void* a = pool_alloc(&pool);
    void* b = pool_alloc(&pool);
    TEST_ASSERT_EQUAL_UINT(TEST_POOL_BLOCK_COUNT - 2, pool_blocks_free(&pool));

    pool_free(&pool, a);
    TEST_ASSERT_EQUAL_UINT(TEST_POOL_BLOCK_COUNT - 1, pool_blocks_free(&pool));

    pool_free(&pool, b);
    TEST_ASSERT_EQUAL_UINT(TEST_POOL_BLOCK_COUNT, pool_blocks_free(&pool));
}

void test_pool_never_fragments_regardless_of_free_order(void) {
    unsigned char backing[TEST_POOL_BLOCK_COUNT * TEST_POOL_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, TEST_POOL_BLOCK_SIZE, TEST_POOL_BLOCK_COUNT);

    void* blocks[TEST_POOL_BLOCK_COUNT];
    for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
        blocks[i] = pool_alloc(&pool);
    }

    int free_order[TEST_POOL_BLOCK_COUNT] = {3, 7, 0, 5, 1, 6, 2, 4};
    for (int cycle = 0; cycle < 5; cycle++) {
        for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
            pool_free(&pool, blocks[free_order[i]]);
        }
        TEST_ASSERT_EQUAL_UINT(TEST_POOL_BLOCK_COUNT, pool_blocks_free(&pool));

        for (int i = 0; i < TEST_POOL_BLOCK_COUNT; i++) {
            blocks[i] = pool_alloc(&pool);
            TEST_ASSERT_NOT_NULL(blocks[i]);
        }
        TEST_ASSERT_EQUAL_UINT(0, pool_blocks_free(&pool));
    }
}

void test_pool_data_survives_until_freed(void) {
    // Item must be at least sizeof(void*) -- see pool.h's documented
    // precondition. A single int alone (4 bytes on most platforms) is
    // smaller than a 64-bit pointer (8 bytes), which is exactly the
    // mistake the assertion in pool_init() exists to catch loudly
    // rather than silently corrupting memory.
    typedef struct {
        int value;
        int padding;
    } Item;

    unsigned char backing[4 * sizeof(Item)];
    Pool pool;
    pool_init(&pool, backing, sizeof(Item), 4);

    Item* a = (Item*)pool_alloc(&pool);
    Item* b = (Item*)pool_alloc(&pool);
    a->value = 111;
    b->value = 222;

    TEST_ASSERT_EQUAL_INT(111, a->value);
    TEST_ASSERT_EQUAL_INT(222, b->value);
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_arena_first_allocation_starts_at_buffer_start", test_arena_first_allocation_starts_at_buffer_start},
    {"test_arena_sequential_allocations_do_not_overlap", test_arena_sequential_allocations_do_not_overlap},
    {"test_arena_returns_null_when_out_of_space", test_arena_returns_null_when_out_of_space},
    {"test_arena_respects_requested_alignment", test_arena_respects_requested_alignment},
    {"test_arena_reset_reuses_the_same_starting_address", test_arena_reset_reuses_the_same_starting_address},
    {"test_arena_bytes_used_and_remaining_track_correctly", test_arena_bytes_used_and_remaining_track_correctly},
    {"test_pool_allocates_distinct_non_overlapping_blocks", test_pool_allocates_distinct_non_overlapping_blocks},
    {"test_pool_returns_null_when_exhausted", test_pool_returns_null_when_exhausted},
    {"test_pool_free_then_alloc_reuses_the_block", test_pool_free_then_alloc_reuses_the_block},
    {"test_pool_blocks_free_tracks_correctly", test_pool_blocks_free_tracks_correctly},
    {"test_pool_never_fragments_regardless_of_free_order", test_pool_never_fragments_regardless_of_free_order},
    {"test_pool_data_survives_until_freed", test_pool_data_survives_until_freed},
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
