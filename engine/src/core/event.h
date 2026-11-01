#pragma once

#include "defines.h"

typedef struct event_context {
    //128 bytes
    union {
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
} event_context;

//should return true if handles
typedef b8 (*PFN_on_event)(u16 code, void *sender, void *listener_inst, event_context data);

b8 event_init();

void event_shutdown();

/**
 * Register to listen for when events are sent with the provided code.
 * Events with duplicate listener/callback combos will not be registered again and
 * will cause this to return FALSE.
 * @param code The event code to listen for.
 * @param listener A ptr to a listener instance. Can be 0/NULL.
 * @param on_event The callback fucntion ptr to be invoked when the event code is fired.
 * @returns TRUE if the event is successfully registered; otherwise FALSE.
 */
KAPI b8 event_register(u16 code, void *listener, PFN_on_event on_event);

/**
 * Unregister from listening for when events are sent with the provided code.
 * If no matching registration is found, returns FALSE.
 * @param code The event code to stop listening for.
 * @param listener A ptr to a listener instance. Can be 0/NULL.
 * @param on_event The callback fucntion ptr to be unregistered.
 * @returns TRUE if the event is successfully unregistered; otherwise FALSE.
 */
KAPI b8 event_unregister(u16 code, void *listener, PFN_on_event on_event);

/**
 * Fires an event to listeners of the provided code.
 * If an event handler returns TRUE, the event is considered handled and
 * is not passed on to any more listeners.
 * @param code The event code to fire.
 * @param listener A ptr to the sender. Can be 0/NULL.
 * @param data The event data.
 * @returns TRUE if handled, otherwise FALSE.
 */
KAPI b8 event_fire(u16 code, void *sender, event_context data);

//sys internal event codes, app should use codes beyond 255
typedef enum sys_event_code {
    //shuts the app down on the next frame
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    //keyboard key pressed
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_PRESSED = 0x02,

    //keyboard key released
    /* Context usage:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_RELEASED = 0x03,

    //mouse button pressed
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    //mouse button released
    /* Context usage:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    //mouse moved
    /* Context usage:
     * u16 x = data.data.u16[0];
     * u16 y = data.data.u16[1];
     */
    EVENT_CODE_MOUSE_MOVED = 0x06,

    //mouse moved
    /* Context usage:
     * u8 z_delta = data.data.u8[0];
     */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    //resized/resolution changed from OS
    /* Context usage:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[1];
     */
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODE = 0xFF
} sys_event_code;