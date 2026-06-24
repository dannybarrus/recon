#include "recon/uart_registers.h"

#include "recon/recon_thread.h"

void uart_registers_init(UartRegisters* regs) {
    regs->tx_data = 0;
    atomic_init(&regs->tx_pending, 0);
    regs->rx_data = 0;
    atomic_init(&regs->rx_ready, 0);
}

void uart_send_byte(UartRegisters* regs, uint8_t byte) {
    regs->tx_data = byte;
    // Release: publishes tx_data above, together with this flag, to
    // whichever thread later does an acquire load of tx_pending.
    atomic_store_explicit(&regs->tx_pending, 1, memory_order_release);

    // THIS is the poll loop the volatile_proof comparison is about in
    // spirit -- the acquire load is what guarantees a fresh read each
    // iteration rather than a cached stale value, the same underlying
    // concern volatile addresses, but with the added, necessary
    // guarantee that the peripheral thread's write actually becomes
    // visible here in the first place.
    while (atomic_load_explicit(&regs->tx_pending, memory_order_acquire)) {
    }
}

int uart_receive_byte(UartRegisters* regs, uint8_t* out, long long max_polls) {
    for (long long i = 0; i < max_polls; i++) {
        // Acquire: synchronizes with the peripheral's release store
        // below, guaranteeing rx_data is visible here once this load
        // observes rx_ready == 1.
        if (atomic_load_explicit(&regs->rx_ready, memory_order_acquire)) {
            *out = regs->rx_data;
            return 1;
        }
    }
    return 0;
}

void uart_peripheral_step(UartRegisters* regs) {
    if (atomic_load_explicit(&regs->tx_pending, memory_order_acquire)) {
        // Single writer (this thread) for rx_ready at this point --
        // relaxed is sufficient, there is nothing else to synchronize
        // with for clearing a flag only this thread ever sets.
        atomic_store_explicit(&regs->rx_ready, 0, memory_order_relaxed);

        recon_thread_sleep_ms(5);  // simulate real transmission latency

        regs->rx_data = regs->tx_data;  // safe: tx_data's visibility was established by the acquire above

        // Release: acknowledges the send (uart_send_byte's poll loop
        // can now return) without yet publishing rx_data/rx_ready --
        // that happens in the next store below.
        atomic_store_explicit(&regs->tx_pending, 0, memory_order_release);

        // Release: publishes rx_data above, together with this flag.
        atomic_store_explicit(&regs->rx_ready, 1, memory_order_release);
    }
}
