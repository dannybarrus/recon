#include "unity.h"
#include "recon/recon_thread.h"
#include "recon/uart_registers.h"

#include <stdatomic.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_init_sets_every_field_to_zero(void) {
    UartRegisters regs;
    regs.tx_data = 0xFF;
    regs.tx_pending = 0xFF;
    regs.rx_data = 0xFF;
    regs.rx_ready = 0xFF;

    uart_registers_init(&regs);

    TEST_ASSERT_EQUAL_UINT8(0, regs.tx_data);
    TEST_ASSERT_EQUAL_UINT8(0, regs.tx_pending);
    TEST_ASSERT_EQUAL_UINT8(0, regs.rx_data);
    TEST_ASSERT_EQUAL_UINT8(0, regs.rx_ready);
}

void test_peripheral_step_does_nothing_when_no_pending_request(void) {
    UartRegisters regs;
    uart_registers_init(&regs);

    uart_peripheral_step(&regs);

    TEST_ASSERT_EQUAL_UINT8(0, regs.rx_ready);
    TEST_ASSERT_EQUAL_UINT8(0, regs.tx_pending);
}

void test_peripheral_step_processes_a_pending_request(void) {
    UartRegisters regs;
    uart_registers_init(&regs);
    regs.tx_data = 0x42;
    regs.tx_pending = 1;

    uart_peripheral_step(&regs);

    TEST_ASSERT_EQUAL_UINT8(0, regs.tx_pending);
    TEST_ASSERT_EQUAL_UINT8(1, regs.rx_ready);
    TEST_ASSERT_EQUAL_UINT8(0x42, regs.rx_data);
}

void test_peripheral_step_clears_stale_rx_ready_before_processing(void) {
    UartRegisters regs;
    uart_registers_init(&regs);
    regs.rx_ready = 1;
    regs.rx_data = 0xFF;

    regs.tx_data = 0x10;
    regs.tx_pending = 1;

    uart_peripheral_step(&regs);

    TEST_ASSERT_EQUAL_UINT8(0x10, regs.rx_data);
    TEST_ASSERT_EQUAL_UINT8(1, regs.rx_ready);
}

void test_receive_times_out_when_nothing_arrives(void) {
    UartRegisters regs;
    uart_registers_init(&regs);

    uint8_t byte;
    int got_data = uart_receive_byte(&regs, &byte, 1000);

    TEST_ASSERT_EQUAL_INT(0, got_data);
}

void test_receive_succeeds_once_rx_ready_is_already_set(void) {
    UartRegisters regs;
    uart_registers_init(&regs);
    regs.rx_data = 0x99;
    regs.rx_ready = 1;

    uint8_t byte;
    int got_data = uart_receive_byte(&regs, &byte, 1000);

    TEST_ASSERT_EQUAL_INT(1, got_data);
    TEST_ASSERT_EQUAL_UINT8(0x99, byte);
}

typedef struct {
    UartRegisters* regs;
    _Atomic int should_stop;
} PeripheralArgs;

static void* test_peripheral_thread(void* arg) {
    PeripheralArgs* args = (PeripheralArgs*)arg;
    while (!atomic_load_explicit(&args->should_stop, memory_order_relaxed)) {
        uart_peripheral_step(args->regs);
        recon_thread_yield();
    }
    return NULL;
}

void test_full_loopback_round_trip_with_real_threading(void) {
    UartRegisters regs;
    uart_registers_init(&regs);

    PeripheralArgs args = {.regs = &regs, .should_stop = 0};
    recon_thread_t peripheral;
    recon_thread_create(&peripheral, test_peripheral_thread, &args);

    uart_send_byte(&regs, 0xAB);
    uint8_t received;
    int got = uart_receive_byte(&regs, &received, 100000000L);

    atomic_store_explicit(&args.should_stop, 1, memory_order_relaxed);
    recon_thread_join(peripheral);

    TEST_ASSERT_EQUAL_INT(1, got);
    TEST_ASSERT_EQUAL_UINT8(0xAB, received);
}

void test_multiple_bytes_round_trip_in_order(void) {
    UartRegisters regs;
    uart_registers_init(&regs);

    PeripheralArgs args = {.regs = &regs, .should_stop = 0};
    recon_thread_t peripheral;
    recon_thread_create(&peripheral, test_peripheral_thread, &args);

    uint8_t to_send[5] = {1, 2, 3, 4, 5};
    uint8_t received[5];

    for (int i = 0; i < 5; i++) {
        uart_send_byte(&regs, to_send[i]);
        uart_receive_byte(&regs, &received[i], 100000000L);
    }

    atomic_store_explicit(&args.should_stop, 1, memory_order_relaxed);
    recon_thread_join(peripheral);

    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8(to_send[i], received[i]);
    }
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_init_sets_every_field_to_zero", test_init_sets_every_field_to_zero},
    {"test_peripheral_step_does_nothing_when_no_pending_request",
     test_peripheral_step_does_nothing_when_no_pending_request},
    {"test_peripheral_step_processes_a_pending_request", test_peripheral_step_processes_a_pending_request},
    {"test_peripheral_step_clears_stale_rx_ready_before_processing",
     test_peripheral_step_clears_stale_rx_ready_before_processing},
    {"test_receive_times_out_when_nothing_arrives", test_receive_times_out_when_nothing_arrives},
    {"test_receive_succeeds_once_rx_ready_is_already_set", test_receive_succeeds_once_rx_ready_is_already_set},
    {"test_full_loopback_round_trip_with_real_threading", test_full_loopback_round_trip_with_real_threading},
    {"test_multiple_bytes_round_trip_in_order", test_multiple_bytes_round_trip_in_order},
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
