#pragma once

#include <stddef.h>

// A fixed-size block (pool) allocator -- every block is the exact same
// size, which eliminates an entire category of problem a general-
// purpose allocator has to deal with: fragmentation. There is no such
// thing as "a gap too small for the next request" when every request
// is the same size as every gap.
//
// ─── The intrusive free list ────────────────────────────────────────────────
//
// Free blocks are threaded into a singly linked list, but that list's
// "next" pointers are NOT stored in separate bookkeeping memory --
// they live INSIDE the free blocks themselves, in the same bytes that
// will later hold user data once the block is allocated. This works
// because a block is never simultaneously free AND in use: while it's
// on the free list, the allocator owns those bytes and uses them to
// store the next-pointer; the moment pool_alloc() hands the block to a
// caller, the allocator stops touching those bytes entirely, and the
// caller is free to overwrite them with anything. This is a real,
// extremely common embedded technique -- it means a pool of N blocks
// costs exactly N * block_size bytes total, with zero per-block
// overhead for the free-list bookkeeping itself.
//
// One consequence: block_size must be at least sizeof(void*), since a
// free block needs room for that intrusive pointer. This is enforced
// with an assertion in pool_init() rather than silently rounded up --
// silently increasing block_size would mean the allocator starts
// striding through memory in larger steps than the buffer the caller
// actually sized was based on, which is a real buffer-overflow trap,
// not a convenience. Callers must request at least sizeof(void*)
// themselves.
typedef struct {
    unsigned char* buffer;
    size_t block_size;
    size_t block_count;
    void* free_list;
} Pool;

// buffer must point to at least block_count * block_size bytes of
// caller-owned memory. block_size must be at least sizeof(void*) --
// see the note above for why this is an assertion rather than
// something pool_init quietly works around.
void pool_init(Pool* pool, unsigned char* buffer, size_t block_size, size_t block_count);

// Returns one block, or NULL if every block is currently allocated.
void* pool_alloc(Pool* pool);

// Returns a block to the pool, making it available for a future
// pool_alloc() call. ptr must have come from THIS pool's pool_alloc(),
// and must not still be in use elsewhere -- freeing it does not zero
// its contents, it only threads it back onto the free list.
void pool_free(Pool* pool, void* ptr);

size_t pool_blocks_free(const Pool* pool);
