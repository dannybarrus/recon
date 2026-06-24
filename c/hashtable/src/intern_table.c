#include "recon/intern_table.h"

#include <assert.h>
#include <string.h>

#define CACHE_LINE_SIZE 64

static uint32_t fnv1a_hash(const char* data, size_t len) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; i++) {
        hash ^= (unsigned char)data[i];
        hash *= 16777619u;
    }
    return hash;
}

static uintptr_t align_up(uintptr_t n, uintptr_t alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

void intern_table_init(InternTable* table, unsigned char* slots_backing, size_t slot_capacity,
                        unsigned char* string_arena_backing, size_t string_arena_capacity) {
    assert((slot_capacity & (slot_capacity - 1)) == 0 && "slot_capacity must be a power of two");

    uintptr_t aligned_address = align_up((uintptr_t)slots_backing, CACHE_LINE_SIZE);
    table->slots = (InternSlot*)aligned_address;
    table->capacity = slot_capacity;
    table->count = 0;
    memset(table->slots, 0, slot_capacity * sizeof(InternSlot));

    arena_init(&table->string_arena, string_arena_backing, string_arena_capacity);
}

const char* intern_table_intern(InternTable* table, const char* str, size_t len) {
    uint32_t hash = fnv1a_hash(str, len);
    size_t mask = table->capacity - 1;
    size_t index = hash & mask;

    for (size_t probe = 0; probe < table->capacity; probe++) {
        InternSlot* slot = &table->slots[index];

        if (slot->key == NULL) {
            char* stored = (char*)arena_alloc(&table->string_arena, len + 1);
            if (!stored) {
                return NULL;
            }
            memcpy(stored, str, len);
            stored[len] = '\0';

            slot->key = stored;
            slot->key_len = (uint32_t)len;
            slot->hash = hash;
            table->count++;
            return stored;
        }

        if (slot->hash == hash && slot->key_len == len && memcmp(slot->key, str, len) == 0) {
            return slot->key;
        }

        index = (index + 1) & mask;
    }

    return NULL;
}

const char* intern_table_intern_cstr(InternTable* table, const char* str) {
    return intern_table_intern(table, str, strlen(str));
}

const char* intern_table_lookup(const InternTable* table, const char* str, size_t len) {
    uint32_t hash = fnv1a_hash(str, len);
    size_t mask = table->capacity - 1;
    size_t index = hash & mask;

    for (size_t probe = 0; probe < table->capacity; probe++) {
        const InternSlot* slot = &table->slots[index];

        if (slot->key == NULL) {
            return NULL;
        }

        if (slot->hash == hash && slot->key_len == len && memcmp(slot->key, str, len) == 0) {
            return slot->key;
        }

        index = (index + 1) & mask;
    }

    return NULL;
}

size_t intern_table_count(const InternTable* table) {
    return table->count;
}
