#pragma once

#include <stdatomic.h>
#include <stdint.h>

// A mock memory-mapped UART peripheral, the kind every microcontroller
// reference manual describes as a small set of fixed register
// addresses the CPU reads and writes directly. Real hardware mounts
// this struct at a literal fixed physical address (something like
// `#define UART1 ((UartRegisters*)0x40011000)`); this simulation just
// uses ordinary allocated memory standing in for that address -- the
// volatility semantics needed are IDENTICAL either way, since the
// concern is about preventing compiler assumptions about a piece of
// memory, not about what physically backs it.
//
// ─── Why plain volatile alone was not enough here -- a real finding ───────
//
// An earlier version of this struct used plain `volatile uint8_t` for
// every field, reasoning that each flag only ever has two possible
// writers using DISJOINT values (the CPU writes 1, the peripheral
// writes 0) in a blocking protocol that never lets both sides write at
// once -- so surely nothing could actually go wrong.
//
// ThreadSanitizer disagreed, correctly: running the loopback demo
// under -DRECON_TSAN=ON reported real data races on tx_pending and
// rx_ready. The reasoning above was about VALUES never colliding in a
// way that loses information -- but the C memory model's definition of
// a data race is about concurrent, unsynchronized access to the same
// memory location from different threads where at least one is a
// write, full stop. It does not matter that the values happened to
// avoid corruption in this specific protocol; plain volatile gives no
// formal synchronization guarantee between threads at all, only a
// guarantee against the COMPILER caching a value within a single
// thread. The demo's visible output was correct -- the program still
// had undefined behavior under it, which is exactly the gap between "a
// real single-core microcontroller's informal but practically safe
// register-access conventions" and "a formally race-free C program."
//
// The fix: tx_pending and rx_ready -- the fields genuinely touched by
// BOTH threads -- are now _Atomic, using the SAME acquire/release
// discipline already proven correct (and TSan-verified) in this
// repo's ringbuffer/ringbuffer.h. tx_data and rx_data stay plain
// volatile, but their visibility is established TRANSITIVELY: a
// release store on the adjacent flag, paired with an acquire load of
// that same flag, guarantees the data write that happened immediately
// before the release is visible to whichever thread's acquire load
// observes it -- exactly the mechanism the ring buffer's head/tail
// indices use to protect its buffered data.
//
// ─── The single-writer-per-field protocol, now correctly synchronized ─────
//
//   tx_data:    written only by the CPU; visibility protected by the
//               release store to tx_pending immediately after it
//   tx_pending: CPU stores 1 (release) to request a send; the
//               peripheral stores 0 (release) to acknowledge. Both
//               sides load it with acquire semantics.
//   rx_data:    written only by the peripheral; visibility protected
//               by the release store to rx_ready immediately after it
//   rx_ready:   written only by the peripheral (it clears its own
//               stale flag before preparing the next byte); the CPU
//               only ever loads it with acquire semantics
typedef struct {
    volatile uint8_t tx_data;
    _Atomic uint8_t tx_pending;
    volatile uint8_t rx_data;
    _Atomic uint8_t rx_ready;
} UartRegisters;

void uart_registers_init(UartRegisters* regs);

// Blocks until the simulated peripheral acknowledges the byte was
// transmitted. This is the function whose poll loop is the actual
// subject of the volatile_proof comparison.
void uart_send_byte(UartRegisters* regs, uint8_t byte);

// Polls for up to max_polls iterations for a received byte. Returns 1
// and writes *out if one arrived in time, 0 if it timed out. long long
// rather than plain long specifically because plain `long` is only
// 32 bits on Windows x64 (the LLP64 data model), unlike Linux/macOS
// where it's 64 -- a large iteration cap stored in a 32-bit long can
// silently overflow into a negative value, making the loop condition
// false immediately and the whole poll a no-op. long long is
// guaranteed at least 64 bits on every platform this code targets.
int uart_receive_byte(UartRegisters* regs, uint8_t* out, long long max_polls);

// Runs the simulated peripheral's side of the protocol for ONE pending
// request, if there is one: clears the stale rx_ready flag, simulates
// transmission latency, loops the byte back into rx_data, acknowledges
// tx_pending, and sets rx_ready. Intended to be called repeatedly from
// a dedicated background thread (see the demo and tests for how), but
// deliberately exposed as a single, callable step rather than its own
// thread-owning function, so it stays directly testable without
// needing real threading in every test that exercises it.
void uart_peripheral_step(UartRegisters* regs);
