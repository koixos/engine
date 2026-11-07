#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "kmemory.h"
#include "event.h"
#include "input.h"
#include "clock.h"

#include "platform/platform.h"

#include "renderer/renderer_frontend.h"

typedef struct app_state {
    game *game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    clock clock;
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

    //renderer setup
    if (!renderer_init(game_inst->app_config.name, &app_st.platform)) {
        KFATAL("Failed to initialize renderer, aborting application!");
        return FALSE;
    }

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
    clock_start(&app_st.clock);
    clock_update(&app_st.clock);
    app_st.last_time = app_st.clock.elapsed;

    u8 frame_count = 0;
    f64 running_time = 0;
    f64 target_frame_secs = 1.0f / 60;

    KINFO(get_mem_usage_str());
    
    while (app_st.is_running) {
        if (!platform_pump_msgs(&app_st.platform))
            app_st.is_running = FALSE;

        if (!app_st.is_suspended) {
            //update clk and get delta time
            clock_update(&app_st.clock);

            f64 curr_time = app_st.clock.elapsed;
            f64 delta = curr_time - app_st.last_time;
            f64 frame_start_time = platform_get_absolute_time();

            if (!app_st.game_inst->update(app_st.game_inst, (f32)delta)) {
                KFATAL("Game update failed, shutting down!");
                app_st.is_running = FALSE;
                break;
            }

            if (!app_st.game_inst->render(app_st.game_inst, (f32)delta)) {
                KFATAL("Game render failed, shutting down!");
                app_st.is_running = FALSE;
                break;
            }

            //TODO: this is gross
            renderer_packet pkt;
            pkt.delta_time = delta;
            renderer_draw_frame(&pkt);

            //figure out how long the frame took
            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_secs = target_frame_secs - frame_elapsed_time;

            //if there is time left, give it back to the os
            if (remaining_secs > 0) {
                u64 remaining_ms = remaining_secs * 1000;
                //TODO: this may be necessary for release builds, thus turned off
                b8 limit_frames = FALSE;
                if (remaining_ms > 0 && limit_frames)
                    platform_sleep(remaining_ms - 1);
                frame_count++;
            }

            //inp update/state copying should always be handled after
            // any input is recorded.
            //as a safety, inp is the last thing to be updated before
            // this frame ends
            input_update(delta);

            //update last time
            app_st.last_time = curr_time;
        }
    }

    app_st.is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, app_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, app_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, app_on_key);

    event_shutdown();
    input_shutdown();
    renderer_shutdown();
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

void app_get_framebuf_size(u32 *width, u32 *height) {
    *width = app_st.width;
    *height = app_st.height;
}