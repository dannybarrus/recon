#pragma once

#include <stddef.h>

#include "recon/state_machine.h"

// A simplified motion controller state machine -- the same shape as
// the control logic in real 3D printer / CNC firmware (Klipper and
// similar projects use exactly this pattern: a small set of states,
// driven by events, with real hardware actions firing on entry/exit).
//
//   IDLE --CMD_HOME--> HOMING --HOME_DONE--> READY
//   READY --CMD_START_PRINT--> PRINTING
//   PRINTING <--CMD_PAUSE/CMD_RESUME--> PAUSED
//   PRINTING or PAUSED --CMD_STOP--> READY
//   ANY (except ERROR) --FAULT_DETECTED--> ERROR
//   ERROR --FAULT_CLEARED--> IDLE
//
// A FAULT_DETECTED event can interrupt the machine from almost any
// state -- exactly the kind of "something just went wrong, stop
// everything now" path real motion control firmware needs (a thermal
// runaway, an endstop trigger during an unexpected move, and so on).

typedef enum {
    EVENT_CMD_HOME,
    EVENT_HOME_DONE,
    EVENT_CMD_START_PRINT,
    EVENT_CMD_PAUSE,
    EVENT_CMD_RESUME,
    EVENT_CMD_STOP,
    EVENT_FAULT_DETECTED,
    EVENT_FAULT_CLEARED,
} PrinterEvent;

// Carries whatever state the handlers need. enter_count is
// instrumentation -- the same "prove it happened, don't just claim it"
// pattern as TrackedResource elsewhere in this repo -- incremented by
// every on_enter action, letting tests assert exactly how many times
// each state was actually entered, not just trust that it was.
typedef struct {
    int step_count_x;
    int step_count_y;
    int step_count_z;
    int print_progress_percent;
    char last_fault_reason[64];
    int enter_count[8];  // indexed by Printer_State; sized for this machine's known state count
    int exit_count[8];
} PrinterContext;

#define PRINTER_STATE_LIST(STATE)                                                       \
    STATE(Printer, IDLE,      NULL,              on_event_idle,     NULL)               \
    STATE(Printer, HOMING,    on_enter_homing,    on_event_homing,    NULL)              \
    STATE(Printer, READY,     on_enter_ready,      on_event_ready,     NULL)             \
    STATE(Printer, PRINTING,  on_enter_printing,   on_event_printing,  on_exit_printing)  \
    STATE(Printer, PAUSED,    on_enter_paused,     on_event_paused,    NULL)              \
    STATE(Printer, ERROR,     on_enter_error,      on_event_error,     on_exit_error)

// Step 1: enum + typedefs. Printer_State now exists.
STATE_MACHINE_DECLARE_STATES(Printer, PRINTER_STATE_LIST)

// Step 2: forward-declare every handler named in the list above, now
// that Printer_State is a real type these declarations can return.
Printer_State on_event_idle(void* ctx, int event);
Printer_State on_event_homing(void* ctx, int event);
Printer_State on_event_ready(void* ctx, int event);
Printer_State on_event_printing(void* ctx, int event);
Printer_State on_event_paused(void* ctx, int event);
Printer_State on_event_error(void* ctx, int event);

void on_enter_homing(void* ctx);
void on_enter_ready(void* ctx);
void on_enter_printing(void* ctx);
void on_exit_printing(void* ctx);
void on_enter_paused(void* ctx);
void on_enter_error(void* ctx);
void on_exit_error(void* ctx);

// Step 3: the dispatch table, now that every handler above is declared.
STATE_MACHINE_DEFINE(Printer, PRINTER_STATE_LIST)
