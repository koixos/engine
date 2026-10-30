#include "platform.h"

//Linux platform layer
#if KPLATFORM_LINUX

#include <core/logger.h>

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   //nanosleep
#else
#include <unistd.h> //usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct internal_state {
    Display *disp;
    xcb_connection_t *conn;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
} internal_state;

KAPI b8 platform_startup(
    platform_state *st,
    const char *app_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height) {

    //create internal st
    st->internal_st = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)st->internal_st;

    //connect to X
    state->disp = XOpenDisplay(NULL);

    //TODO: this action is global along whole OS, find a better way to avoid key repeats
    //turn off key repeats
    XAutoRepeatOff(state->disp);

    //retrieve the conn from disp
    state->conn = XGetXCBConnection(state->disp);

    if (xcb_connection_has_error(state->conn)) {
        KFATAL("Failed to connect to X server via XCB.");
        return FALSE;
    }

    //get data from X server
    const struct xcb_setup_t *setup = xcb_get_setup(state->conn);

    //loop through screens using iter
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 i = screen_p; i > 0; i--)
        xcb_screen_next(&it);
    
    //after screens have been looped through, assign it
    state->screen = it.data;

    //alloc an XID for window to be created 
    state->window = xcb_generate_id(state->conn);

    //filling window bg with a single clr
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    //register event types
    u32 event_vals = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                    XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                    XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                    XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    //values to be sent over XCB: bg clr, events
    u32 val_list[] = { state->screen->black_pixel, event_vals };

    //create window
    xcb_void_cookie_t cookie = xcb_create_window(
        state->conn,
        XCB_COPY_FROM_PARENT,           //depth
        state->window,
        state->screen->root,            //parent
        x,
        y,
        width,
        height,
        0,                              //no border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,  //class
        state->screen->root_visual,
        event_mask,
        val_list
    );

    //change title
    xcb_change_property(
        state->conn,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,  //data shoukd be viewed 8 bits at a time
        strlen(app_name),
        app_name
    );

    //tell server to notify when window manager
    //attempts to kill the window
    xcb_intern_atom_cookie_t wm_del_cookie = xcb_intern_atom(
        state->conn,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        state->conn,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t *wm_del_reply = xcb_intern_atom_reply(
        state->conn,
        wm_del_cookie,
        NULL
    );

    xcb_intern_atom_reply_t *wm_protocols_reply = xcb_intern_atom_reply(
        state->conn,
        wm_protocols_cookie,
        NULL
    );

    state->wm_delete_win = wm_del_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(
        state->conn,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocols_reply->atom,
        4,
        32,
        1,
        &wm_del_reply->atom
    );

    //map window to screen
    xcb_map_window(state->conn, state->window);

    //flush the stream
    i32 stream_res = xcb_flush(state->conn);
    if (stream_res <= 0) {
        KFATAL("An error occurred when flushing the stream: %d", stream_res);
        return FALSE;
    }

    return TRUE;
}

KAPI void platform_shutdown(platform_state *st) {
    //cold-cast to the known type
    internal_state *state = (internal_state *)st->internal_st;
    
    //dont forget to turn key repeats back on 
    XAutoRepeatOn(state->disp);
    
    xcb_destroy_window(state->conn, state->window);
}

b8 platform_pump_msgs(platform_state *st) {
    //cold-cast to the known type
    internal_state *state = (internal_state *)st->internal_st;
    
    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    b8 quit_flagged = FALSE;

    while (event != 0) {
        event = xcb_poll_for_event(state->conn);
        if (event == 0)
            break;

        //inp events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                //TODO: key presses and releases
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                //TODO: mouse button presses and releases
            } break;
            case XCB_MOTION_NOTIFY:
                //TODO: mouse movement
                break;
            case XCB_CONFIGURE_NOTIFY: {
                //TODO: resizing
            } break;
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;
                //window close
                if (cm->data.data32[0] == state->wm_delete_win)
                    quit_flagged = TRUE;
            } break;
            default:
                break;
        }

        free(event);
    }
    
    return !quit_flagged;
}

void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void *block, b8 aligned) {
    free(block);
} 

void* platform_zero_mem(void *block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_mem(void *dest, const void *src, u64 size) {
    return memcpy(dest, src, size);
}

void* platform_set_mem(void *dest, i32 val, u64 size) {
    return memset(dest, val, size);
}

void platform_console_write(const char *msg, u8 clr) {
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char *clr_strs[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", clr_strs[clr], msg);
}

void platform_console_write_error(const char *msg, u8 clr) {
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char *clr_strs[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", clr_strs[clr], msg);
}

f64 platform_get_absolute_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

//sleep on the thread for provided ms - this blocks main thread
// should only be used to give time back to OS for unused update power
// thus it is not exported
void platform_sleep(u64 ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000)
        sleep(ms / 1000);
    usleep((ms % 1000) * 1000);
#endif
}

#endif //KPLATFORM_LINUX