#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>

// A type-safe, lock-free, single-producer/single-consumer ring buffer.
//
// ─── Why a macro, not void* ─────────────────────────────────────────────────
//
// C has no templates. The conventional way to write one ring buffer
// that works for any element type is to store void* and cast in and
// out at every call site -- this loses type safety entirely (nothing
// stops a caller from pushing an int into a buffer meant for a
// float*), and it adds a pointer-indirection and heap-allocation cost
// per element that a value type like int or a small struct shouldn't
// need to pay at all.
//
// RING_BUFFER_DEFINE(TYPE, NAME, CAPACITY) is the C answer: an X-macro
// that, when invoked, generates a COMPLETE, INDEPENDENT struct type and
// a full set of functions for that exact TYPE and CAPACITY, entirely
// at compile time. RING_BUFFER_DEFINE(int, IntQueue, 16) and
// RING_BUFFER_DEFINE(float, FloatQueue, 64) produce two genuinely
// distinct, non-interchangeable types -- IntQueue_push(q, 1.5f) is a
// compile error, not a runtime surprise. Every function is declared
// `static inline`, so this macro is safe to invoke from a header that
// gets included into multiple translation units without violating the
// one-definition rule -- the C equivalent of why C++ class templates
// live entirely in headers too.
//
// ─── Why this is actually lock-free, not just "no mutex" ──────────────────
//
// It is tempting to reach for `volatile` here and call it done.
// `volatile` is NOT sufficient for this: it only tells the compiler not
// to cache a value in a register or reorder accesses to it at the
// SOURCE level -- it makes no guarantee at all about CPU-level memory
// ordering or visibility across cores, and it is not atomic for
// anything wider than the platform guarantees by accident. Two threads
// on different cores can both be "reading volatile" and still observe
// stale or torn values. This is the same category of mistake as
// thinking a non-atomic counter is safe just because it compiles --
// see this repo's Go bloom filter and Java concurrency modules for the
// same lesson in different languages.
//
// What actually makes this safe is C11's <stdatomic.h>, used with
// explicit memory ordering:
//   - the producer's OWN index (tail) is loaded with memory_order_relaxed
//     -- only the producer ever writes it, so there is no other writer
//     to synchronize with for that load.
//   - the OTHER side's index (head, as seen by the producer) is loaded
//     with memory_order_acquire -- this synchronizes with the
//     consumer's memory_order_release store of head, guaranteeing the
//     producer sees a head value that is not stale.
//   - publishing your own updated index is a memory_order_release
//     store -- this guarantees that the data write that happened
//     immediately before it (writing the new element into the buffer)
//     is visible to whichever thread later does an acquire load of
//     that same index.
//
// This acquire/release pairing is what makes the data write and the
// index update visible together, in order, without a mutex.
//
// ─── Why SPSC specifically is simpler than a general lock-free queue ──────
//
// This implementation relies on EXACTLY ONE thread ever calling push()
// and EXACTLY ONE (different) thread ever calling pop(). That
// constraint is what allows plain atomic load/store to be sufficient --
// a multi-producer or multi-consumer queue would need compare-and-swap
// retry loops to resolve contention between multiple writers of the
// SAME index, which is a meaningfully harder problem. Calling push()
// from more than one thread, or pop() from more than one thread, is
// undefined behavior with this implementation -- it is not a general-
// purpose concurrent queue.
//
// ─── Why head/tail never wrap, only their INDEX into the array does ───────
//
// head and tail are monotonically increasing counters across the
// buffer's entire lifetime -- they are never reset back into [0,
// CAPACITY) themselves. Only `& (CAPACITY - 1)` (a fast bitmask,
// requiring CAPACITY to be a power of two) wraps a counter into a
// valid array index at the point of actually reading or writing an
// element. This avoids the classic ambiguity naive circular buffers
// run into, where head == tail could mean either empty or full --
// here, head == tail unambiguously means empty, and
// tail - head == CAPACITY unambiguously means full.
#define RING_BUFFER_DEFINE(TYPE, NAME, CAPACITY)                              \
    _Static_assert(((CAPACITY) & ((CAPACITY) - 1)) == 0,                     \
                    "ring buffer capacity must be a power of two");          \
                                                                                \
    typedef struct {                                                          \
        TYPE buffer[CAPACITY];                                                \
        atomic_size_t head; /* next slot the consumer will read */            \
        atomic_size_t tail; /* next slot the producer will write */           \
    } NAME##_t;                                                               \
                                                                                \
    static inline void NAME##_init(NAME##_t* rb) {                           \
        atomic_init(&rb->head, 0);                                            \
        atomic_init(&rb->tail, 0);                                            \
    }                                                                          \
                                                                                \
    static inline bool NAME##_push(NAME##_t* rb, TYPE value) {               \
        size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed); \
        size_t head = atomic_load_explicit(&rb->head, memory_order_acquire); \
        if (tail - head == (CAPACITY)) {                                     \
            return false;                                                     \
        }                                                                     \
        rb->buffer[tail & ((CAPACITY) - 1)] = value;                         \
        atomic_store_explicit(&rb->tail, tail + 1, memory_order_release);    \
        return true;                                                          \
    }                                                                          \
                                                                                \
    static inline bool NAME##_pop(NAME##_t* rb, TYPE* out) {                 \
        size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed); \
        size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire); \
        if (head == tail) {                                                   \
            return false;                                                     \
        }                                                                     \
        *out = rb->buffer[head & ((CAPACITY) - 1)];                          \
        atomic_store_explicit(&rb->head, head + 1, memory_order_release);    \
        return true;                                                          \
    }                                                                          \
                                                                                \
    static inline bool NAME##_is_empty(const NAME##_t* rb) {                 \
        size_t head = atomic_load_explicit((atomic_size_t*)&rb->head, memory_order_acquire); \
        size_t tail = atomic_load_explicit((atomic_size_t*)&rb->tail, memory_order_acquire); \
        return head == tail;                                                  \
    }                                                                          \
                                                                                \
    static inline bool NAME##_is_full(const NAME##_t* rb) {                  \
        size_t head = atomic_load_explicit((atomic_size_t*)&rb->head, memory_order_acquire); \
        size_t tail = atomic_load_explicit((atomic_size_t*)&rb->tail, memory_order_acquire); \
        return (tail - head) == (CAPACITY);                                  \
    }                                                                          \
                                                                                \
    static inline size_t NAME##_size(const NAME##_t* rb) {                   \
        size_t head = atomic_load_explicit((atomic_size_t*)&rb->head, memory_order_acquire); \
        size_t tail = atomic_load_explicit((atomic_size_t*)&rb->tail, memory_order_acquire); \
        return tail - head;                                                   \
    }                                                                          \
                                                                                \
    static inline size_t NAME##_capacity(const NAME##_t* rb) {               \
        (void)rb;                                                             \
        return (CAPACITY);                                                    \
    }
