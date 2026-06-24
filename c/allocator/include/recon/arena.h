#pragma once

#include <stddef.h>

// A bump (arena) allocator -- the fastest possible allocator shape:
// allocation is "return the current pointer, advance it by size," and
// freeing an individual allocation is not supported AT ALL. Instead,
// the entire arena is freed in one O(1) operation that simply resets
// the bump pointer back to the start -- every allocation made since
// the last reset becomes invalid at once. This is the right tool when
// a group of allocations all share the same lifetime (everything
// needed for one frame of a game loop, one parsed request, one
// G-code line) and can be thrown away together rather than tracked
// and freed individually.
//
// ─── Why alignment matters here, specifically ──────────────────────────────
//
// A naive bump allocator that just adds `size` to the offset every
// time will happily hand back a `double*` that starts at an odd byte
// address. On some architectures that is a hard crash; on others it is
// just slower (unaligned access often costs extra cycles); either way
// it is a real correctness bug waiting to surface the moment someone
// allocates a type with stricter alignment requirements than whatever
// was allocated immediately before it. arena_alloc_aligned() rounds
// the bump pointer UP to the next multiple of the requested alignment
// before handing it out, which is what an allocator that actually
// understands memory layout is supposed to do -- arena_alloc() is a
// thin convenience wrapper that just defaults to pointer-sized
// alignment, which is sufficient for the great majority of cases.
typedef struct {
    unsigned char* buffer;
    size_t capacity;
    size_t offset;
} Arena;

// buffer must point to at least `capacity` bytes of caller-owned
// memory -- a stack array, a static buffer, or a single malloc'd
// block. The arena itself never allocates anything; it only ever
// hands out sub-regions of what it was given.
void arena_init(Arena* arena, unsigned char* buffer, size_t capacity);

// Allocates `size` bytes aligned to `alignment` (which must be a power
// of two). Returns NULL if there isn't enough room left.
void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment);

// Convenience wrapper -- aligns to sizeof(void*), which is correct for
// the overwhelming majority of allocations. Use arena_alloc_aligned
// directly for anything with a stricter requirement (SIMD types,
// hardware DMA buffers, and so on).
void* arena_alloc(Arena* arena, size_t size);

// Frees EVERY allocation made since the last init/reset, in O(1) --
// this does not zero or touch the underlying memory, it only resets
// the bump pointer. Every pointer previously returned by this arena is
// now invalid and must not be used again.
void arena_reset(Arena* arena);

size_t arena_bytes_used(const Arena* arena);
size_t arena_bytes_remaining(const Arena* arena);
