#pragma once

#include <stddef.h>
#include <stdint.h>

#include "recon/arena.h"

// A string-interning dictionary: every unique string content is
// stored exactly once, and interning the same content again always
// returns the SAME canonical pointer. This means two interned strings
// can be compared for equality with a pointer comparison instead of
// strcmp -- a real, common technique in compilers, interpreters, and
// parsers where the same small set of keywords or keys (a config
// file's repeated "name", "value", "type" keys; a symbol table's
// repeated identifiers) shows up over and over.
//
// ─── Why open addressing, not chaining ─────────────────────────────────────
//
// Chaining resolves collisions with a linked list per bucket --
// correct, but each node is a separate heap allocation scattered
// wherever the allocator happened to put it, which is exactly the kind
// of pointer-chasing that defeats CPU cache locality. Open addressing
// keeps every slot in ONE contiguous array; a collision just moves to
// a NEARBY slot in the same array instead of following a pointer
// somewhere else in memory. Specifically, linear probing (the simplest
// open-addressing scheme -- on collision, just try the next slot) is
// the right choice here, not an oversimplification: each successive
// probe touches the immediately adjacent memory address, which is
// very likely already pulled into cache by the probe before it.
// Quadratic probing or double hashing scatter the probe sequence
// further apart specifically to reduce clustering, which is the
// opposite of what a cache-locality goal wants.
//
// ─── The cache-line-alignment claim, verified, not just asserted ──────────
//
// InternSlot is exactly 16 bytes -- a pointer, a uint32_t length, and
// a uint32_t cached hash, with no padding at all (verified with
// _Static_assert below, and independently confirmed by actually
// compiling and inspecting sizeof() before this design was settled
// on). A typical CPU cache line is 64 bytes, so exactly 4 slots fit
// per cache line with no slot ever straddling a cache-line boundary --
// PROVIDED the slots array itself starts at a 64-byte-aligned address,
// which intern_table_init() arranges internally (see its
// implementation for the same align-up technique the arena allocator
// uses).
//
// ─── Why this depends on the arena allocator from allocator/ ──────────────
//
// Every interned string lives for the same lifetime: from the moment
// it's interned until the whole table is torn down. Nothing ever needs
// to free ONE interned string individually -- which is exactly the
// access pattern an arena is for. Rather than re-deriving a second
// bump allocator here, this table uses the one already built and
// verified in allocator/.
typedef struct {
    const char* key;   // NULL means this slot is empty; otherwise points into the string arena
    uint32_t key_len;
    uint32_t hash;
} InternSlot;

_Static_assert(sizeof(InternSlot) == 16, "InternSlot must stay exactly 16 bytes for the cache-line math to hold");

typedef struct {
    InternSlot* slots;
    size_t capacity;
    size_t count;
    Arena string_arena;
} InternTable;

// slots_backing must point to at least
// (slot_capacity * sizeof(InternSlot)) + 63 bytes -- the extra 63
// bytes give intern_table_init() room to find a 64-byte-aligned
// starting address inside the buffer, the same way arena_alloc_aligned
// finds room for alignment padding. slot_capacity must be a power of
// two. string_arena_backing/string_arena_capacity back the arena that
// stores the actual interned string bytes.
void intern_table_init(InternTable* table, unsigned char* slots_backing, size_t slot_capacity,
                        unsigned char* string_arena_backing, size_t string_arena_capacity);

// Interns the first `len` bytes of `str`. If an identical string is
// already interned, returns the EXISTING canonical pointer without
// allocating anything new. Returns NULL if the table is full (every
// slot occupied with non-matching content) or the string arena is out
// of room.
const char* intern_table_intern(InternTable* table, const char* str, size_t len);

// Convenience wrapper for a null-terminated string.
const char* intern_table_intern_cstr(InternTable* table, const char* str);

// Looks up a string WITHOUT interning it -- returns the canonical
// pointer if already present, or NULL if not. Never allocates.
const char* intern_table_lookup(const InternTable* table, const char* str, size_t len);

size_t intern_table_count(const InternTable* table);
