#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "kmemory.h"
#include "event.h"
#include "input.h"

typedef struct app_state {
    game *game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} app_state;

static b8 inited = FALSE;
static app_state app_st;

//event handlers
b8 app_on_event(u16 code, void *sender, void *listener_inst, event_context context);
b8 app_on_key(u16 code, void *sender, void *listener_inst, event_context context);
 
b8 app_create(game *game_inst) {
    if (inited) {
        KERROR("app_create called more than once.");
        return FALSE;
    }

    app_st.game_inst = game_inst;

    //init subsystems
    init_logging();
    init_input();
    
    //TODO: remove these
    KFATAL("imma doin -- %.2f", 3.14f);
    KERROR("imma doin testing -- %.2f", 3.14f);
    KWARN("imma doin testing -- %.2f", 3.14f);
    KINFO("imma doin testing -- %.2f", 3.14f);
    KDEBUG("imma doin testing -- %.2f", 3.14f);
    KTRACE("imma doin testing -- %.2f", 3.14f);

    app_st.is_running = TRUE;
    app_st.is_suspended = FALSE;

    if (!event_init()) {
        KERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, app_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, app_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, app_on_key);

    if (!platform_startup(
            &app_st.platform,
            game_inst->app_config.name,
            game_inst->app_config.start_pos_x,
            game_inst->app_config.start_pos_y,
            game_inst->app_config.start_width,
            game_inst->app_config.start_height))
        return FALSE;

    //init game
    if (!app_st.game_inst->init(app_st.game_inst)) {
        KFATAL("Game failed to initialize!");
        return FALSE;
    }

    app_st.game_inst->on_resize(app_st.game_inst, app_st.width, app_st.height);

    inited = TRUE;

    return TRUE;
}

b8 app_run() {
    KINFO(get_mem_usage_str());
    
    while (app_st.is_running) {
        if (!platform_pump_msgs(&app_st.platform))
            app_st.is_running = FALSE;

        if (!app_st.is_suspended) {
            if (!app_st.game_inst->update(app_st.game_inst, (f32)0)) {
                KFATAL("Game update failed, shutting down!");
                break;
            }

            if (!app_st.game_inst->render(app_st.game_inst, (f32)0)) {
                KFATAL("Game render failed, shutting down!");
                break;
            }

            //inp update/state copying should always be handled after
            // any input is recorded.
            //as a safety, inp is the last thing to be updated before
            // this frame ends
            input_update(0);
        }
    }

    app_st.is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, app_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, app_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, app_on_key);

    event_shutdown();
    input_shutdown();
    platform_shutdown(&app_st.platform);

    return TRUE;
}

b8 app_on_event(u16 code, void *sender, void *listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.");
            app_st.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 app_on_key(u16 code, void *sender, void *listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_KEY_PRESSED: {
            u16 key_code = context.data.u16[0];
            if (key_code == KEY_ESCAPE) {
                //technically firing an event to itself, but there may be other listeners
                event_context data = { };
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                return TRUE;
            } else if (key_code == KEY_A)
                KDEBUG("Explicit - A key pressed!!!!");
            else
                KDEBUG("'%c' key pressed in window....", key_code);
        } break;
        case EVENT_CODE_KEY_RELEASED: {
            u16 key_code = context.data.u16[0];
            if (key_code == KEY_B)
                KDEBUG("Explicit - B key released!!!!");
            else
                KDEBUG("'%c' key released in window....", key_code);
        } break;
    }
    return FALSE;
}