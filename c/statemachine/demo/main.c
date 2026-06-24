#include <stdio.h>
#include <string.h>

#include "recon/printer_controller.h"

static void print_transition(Printer_State before, int event, Printer_State after) {
    printf("  %-9s --(event %d)--> %-9s\n",
           Printer_state_name(before), event, Printer_state_name(after));
}

static void demo_happy_path(void) {
    printf("--- happy path: idle -> homing -> ready -> printing -> ready ---\n");

    PrinterContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    Printer_State state = Printer_IDLE;

    int events[] = {
        EVENT_CMD_HOME,
        EVENT_HOME_DONE,
        EVENT_CMD_START_PRINT,
        EVENT_CMD_STOP,
    };

    for (size_t i = 0; i < sizeof(events) / sizeof(events[0]); i++) {
        Printer_State before = state;
        state = Printer_dispatch(state, &ctx, events[i]);
        print_transition(before, events[i], state);
    }

    printf("  final state: %s   (expected: READY)\n", Printer_state_name(state));
    printf("  HOMING entered %d time(s)   (expected: 1)\n\n", ctx.enter_count[Printer_HOMING]);
}

static void demo_pause_resume(void) {
    printf("--- pause/resume: printing keeps its progress across a pause ---\n");

    PrinterContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    Printer_State state = Printer_IDLE;

    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);

    ctx.print_progress_percent = 42;
    printf("  print_progress_percent set to %d while PRINTING\n", ctx.print_progress_percent);

    state = Printer_dispatch(state, &ctx, EVENT_CMD_PAUSE);
    printf("  paused -- state: %s\n", Printer_state_name(state));

    state = Printer_dispatch(state, &ctx, EVENT_CMD_RESUME);
    printf("  resumed -- state: %s\n", Printer_state_name(state));
    printf("  print_progress_percent after resume: %d   (expected: still 42 -- on_enter_printing\n",
           ctx.print_progress_percent);
    printf("  deliberately does not reset it, since resuming should not discard progress)\n");
    printf("  PRINTING entered %d time(s)   (expected: 2 -- once on start, once on resume)\n\n",
           ctx.enter_count[Printer_PRINTING]);
}

static void demo_fault_interrupts_from_any_state(void) {
    printf("--- fault path: FAULT_DETECTED interrupts mid-print ---\n");

    PrinterContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    Printer_State state = Printer_IDLE;

    state = Printer_dispatch(state, &ctx, EVENT_CMD_HOME);
    state = Printer_dispatch(state, &ctx, EVENT_HOME_DONE);
    state = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    printf("  state before fault: %s\n", Printer_state_name(state));

    strncpy(ctx.last_fault_reason, "endstop triggered during print", sizeof(ctx.last_fault_reason) - 1);
    state = Printer_dispatch(state, &ctx, EVENT_FAULT_DETECTED);

    printf("  state after fault: %s   (expected: ERROR)\n", Printer_state_name(state));
    printf("  fault reason recorded: \"%s\"\n", ctx.last_fault_reason);

    Printer_State ignored = Printer_dispatch(state, &ctx, EVENT_CMD_START_PRINT);
    printf("  CMD_START_PRINT while in ERROR is ignored, state stays: %s   (expected: ERROR)\n",
           Printer_state_name(ignored));

    state = Printer_dispatch(state, &ctx, EVENT_FAULT_CLEARED);
    printf("  after FAULT_CLEARED: %s   (expected: IDLE)\n", Printer_state_name(state));
    printf("  fault reason after recovery: \"%s\"   (expected: empty -- cleared by on_exit_error)\n\n",
           ctx.last_fault_reason);
}

int main(void) {
    demo_happy_path();
    demo_pause_resume();
    demo_fault_interrupts_from_any_state();
    return 0;
}
