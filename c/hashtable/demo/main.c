#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recon/intern_table.h"
#include "recon/recon_time.h"

static int is_aligned(void* ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}

static void demo_basic_interning(void) {
    printf("--- basic interning: same content always returns the same pointer ---\n");
    printf("  (a config parser repeatedly seeing the same key names is the real\n");
    printf("  scenario this serves -- \"name\", \"value\", \"type\" showing up thousands\n");
    printf("  of times across many entries, each time comparable by pointer instead\n");
    printf("  of strcmp)\n\n");

    unsigned char slots_backing[1024 * sizeof(InternSlot) + 64];
    unsigned char string_backing[4096];
    InternTable table;
    intern_table_init(&table, slots_backing, 1024, string_backing, sizeof(string_backing));

    const char* name1 = intern_table_intern_cstr(&table, "name");
    const char* value1 = intern_table_intern_cstr(&table, "value");
    const char* name2 = intern_table_intern_cstr(&table, "name");

    printf("  intern(\"name\") first call:  %p\n", (void*)name1);
    printf("  intern(\"name\") second call: %p   (expected: same address)\n", (void*)name2);
    printf("  pointer equality: %s\n", (name1 == name2) ? "yes" : "NO -- unexpected");
    printf("  \"name\" and \"value\" are different pointers: %s\n\n",
           (name1 != value1) ? "yes" : "NO -- unexpected");

    printf("  table count after interning 2 unique strings (3 calls): %zu   (expected: 2)\n\n",
           intern_table_count(&table));
}

static void demo_cache_line_alignment(void) {
    printf("--- the cache-line-alignment claim, checked directly ---\n");

    printf("  sizeof(InternSlot) = %zu bytes   (expected: 16, enforced at compile time\n", sizeof(InternSlot));
    printf("  by a _Static_assert -- this isn't just claimed, the build fails if it drifts)\n");
    printf("  64-byte cache line / 16-byte slot = %d slots per cache line, exactly\n\n", 64 / 16);

    unsigned char slots_backing[256 * sizeof(InternSlot) + 64];
    unsigned char string_backing[4096];
    InternTable table;
    intern_table_init(&table, slots_backing, 256, string_backing, sizeof(string_backing));

    printf("  slots array start address is 64-byte aligned: %s\n\n",
           is_aligned(table.slots, 64) ? "yes" : "NO -- unexpected");
}

static char* make_string(int n) {
    char* buf = malloc(16);
    snprintf(buf, 16, "key_%d", n);
    return buf;
}

static void demo_o1_lookup_benchmark(void) {
    printf("--- empirical proof of O(1) average lookup, not just claimed ---\n");
    printf("  If lookups were O(n), per-lookup time would grow as the table fills.\n");
    printf("  Measuring the SAME number of lookups against tables of increasing size\n");
    printf("  should show roughly FLAT per-lookup time if the O(1) claim actually holds.\n\n");

    size_t capacities[] = {1024, 16384, 262144};
    const int kLookupsPerRound = 200000;

    for (size_t c = 0; c < sizeof(capacities) / sizeof(capacities[0]); c++) {
        size_t capacity = capacities[c];
        size_t string_count = capacity / 2;

        size_t slots_bytes = capacity * sizeof(InternSlot) + 64;
        unsigned char* slots_backing = malloc(slots_bytes);
        size_t arena_bytes = string_count * 32;
        unsigned char* arena_backing = malloc(arena_bytes);

        InternTable table;
        intern_table_init(&table, slots_backing, capacity, arena_backing, arena_bytes);

        char** keys = malloc(string_count * sizeof(char*));
        for (size_t i = 0; i < string_count; i++) {
            keys[i] = make_string((int)i);
            intern_table_intern_cstr(&table, keys[i]);
        }

        double start = recon_monotonic_seconds();
        for (int i = 0; i < kLookupsPerRound; i++) {
            const char* key = keys[i % string_count];
            const char* result = intern_table_lookup(&table, key, strlen(key));
            (void)result;
        }
        double end = recon_monotonic_seconds();

        double per_lookup_nanoseconds = ((end - start) / kLookupsPerRound) * 1e9;
        printf("  capacity=%-7zu  strings_interned=%-6zu  %d lookups: %.1f ns/lookup\n",
               capacity, string_count, kLookupsPerRound, per_lookup_nanoseconds);

        for (size_t i = 0; i < string_count; i++) {
            free(keys[i]);
        }
        free(keys);
        free(slots_backing);
        free(arena_backing);
    }

    printf("\n  If these numbers stay roughly flat across a 256x growth in table size\n");
    printf("  (1024 -> 262144), that's the actual empirical signature of O(1) average\n");
    printf("  lookup -- not a comment claiming it.\n\n");
}

int main(void) {
    demo_basic_interning();
    demo_cache_line_alignment();
    demo_o1_lookup_benchmark();
    return 0;
}
