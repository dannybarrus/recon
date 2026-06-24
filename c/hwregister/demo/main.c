#include <stdatomic.h>
#include <stdio.h>

#include "recon/recon_thread.h"
#include "recon/uart_registers.h"

typedef struct {
    UartRegisters* regs;
    _Atomic int should_stop;
} PeripheralThreadArgs;

static void* peripheral_thread_func(void* arg) {
    PeripheralThreadArgs* args = (PeripheralThreadArgs*)arg;
    while (!atomic_load_explicit(&args->should_stop, memory_order_relaxed)) {
        uart_peripheral_step(args->regs);
        recon_thread_yield();
    }
    return NULL;
}

static void demo_loopback(void) {
    printf("--- UART loopback: send a byte, the simulated peripheral echoes it back ---\n");
    printf("  (looping transmit back into receive is the real technique hardware\n");
    printf("  engineers use to bench-test a UART driver without a second device)\n\n");

    UartRegisters regs;
    uart_registers_init(&regs);

    PeripheralThreadArgs args = {.regs = &regs, .should_stop = 0};
    recon_thread_t peripheral;
    recon_thread_create(&peripheral, peripheral_thread_func, &args);

    const char* message = "Hello, UART!";
    printf("  sending: \"%s\"\n", message);

    char received[32];
    int received_count = 0;

    for (int i = 0; message[i] != '\0'; i++) {
        uart_send_byte(&regs, (uint8_t)message[i]);

        uint8_t byte;
        if (uart_receive_byte(&regs, &byte, 100000000L)) {
            received[received_count++] = (char)byte;
        }
    }
    received[received_count] = '\0';

    printf("  received: \"%s\"   (expected: identical to what was sent)\n\n", received);

    atomic_store_explicit(&args.should_stop, 1, memory_order_relaxed);
    recon_thread_join(peripheral);
}

static void demo_receive_timeout(void) {
    printf("--- uart_receive_byte() correctly times out when nothing arrives ---\n");

    UartRegisters regs;
    uart_registers_init(&regs);

    uint8_t byte;
    int got_data = uart_receive_byte(&regs, &byte, 1000);

    printf("  uart_receive_byte() with no peripheral running: %s   (expected: timed out)\n\n",
           got_data ? "got data -- unexpected" : "timed out, as expected");
}

int main(void) {
    demo_loopback();
    demo_receive_timeout();
    printf("--- the volatile_proof comparison ---\n");
    printf("  Built as two SEPARATE executables, both forced to a real optimization\n");
    printf("  level. Run them directly:\n");
    printf("    .../hwregister_volatile_proof_with\n");
    printf("    .../hwregister_volatile_proof_without\n");
    printf("  to see what your own compiler actually does with each.\n");
    return 0;
}
