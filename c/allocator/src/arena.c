#include "recon/arena.h"

#include <assert.h>
#include <stdint.h>

static size_t align_up(size_t n, size_t alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

void arena_init(Arena* arena, unsigned char* buffer, size_t capacity) {
    arena->buffer = buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment) {
    assert((alignment & (alignment - 1)) == 0 && "alignment must be a power of two");

    uintptr_t current_address = (uintptr_t)(arena->buffer + arena->offset);
    uintptr_t aligned_address = align_up(current_address, alignment);
    size_t padding = (size_t)(aligned_address - current_address);

    if (arena->offset + padding + size > arena->capacity) {
        return NULL;
    }

    arena->offset += padding;
    void* result = arena->buffer + arena->offset;
    arena->offset += size;
    return result;
}

void* arena_alloc(Arena* arena, size_t size) {
    return arena_alloc_aligned(arena, size, sizeof(void*));
}

void arena_reset(Arena* arena) {
    arena->offset = 0;
}

size_t arena_bytes_used(const Arena* arena) {
    return arena->offset;
}

size_t arena_bytes_remaining(const Arena* arena) {
    return arena->capacity - arena->offset;
}
