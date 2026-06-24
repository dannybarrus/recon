#pragma once

// A generic, declarative-looking finite state machine framework built
// on a function-pointer dispatch table -- the bedrock pattern behind
// most embedded control logic. A motion controller's "where am I, and
// what do I do with this event" question is answered by an array
// lookup and a function call, not a sprawling switch statement that
// grows linearly with every new state someone adds.
//
// ─── Why a dispatch table instead of a switch statement ────────────────────
//
// A switch-based state machine works, but it has a real maintenance
// cost: every state's logic lives in one giant function, and adding a
// state means editing that function's switch arms directly, in the
// middle of everyone else's logic. A dispatch table inverts this --
// each state's behavior is its own small, independently testable
// function, and "adding a state" means adding one line to a list, not
// editing a shared function body at all.
//
// ─── How to use this (two steps, not one) ──────────────────────────────────
//
// 1. Define your own list-macro enumerating every state, then invoke
//    STATE_MACHINE_DECLARE_STATES. This generates the state enum
//    (Printer_IDLE, Printer_HOMING, ..., Printer_STATE_COUNT) and the
//    handler function-pointer typedefs, but NOT the dispatch table
//    yet.
//
//        #define PRINTER_STATE_LIST(STATE)
//            STATE(Printer, IDLE,   NULL,          on_idle,   NULL)
//            STATE(Printer, HOMING, on_enter_home, on_homing, NULL)
//
//    (Each STATE(...) line in a real list-macro ends with a backslash
//    line-continuation, omitted here only so this comment itself
//    doesn't trip a "multi-line comment" compiler warning.)
//
//        STATE_MACHINE_DECLARE_STATES(Printer, PRINTER_STATE_LIST)
//
// 2. NOW forward-declare your on_enter/on_event/on_exit functions --
//    this has to happen here, after step 1 and before step 3, because
//    these declarations need the Printer_State enum type that step 1
//    just created, and step 3's dispatch table needs these functions
//    already declared so it knows their addresses' types.
//
//      Printer_State on_idle(void* ctx, int event);
//      Printer_State on_homing(void* ctx, int event);
//      void on_enter_home(void* ctx);
//
// 3. Invoke STATE_MACHINE_DEFINE with the SAME name and state list.
//    This generates the actual dispatch table plus:
//      - Printer_dispatch(current, ctx, event) -- runs the current
//        state's on_event handler, and if it returns a DIFFERENT
//        state, runs the old state's on_exit (if any) then the new
//        state's on_enter (if any).
//      - Printer_state_name(state) -- for logging/debugging
//
//      STATE_MACHINE_DEFINE(Printer, PRINTER_STATE_LIST)
//
// 4. Write the actual function bodies, typically in a .c file.

#define STATE_MACHINE_ENUM_ENTRY(PREFIX, STATE_NAME, ON_ENTER, ON_EVENT, ON_EXIT) \
    PREFIX##_##STATE_NAME,

#define STATE_MACHINE_TABLE_ENTRY(PREFIX, STATE_NAME, ON_ENTER, ON_EVENT, ON_EXIT) \
    {#STATE_NAME, (ON_ENTER), (ON_EVENT), (ON_EXIT)},

#define STATE_MACHINE_DECLARE_STATES(NAME, STATE_LIST)                       \
    typedef enum {                                                            \
        STATE_LIST(STATE_MACHINE_ENUM_ENTRY)                                  \
        NAME##_STATE_COUNT                                                    \
    } NAME##_State;                                                           \
                                                                                \
    typedef NAME##_State (*NAME##_EventHandler)(void* ctx, int event);        \
    typedef void (*NAME##_Action)(void* ctx);

#define STATE_MACHINE_DEFINE(NAME, STATE_LIST)                               \
    typedef struct {                                                          \
        const char* name;                                                     \
        NAME##_Action on_enter;                                               \
        NAME##_EventHandler on_event;                                         \
        NAME##_Action on_exit;                                                \
    } NAME##_StateInfo;                                                       \
                                                                                \
    static const NAME##_StateInfo NAME##_states[] = {                        \
        STATE_LIST(STATE_MACHINE_TABLE_ENTRY)                                 \
    };                                                                        \
                                                                                \
    /* Runs the current state's event handler, then -- only if it     */     \
    /* actually returned a DIFFERENT state -- runs the outgoing       */     \
    /* state's on_exit (if any) and the incoming state's on_enter (if */     \
    /* any). An event a state doesn't care about should have its      */     \
    /* on_event handler simply return the same state it was given;    */     \
    /* no exit/enter actions fire for that case, since nothing         */    \
    /* actually changed.                                               */   \
    static inline NAME##_State NAME##_dispatch(NAME##_State current, void* ctx, int event) { \
        NAME##_State next = NAME##_states[current].on_event(ctx, event);     \
        if (next != current) {                                               \
            if (NAME##_states[current].on_exit) {                            \
                NAME##_states[current].on_exit(ctx);                         \
            }                                                                 \
            if (NAME##_states[next].on_enter) {                              \
                NAME##_states[next].on_enter(ctx);                           \
            }                                                                 \
        }                                                                     \
        return next;                                                          \
    }                                                                          \
                                                                                \
    static inline const char* NAME##_state_name(NAME##_State state) {        \
        return NAME##_states[state].name;                                     \
    }
