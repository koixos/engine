#include "input.h"
#include "event.h"
#include "kmemory.h"
#include "logger.h"

typedef struct keyboard_state {
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state {
    i16 x;
    i16 y;
    u8 btns[BUTTON_MAX_BTNS];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_curr;
    keyboard_state keyboard_prev;
    mouse_state mouse_curr;
    mouse_state mouse_prev;
} input_state;

static b8 is_inited = FALSE;
static input_state state = { };

void init_input() {
    kzero_mem(&state, sizeof(input_state));
    is_inited = TRUE;
    KINFO("Input subsystem initialized.");
}

void input_shutdown() {
    //TODO: add shutdown routines
    is_inited = FALSE;
}

void input_update(f64 delta_time) {
    if (!is_inited)
        return;

    //copy current states to previous states
    kcopy_mem(&state.keyboard_prev, &state.keyboard_curr, sizeof(keyboard_state));
    kcopy_mem(&state.mouse_prev, &state.mouse_curr, sizeof(mouse_state));
}

//keyboard inp

b8 input_is_key_down(keys key) {
    if (!is_inited)
        return FALSE;
    return state.keyboard_curr.keys[key] == TRUE;
}

b8 input_is_key_up(keys key) {
    if (!is_inited)
        return TRUE;
    return state.keyboard_curr.keys[key] == FALSE;
}

b8 input_was_key_down(keys key) {
    if (!is_inited)
        return FALSE;
    return state.keyboard_prev.keys[key] == TRUE;
}

b8 input_was_key_up(keys key) {
    if (!is_inited)
        return TRUE;
    return state.keyboard_prev.keys[key] == FALSE;
}

void input_process_key(keys key, b8 pressed) {
    //if state is actually changed
    if (state.keyboard_curr.keys[key] != pressed) {
        state.keyboard_curr.keys[key] = pressed;

        //fire of an event for immediate processing
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

//mouse inp

b8 input_is_btn_down(buttons btn) {
    if (!is_inited)
        return FALSE;
    return state.mouse_curr.btns[btn] == TRUE;
}

b8 input_is_btn_up(buttons btn) {
    if (!is_inited)
        return TRUE;
    return state.mouse_curr.btns[btn] == FALSE;
}

b8 input_was_btn_down(buttons btn) {
    if (!is_inited)
        return FALSE;
    return state.mouse_prev.btns[btn] == TRUE;
}

b8 input_was_btn_up(buttons btn) {
    if (!is_inited)
        return TRUE;
    return state.mouse_prev.btns[btn] == FALSE;
}

void input_get_mouse_pos(i32 *x, i32 *y) {
    if (!is_inited) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_curr.x;
    *y = state.mouse_curr.y;
}

void input_get_prev_mouse_pos(i32 *x, i32 *y) {
    if (!is_inited) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_prev.x;
    *y = state.mouse_prev.y;
}

void input_process_btn(buttons btn, b8 pressed) {
    //if state is actually changed
    if (state.mouse_curr.btns[btn] != pressed) {
        state.mouse_curr.btns[btn] = pressed;

        //fire of an event for immediate processing
        event_context context;
        context.data.u16[0] = btn;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y) {
    //if state is actually changed
    if (state.mouse_curr.x != x || state.mouse_curr.y != y) {
        KDEBUG("Mouse pos: %i, %i", x, y);

        state.mouse_curr.x = x;
        state.mouse_curr.y = y;

        //fire of an event for immediate processing
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(i8 z_delta) {
    //fire of an event for immediate processing
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}