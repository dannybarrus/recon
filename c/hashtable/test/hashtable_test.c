#include "unity.h"
#include "recon/intern_table.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static int is_aligned(void* ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}

#define TEST_SLOT_CAPACITY 64

static void make_table(InternTable* table, unsigned char* slots_backing, unsigned char* string_backing,
                        size_t string_backing_size) {
    intern_table_init(table, slots_backing, TEST_SLOT_CAPACITY, string_backing, string_backing_size);
}

void test_interning_same_content_returns_same_pointer(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    const char* a = intern_table_intern_cstr(&table, "hello");
    const char* b = intern_table_intern_cstr(&table, "hello");

    TEST_ASSERT_EQUAL_PTR(a, b);
}

void test_interning_different_content_returns_different_pointers(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    const char* a = intern_table_intern_cstr(&table, "hello");
    const char* b = intern_table_intern_cstr(&table, "world");

    TEST_ASSERT_NOT_EQUAL(a, b);
}

void test_interned_string_content_matches_original(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    const char* result = intern_table_intern_cstr(&table, "hello");
    TEST_ASSERT_EQUAL_STRING("hello", result);
}

void test_count_tracks_unique_strings_only(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    intern_table_intern_cstr(&table, "a");
    intern_table_intern_cstr(&table, "b");
    intern_table_intern_cstr(&table, "a");

    TEST_ASSERT_EQUAL_UINT(2, intern_table_count(&table));
}

void test_lookup_finds_an_already_interned_string(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    const char* interned = intern_table_intern_cstr(&table, "hello");
    const char* found = intern_table_lookup(&table, "hello", 5);

    TEST_ASSERT_EQUAL_PTR(interned, found);
}

void test_lookup_returns_null_for_absent_string(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    intern_table_intern_cstr(&table, "hello");
    const char* found = intern_table_lookup(&table, "goodbye", 7);

    TEST_ASSERT_NULL(found);
}

void test_lookup_never_allocates_or_modifies_count(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    intern_table_intern_cstr(&table, "hello");
    size_t count_before = intern_table_count(&table);

    intern_table_lookup(&table, "hello", 5);
    intern_table_lookup(&table, "absent", 6);

    TEST_ASSERT_EQUAL_UINT(count_before, intern_table_count(&table));
}

void test_slots_array_is_cache_line_aligned(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    TEST_ASSERT_TRUE(is_aligned(table.slots, 64));
}

void test_intern_slot_is_exactly_16_bytes(void) {
    TEST_ASSERT_EQUAL_UINT(16, sizeof(InternSlot));
}

void test_many_unique_strings_all_remain_individually_lookupable(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    char buffers[40][8];
    int string_count = 40;

    for (int i = 0; i < string_count; i++) {
        snprintf(buffers[i], sizeof(buffers[i]), "k%d", i);
        intern_table_intern_cstr(&table, buffers[i]);
    }

    for (int i = 0; i < string_count; i++) {
        const char* found = intern_table_lookup(&table, buffers[i], strlen(buffers[i]));
        TEST_ASSERT_NOT_NULL(found);
        TEST_ASSERT_EQUAL_STRING(buffers[i], found);
    }

    TEST_ASSERT_EQUAL_UINT((size_t)string_count, intern_table_count(&table));
}

void test_interning_with_explicit_length_handles_embedded_content_correctly(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    unsigned char strings[4096];
    InternTable table;
    make_table(&table, slots, strings, sizeof(strings));

    const char* source = "abcdef";
    const char* result = intern_table_intern(&table, source, 3);

    TEST_ASSERT_EQUAL_STRING("abc", result);
}

void test_string_arena_exhaustion_returns_null_gracefully(void) {
    unsigned char slots[TEST_SLOT_CAPACITY * sizeof(InternSlot) + 64];
    // 16 bytes, not just enough for "ab\0" (3 bytes) -- arena_alloc()
    // defaults to 8-byte alignment, and a stack array isn't
    // guaranteed to start 8-byte aligned itself, so up to 7 bytes of
    // alignment padding could be needed before the first 3 real bytes
    // even land. A buffer sized for the data alone, ignoring the
    // allocator's own alignment overhead, was the actual bug in an
    // earlier version of this test -- it failed the FIRST intern call
    // too, not just the second one this test means to exercise.
    unsigned char tiny_strings[16];
    InternTable table;
    make_table(&table, slots, tiny_strings, sizeof(tiny_strings));

    const char* first = intern_table_intern_cstr(&table, "ab");
    TEST_ASSERT_NOT_NULL(first);

    const char* second = intern_table_intern_cstr(&table, "this string will not fit");
    TEST_ASSERT_NULL(second);
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_interning_same_content_returns_same_pointer", test_interning_same_content_returns_same_pointer},
    {"test_interning_different_content_returns_different_pointers",
     test_interning_different_content_returns_different_pointers},
    {"test_interned_string_content_matches_original", test_interned_string_content_matches_original},
    {"test_count_tracks_unique_strings_only", test_count_tracks_unique_strings_only},
    {"test_lookup_finds_an_already_interned_string", test_lookup_finds_an_already_interned_string},
    {"test_lookup_returns_null_for_absent_string", test_lookup_returns_null_for_absent_string},
    {"test_lookup_never_allocates_or_modifies_count", test_lookup_never_allocates_or_modifies_count},
    {"test_slots_array_is_cache_line_aligned", test_slots_array_is_cache_line_aligned},
    {"test_intern_slot_is_exactly_16_bytes", test_intern_slot_is_exactly_16_bytes},
    {"test_many_unique_strings_all_remain_individually_lookupable",
     test_many_unique_strings_all_remain_individually_lookupable},
    {"test_interning_with_explicit_length_handles_embedded_content_correctly",
     test_interning_with_explicit_length_handles_embedded_content_correctly},
    {"test_string_arena_exhaustion_returns_null_gracefully", test_string_arena_exhaustion_returns_null_gracefully},
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
