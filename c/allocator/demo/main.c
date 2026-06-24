#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "recon/arena.h"
#include "recon/pool.h"
#include "recon/recon_time.h"

static int is_aligned(void* ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}

static void demo_arena_basics(void) {
    printf("--- arena: bump allocation over a fixed stack buffer ---\n");

    unsigned char backing[256];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    char* name = (char*)arena_alloc(&arena, 16);
    int* count = (int*)arena_alloc(&arena, sizeof(int));
    double* ratio = (double*)arena_alloc_aligned(&arena, sizeof(double), sizeof(double));

    printf("  allocated a 16-byte buffer, an int, and a double from one 256-byte stack array\n");
    printf("  bytes used: %zu, bytes remaining: %zu\n", arena_bytes_used(&arena),
           arena_bytes_remaining(&arena));
    printf("  double pointer is 8-byte aligned: %s   (expected: yes)\n",
           is_aligned(ratio, sizeof(double)) ? "yes" : "NO");

    *count = 42;
    *ratio = 3.14;
    snprintf(name, 16, "demo");
    printf("  values round-trip correctly: name=\"%s\" count=%d ratio=%.2f\n\n", name, *count, *ratio);
}

static void demo_arena_reset_reuses_memory(void) {
    printf("--- arena: reset frees everything at once, O(1) ---\n");

    unsigned char backing[64];
    Arena arena;
    arena_init(&arena, backing, sizeof(backing));

    void* first_before_reset = arena_alloc(&arena, 8);
    arena_alloc(&arena, 8);
    arena_alloc(&arena, 8);
    printf("  made 3 allocations, bytes used: %zu\n", arena_bytes_used(&arena));

    arena_reset(&arena);
    printf("  after arena_reset(): bytes used: %zu   (expected: 0)\n", arena_bytes_used(&arena));

    void* first_after_reset = arena_alloc(&arena, 8);
    printf("  first allocation after reset reuses the exact same address as before: %s\n\n",
           (first_after_reset == first_before_reset) ? "yes" : "NO");
}

#define DEMO_BLOCK_COUNT 8
#define DEMO_BLOCK_SIZE 32

static void demo_pool_zero_fragmentation(void) {
    printf("--- pool: zero fragmentation, regardless of free order ---\n");

    unsigned char backing[DEMO_BLOCK_COUNT * DEMO_BLOCK_SIZE];
    Pool pool;
    pool_init(&pool, backing, DEMO_BLOCK_SIZE, DEMO_BLOCK_COUNT);

    void* blocks[DEMO_BLOCK_COUNT];
    for (int i = 0; i < DEMO_BLOCK_COUNT; i++) {
        blocks[i] = pool_alloc(&pool);
    }
    printf("  allocated all %d blocks, blocks_free: %zu   (expected: 0)\n", DEMO_BLOCK_COUNT,
           pool_blocks_free(&pool));

    void* should_fail = pool_alloc(&pool);
    printf("  one more allocation while exhausted: %s   (expected: NULL)\n",
           should_fail == NULL ? "NULL" : "non-NULL -- unexpected");

    int free_order[DEMO_BLOCK_COUNT] = {3, 7, 0, 5, 1, 6, 2, 4};
    for (int i = 0; i < DEMO_BLOCK_COUNT; i++) {
        pool_free(&pool, blocks[free_order[i]]);
    }
    printf("  freed all %d blocks in scrambled order, blocks_free: %zu   (expected: %d)\n",
           DEMO_BLOCK_COUNT, pool_blocks_free(&pool), DEMO_BLOCK_COUNT);

    int reallocated = 0;
    for (int i = 0; i < DEMO_BLOCK_COUNT; i++) {
        if (pool_alloc(&pool) != NULL) {
            reallocated++;
        }
    }
    printf("  re-allocated %d of %d blocks successfully   (expected: %d -- a fragmented\n",
           reallocated, DEMO_BLOCK_COUNT, DEMO_BLOCK_COUNT);
    printf("  allocator could fail to satisfy this depending on free order; this one can't)\n\n");
}

typedef struct {
    int x, y, z;
    double value;
} SmallObject;

