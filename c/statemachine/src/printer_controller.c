#include "recon/printer_controller.h"

#include <string.h>

// ─── on_event handlers ──────────────────────────────────────────────────────
// Each one returns the next state for the given event, or the SAME
// state if this state doesn't care about that event. FAULT_DETECTED is
// handled identically in every non-ERROR state -- the "stop everything
// now" path real motion control firmware needs from almost anywhere.

Printer_State on_event_idle(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_CMD_HOME: return Printer_HOMING;
        case EVENT_FAULT_DETECTED: return Printer_ERROR;
        default: return Printer_IDLE;
    }
}

Printer_State on_event_homing(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_HOME_DONE: return Printer_READY;
        case EVENT_FAULT_DETECTED: return Printer_ERROR;
        default: return Printer_HOMING;
    }
}

Printer_State on_event_ready(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_CMD_START_PRINT: return Printer_PRINTING;
        case EVENT_CMD_HOME: return Printer_HOMING;
        case EVENT_FAULT_DETECTED: return Printer_ERROR;
        default: return Printer_READY;
    }
}

Printer_State on_event_printing(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_CMD_PAUSE: return Printer_PAUSED;
        case EVENT_CMD_STOP: return Printer_READY;
        case EVENT_FAULT_DETECTED: return Printer_ERROR;
        default: return Printer_PRINTING;
    }
}

Printer_State on_event_paused(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_CMD_RESUME: return Printer_PRINTING;
        case EVENT_CMD_STOP: return Printer_READY;
        case EVENT_FAULT_DETECTED: return Printer_ERROR;
        default: return Printer_PAUSED;
    }
}

Printer_State on_event_error(void* ctx, int event) {
    (void)ctx;
    switch (event) {
        case EVENT_FAULT_CLEARED: return Printer_IDLE;
        default: return Printer_ERROR;
    }
}

// ─── on_enter / on_exit actions ────────────────────────────────────────────
// These are where real hardware actions would actually live in a real
// firmware build (issuing step pulses, engaging a relay, and so on).
// Here, each one updates the shared context so its effect is directly
// observable and testable.

void on_enter_homing(void* ctx) {
    PrinterContext* p = (PrinterContext*)ctx;
    p->step_count_x = 0;
    p->step_count_y = 0;
    p->step_count_z = 0;
    p->enter_count[Printer_HOMING]++;
}

void on_enter_ready(void* ctx) {
    PrinterContext* p = (PrinterContext*)ctx;
    p->enter_count[Printer_READY]++;
}

void on_enter_printing(void* ctx) {
    // Fires for BOTH a fresh start (READY -> PRINTING) and a resume
    // (PAUSED -> PRINTING) -- deliberately does NOT reset
    // print_progress_percent, since resuming a paused print should not
    // discard progress already made.
    PrinterContext* p = (PrinterContext*)ctx;
    p->enter_count[Printer_PRINTING]++;
}

void on_exit_printing(void* ctx) {
    // A real system would persist a resumable checkpoint here. This
    // records that the action ran, which is what the test suite
    // actually verifies.
    PrinterContext* p = (PrinterContext*)ctx;
    p->exit_count[Printer_PRINTING]++;
}

void on_enter_paused(void* ctx) {
    PrinterContext* p = (PrinterContext*)ctx;
    p->enter_count[Printer_PAUSED]++;
}

void on_enter_error(void* ctx) {
    // The caller is responsible for setting ctx->last_fault_reason
    // BEFORE raising EVENT_FAULT_DETECTED -- the dispatch signature is
    // deliberately just (ctx, int event) with no event payload, so any
    // event-specific detail travels through the shared context instead
    // of a richer event type. A larger system would likely want a real
    // tagged event payload; this keeps the dispatch mechanism itself
    // simple, which is the actual point of this module.
    PrinterContext* p = (PrinterContext*)ctx;
    p->enter_count[Printer_ERROR]++;
}

void on_exit_error(void* ctx) {
    PrinterContext* p = (PrinterContext*)ctx;
    p->exit_count[Printer_ERROR]++;
    memset(p->last_fault_reason, 0, sizeof(p->last_fault_reason));
}
