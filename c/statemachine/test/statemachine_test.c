#include "unity.h"
#include "recon/printer_controller.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static PrinterContext fresh_context(void) {
    PrinterContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    return ctx;
}

void test_idle_ignores_unrelated_events(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_dispatch(Printer_IDLE, &ctx, EVENT_CMD_PAUSE);
    TEST_ASSERT_EQUAL(Printer_IDLE, state);
}

void test_idle_to_homing_on_cmd_home(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_dispatch(Printer_IDLE, &ctx, EVENT_CMD_HOME);
    TEST_ASSERT_EQUAL(Printer_HOMING, state);
}

void test_homing_zeroes_step_counters_on_enter(void) {
    PrinterContext ctx = fresh_context();
    ctx.step_count_x = 100;
    ctx.step_count_y = 200;
    ctx.step_count_z = 300;

    Printer_dispatch(Printer_IDLE, &ctx, EVENT_CMD_HOME);

    TEST_ASSERT_EQUAL(0, ctx.step_count_x);
    TEST_ASSERT_EQUAL(0, ctx.step_count_y);
    TEST_ASSERT_EQUAL(0, ctx.step_count_z);
}

void test_full_happy_path_idle_to_ready(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_IDLE;

    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    TEST_ASSERT_EQUAL(Printer_HOMING, state);

    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    TEST_ASSERT_EQUAL(Printer_READY, state);

    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    TEST_ASSERT_EQUAL(Printer_PRINTING, state);

    state = Printer_dispatch(state, &ctx, EVENT_CMD_STOP);
    TEST_ASSERT_EQUAL(Printer_READY, state);
}

void test_no_transition_means_no_enter_or_exit_actions_fire(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_dispatch(Printer_IDLE, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    TEST_ASSERT_EQUAL(Printer_READY, state);
    TEST_ASSERT_EQUAL(1, ctx.enter_count[Printer_READY]);

    state = Printer_dispatch(state, &ctx, EVENT_CMD_PAUSE);
    TEST_ASSERT_EQUAL(Printer_READY, state);
    TEST_ASSERT_EQUAL(1, ctx.enter_count[Printer_READY]);
}

void test_pause_resume_preserves_progress(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_IDLE;
    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);

    ctx.print_progress_percent = 73;

    state = Printer_dispatch(state, &ctx, EVENT_CMD_PAUSE);
    TEST_ASSERT_EQUAL(Printer_PAUSED, state);

    state = Printer_dispatch(state, &ctx, EVENT_CMD_RESUME);
    TEST_ASSERT_EQUAL(Printer_PRINTING, state);
    TEST_ASSERT_EQUAL(73, ctx.print_progress_percent);
}

void test_printing_entered_twice_across_a_pause_resume_cycle(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_IDLE;
    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_PAUSE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_RESUME);

    TEST_ASSERT_EQUAL(2, ctx.enter_count[Printer_PRINTING]);
    TEST_ASSERT_EQUAL(1, ctx.exit_count[Printer_PRINTING]);
}

void test_fault_interrupts_from_printing(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_IDLE;
    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);

    strncpy(ctx.last_fault_reason, "thermal runaway", sizeof(ctx.last_fault_reason) - 1);
    state = Printer_dispatch(state, &ctx, EVENT_FAULT_DETECTED);

    TEST_ASSERT_EQUAL(Printer_ERROR, state);
    TEST_ASSERT_EQUAL_STRING("thermal runaway", ctx.last_fault_reason);
    TEST_ASSERT_EQUAL(1, ctx.exit_count[Printer_PRINTING]);
}

void test_fault_interrupts_from_paused(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_IDLE;
    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_PAUSE);

    state = Printer_dispatch(state, &ctx, EVENT_FAULT_DETECTED);
    TEST_ASSERT_EQUAL(Printer_ERROR, state);
}

void test_error_state_ignores_everything_except_fault_cleared(void) {
    PrinterContext ctx = fresh_context();
    Printer_State state = Printer_dispatch(Printer_IDLE, &ctx, EVENT_FAULT_DETECTED);
    TEST_ASSERT_EQUAL(Printer_ERROR, state);

    Printer_State still_error = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    TEST_ASSERT_EQUAL(Printer_ERROR, still_error);

    still_error = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    TEST_ASSERT_EQUAL(Printer_ERROR, still_error);
}

void test_fault_cleared_returns_to_idle_and_clears_reason(void) {
    PrinterContext ctx = fresh_context();
    strncpy(ctx.last_fault_reason, "some fault", sizeof(ctx.last_fault_reason) - 1);
    Printer_State state = Printer_dispatch(Printer_IDLE, &ctx, EVENT_FAULT_DETECTED);
    TEST_ASSERT_EQUAL(Printer_ERROR, state);

    state = Printer_dispatch(state, &ctx, EVENT_FAULT_CLEARED);
    TEST_ASSERT_EQUAL(Printer_IDLE, state);
    TEST_ASSERT_EQUAL_STRING("", ctx.last_fault_reason);
}

void test_state_name_returns_readable_strings(void) {
    TEST_ASSERT_EQUAL_STRING("IDLE", Printer_state_name(Printer_IDLE));
    TEST_ASSERT_EQUAL_STRING("PRINTING", Printer_state_name(Printer_PRINTING));
    TEST_ASSERT_EQUAL_STRING("ERROR", Printer_state_name(Printer_ERROR));
}

typedef struct {
    const char* name;
    UnityTestFunction func;
} TestCase;

static const TestCase test_cases[] = {
    {"test_idle_ignores_unrelated_events", test_idle_ignores_unrelated_events},
    {"test_idle_to_homing_on_cmd_home", test_idle_to_homing_on_cmd_home},
    {"test_homing_zeroes_step_counters_on_enter", test_homing_zeroes_step_counters_on_enter},
    {"test_full_happy_path_idle_to_ready", test_full_happy_path_idle_to_ready},
    {"test_no_transition_means_no_enter_or_exit_actions_fire",
     test_no_transition_means_no_enter_or_exit_actions_fire},
    {"test_pause_resume_preserves_progress", test_pause_resume_preserves_progress},
    {"test_printing_entered_twice_across_a_pause_resume_cycle",
     test_printing_entered_twice_across_a_pause_resume_cycle},
    {"test_fault_interrupts_from_printing", test_fault_interrupts_from_printing},
    {"test_fault_interrupts_from_paused", test_fault_interrupts_from_paused},
    {"test_error_state_ignores_everything_except_fault_cleared",
     test_error_state_ignores_everything_except_fault_cleared},
    {"test_fault_cleared_returns_to_idle_and_clears_reason",
     test_fault_cleared_returns_to_idle_and_clears_reason},
    {"test_state_name_returns_readable_strings", test_state_name_returns_readable_strings},
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