static void demo_performance_pool_vs_malloc(void) {
    printf("--- performance: malloc/free vs pool, interleaved alloc-use-free ---\n");

    const int kIterations = 2000000;
    const size_t kObjectSize = sizeof(SmallObject);
    double start, end;

    start = recon_monotonic_seconds();
    for (int i = 0; i < kIterations; i++) {
        SmallObject* obj = (SmallObject*)malloc(kObjectSize);
        obj->x = i;
        free(obj);
    }
    end = recon_monotonic_seconds();
    double malloc_seconds = end - start;

    unsigned char pool_backing[sizeof(SmallObject)];
    Pool pool;
    pool_init(&pool, pool_backing, kObjectSize, 1);

    start = recon_monotonic_seconds();
    for (int i = 0; i < kIterations; i++) {
        SmallObject* obj = (SmallObject*)pool_alloc(&pool);
        obj->x = i;
        pool_free(&pool, obj);
    }
    end = recon_monotonic_seconds();
    double pool_seconds = end - start;

    printf("  %d iterations of allocate-use-free, one object at a time:\n", kIterations);
    printf("    malloc/free: %.4f seconds\n", malloc_seconds);
    printf("    pool:        %.4f seconds   (%.1fx faster than malloc/free)\n", pool_seconds,
           malloc_seconds / pool_seconds);
    printf("  Worth knowing: modern malloc implementations (glibc's tcache, here) have a\n");
    printf("  per-thread fast path specifically optimized for exactly this \"alloc, use,\n");
    printf("  immediately free, same size\" pattern -- a custom pool still wins, but not\n");
    printf("  by the dramatic margin a naive mental model of malloc would predict.\n\n");
}

static void demo_performance_arena_vs_malloc(void) {
    printf("--- performance: malloc+N frees vs arena_alloc+ONE reset ---\n");
    printf("  This is the arena's actual structural advantage -- not raw per-allocation\n");
    printf("  speed, but avoiding individual free() calls entirely. An earlier version of\n");
    printf("  this benchmark forced the arena into the SAME shape as the pool comparison\n");
    printf("  above (alloc one, free one, repeat) and found it roughly tied with malloc --\n");
    printf("  which makes sense once you notice that shape never lets the arena skip any\n");
    printf("  frees at all. This version measures what the arena is actually FOR.\n\n");

    const int kBatchSize = 1000;
    const int kBatchCount = 2000;  // 2,000,000 objects total, same as the other benchmark
    const size_t kObjectSize = sizeof(SmallObject);
    double start, end;

    void* ptrs[1000];
    start = recon_monotonic_seconds();
    for (int batch = 0; batch < kBatchCount; batch++) {
        for (int i = 0; i < kBatchSize; i++) {
            SmallObject* obj = (SmallObject*)malloc(kObjectSize);
            obj->x = i;
            ptrs[i] = obj;
        }
        for (int i = 0; i < kBatchSize; i++) {
            free(ptrs[i]);
        }
    }
    end = recon_monotonic_seconds();
    double malloc_seconds = end - start;

    unsigned char arena_backing[1000 * sizeof(SmallObject)];
    Arena arena;
    arena_init(&arena, arena_backing, sizeof(arena_backing));

    start = recon_monotonic_seconds();
    for (int batch = 0; batch < kBatchCount; batch++) {
        for (int i = 0; i < kBatchSize; i++) {
            SmallObject* obj = (SmallObject*)arena_alloc(&arena, kObjectSize);
            obj->x = i;
        }
        arena_reset(&arena);  // ONE call, instead of kBatchSize individual free() calls
    }
    end = recon_monotonic_seconds();
    double arena_seconds = end - start;

    printf("  %d batches of %d objects each (%d total), freed per-batch:\n", kBatchCount, kBatchSize,
           kBatchCount * kBatchSize);
    printf("    malloc + %d individual free() calls per batch: %.4f seconds\n", kBatchSize, malloc_seconds);
    printf("    arena_alloc + ONE arena_reset() call per batch: %.4f seconds   (%.1fx faster)\n",
           arena_seconds, malloc_seconds / arena_seconds);
    printf("\n  Honest finding: this margin is real but modest, not the dramatic win the\n");
    printf("  \"N frees vs 1 reset\" theory alone would predict. The reason: glibc's malloc\n");
    printf("  has used a per-thread tcache since 2.26 -- a fast-path linked freelist per\n");
    printf("  size class, which is THE SAME trick this folder's pool allocator uses. Modern\n");
    printf("  free() is nearly as cheap as a list push, so avoiding individual frees just\n");
    printf("  doesn't cost what it used to.\n");
    printf("  The durable, real reasons to still write a custom allocator today don't\n");
    printf("  depend on malloc being slow at all: deterministic worst-case latency (no\n");
    printf("  general-purpose allocator can promise this, which matters for real-time\n");
    printf("  code), zero per-allocation metadata overhead (a pool's only \"bookkeeping\"\n");
    printf("  is the intrusive pointer reused from user data, vs malloc's per-chunk\n");
    printf("  header), placing memory in a specific region (DMA-capable memory, a memory-\n");
    printf("  mapped region), and bare-metal targets where malloc may not exist at all.\n\n");
}

int main(void) {
    demo_arena_basics();
    demo_arena_reset_reuses_memory();
    demo_pool_zero_fragmentation();
    demo_performance_pool_vs_malloc();
    demo_performance_arena_vs_malloc();
    return 0;
}
