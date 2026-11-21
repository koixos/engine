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

static input_state *state_ptr;

/* priv funcs */

// keyboard inp
b8 input_is_key_down(keys key);
b8 input_was_key_up(keys key);

//mouse inp
b8 input_is_btn_down(buttons btn);
b8 input_is_btn_up(buttons btn);
b8 input_was_btn_down(buttons btn);
b8 input_was_btn_up(buttons btn);

void input_get_mouse_pos(i32 *x, i32 *y);
void input_get_prev_mouse_pos(i32 *x, i32 *y);
/* ---------- */

void init_input_sys(u64 *mem_requirement, void *st) {
    *mem_requirement = sizeof(input_state);
    if (!st) return;
    kzero_mem(st, sizeof(input_state));
    state_ptr = st;
    KDEBUG("Input subsystem initialized.");
}

void input_sys_shutdown(void *st) {
    //TODO: add shutdown routines
    state_ptr = 0;
}

void input_update(f64 delta_time) {
    if (!state_ptr) return;
    //copy current states to previous states
    kcopy_mem(&state_ptr->keyboard_prev, &state_ptr->keyboard_curr, sizeof(keyboard_state));
    kcopy_mem(&state_ptr->mouse_prev, &state_ptr->mouse_curr, sizeof(mouse_state));
}

void input_process_key(keys key, b8 pressed) {
    //if state is actually changed
    if (state_ptr && state_ptr->keyboard_curr.keys[key] != pressed) {
        state_ptr->keyboard_curr.keys[key] = pressed;
        //fire of an event for immediate processing
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
    }
}

void input_process_btn(buttons btn, b8 pressed) {
    //if state is actually changed
    if (state_ptr->mouse_curr.btns[btn] != pressed) {
        state_ptr->mouse_curr.btns[btn] = pressed;
        //fire of an event for immediate processing
        event_context context;
        context.data.u16[0] = btn;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y) {
    //if state is actually changed
    if (state_ptr->mouse_curr.x != x || state_ptr->mouse_curr.y != y) {
        state_ptr->mouse_curr.x = x;
        state_ptr->mouse_curr.y = y;
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

b8 input_is_key_up(keys key) {
    return state_ptr ? !state_ptr->keyboard_curr.keys[key] : true;
}

b8 input_was_key_down(keys key) {
    return state_ptr ? state_ptr->keyboard_prev.keys[key] : false;
}

b8 input_is_key_down(keys key) {
    return state_ptr ? state_ptr->keyboard_curr.keys[key] : false;
}

b8 input_was_key_up(keys key) {
    return state_ptr ? !state_ptr->keyboard_prev.keys[key] : true;
}

b8 input_is_btn_down(buttons btn) {
    return state_ptr ? state_ptr->mouse_curr.btns[btn] : false;
}

b8 input_is_btn_up(buttons btn) {
    return state_ptr ? !state_ptr->mouse_curr.btns[btn] : true;
}

b8 input_was_btn_down(buttons btn) {
    return state_ptr ? state_ptr->mouse_prev.btns[btn] : false;
}

b8 input_was_btn_up(buttons btn) {
    return state_ptr ? !state_ptr->mouse_prev.btns[btn] : true;
}

void input_get_mouse_pos(i32 *x, i32 *y) {
    if (!state_ptr) {
        *x = 0; *y = 0;
        return;
    }

    *x = state_ptr->mouse_curr.x;
    *y = state_ptr->mouse_curr.y;
}

void input_get_prev_mouse_pos(i32 *x, i32 *y) {
    if (!state_ptr) {
        *x = 0; *y = 0;
        return;
    }

    *x = state_ptr->mouse_prev.x;
    *y = state_ptr->mouse_prev.y;
}