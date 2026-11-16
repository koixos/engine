#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "kmemory.h"
#include "event.h"
#include "input.h"
#include "clock.h"

#include "platform/platform.h"

#include "memory/linear_allocator.h"

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
    linear_allocator systems_allocator;

    //subsystems

    u64 memory_sys_mem_requirement;
    void *memory_sys_st;

    u64 logging_sys_mem_requirement;
    void *logging_sys_st;
} app_state;

static app_state *app_st;

//event handlers
b8 app_on_event(u16 code, void *sender, void *listener_inst, event_context context);
b8 app_on_key(u16 code, void *sender, void *listener_inst, event_context context);
b8 app_on_resized(u16 code, void *sender, void *listener_inst, event_context context);
 
b8 app_create(game *game_inst) {
    if (game_inst->app_state) {
        KERROR("app_create called more than once.");
        return false;
    }

    //TODO: what does this mean??
    game_inst->app_state = kallocate(sizeof(app_state), MEM_TAG_APPLICATION);
    app_st = game_inst->app_state;
    app_st->game_inst = game_inst;
    app_st->is_running = false;
    app_st->is_suspended = false;

    u64 systems_alloc_total_size = 64 * 1024 * 1024;    //64 mb
    linear_allocator_create(systems_alloc_total_size, 0, &app_st->systems_allocator);

    /* init subsystems below */

    init_mem(&app_st->memory_sys_mem_requirement, 0);
    app_st->memory_sys_st = 
        linear_allocator_allocate(&app_st->systems_allocator, app_st->memory_sys_mem_requirement);
    if (!init_mem(&app_st->memory_sys_mem_requirement, app_st->memory_sys_st)) {
        KERROR("Failed to init memory system; shutting down.");
        return false;
    }

    init_logging(&app_st->logging_sys_mem_requirement, 0);
    app_st->logging_sys_st = 
        linear_allocator_allocate(&app_st->systems_allocator, app_st->logging_sys_mem_requirement);
    if (!init_logging(&app_st->logging_sys_mem_requirement, app_st->logging_sys_st)) {
        KERROR("Failed to init logging system; shutting down.");
        return false;
    }

    init_input();

    if (!event_init()) {
        KERROR("Event system failed initialization. Application cannot continue.");
        return false;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, 0, app_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, app_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, app_on_key);
    event_register(EVENT_CODE_RESIZED, 0, app_on_resized);

    if (!platform_startup(
            &app_st->platform,
            game_inst->app_config.name,
            game_inst->app_config.start_pos_x,
            game_inst->app_config.start_pos_y,
            game_inst->app_config.start_width,
            game_inst->app_config.start_height))
        return false;

    //renderer setup
    if (!renderer_init(game_inst->app_config.name, &app_st->platform)) {
        KFATAL("Failed to initialize renderer, aborting application!");
        return false;
    }

    //init game
    if (!game_inst->init(game_inst)) {
        KFATAL("Game failed to initialize!");
        return false;
    }

    game_inst->on_resize(game_inst, app_st->width, app_st->height);

    return true;
}

b8 app_run() {
    app_st->is_running = true;

    clock_start(&app_st->clock);
    clock_update(&app_st->clock);
    app_st->last_time = app_st->clock.elapsed;

    u8 frame_count = 0;
    f64 running_time = 0;
    f64 target_frame_secs = 1.0f / 60;

    KINFO(get_mem_usage_str());
    
    while (app_st->is_running) {
        if (!platform_pump_msgs(&app_st->platform))
            app_st->is_running = false;        

        if (!app_st->is_suspended) {
            //update clk and get delta time
            clock_update(&app_st->clock);

            f64 curr_time = app_st->clock.elapsed;
            f64 delta = curr_time - app_st->last_time;
            f64 frame_start_time = platform_get_absolute_time();

            if (!app_st->game_inst->update(app_st->game_inst, (f32)delta)) {
                KFATAL("Game update failed, shutting down!");
                app_st->is_running = false;
                break;
            }

            if (!app_st->game_inst->render(app_st->game_inst, (f32)delta)) {
                KFATAL("Game render failed, shutting down!");
                app_st->is_running = false;
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
                b8 limit_frames = false;
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
            app_st->last_time = curr_time;
        }
    }

    app_st->is_running = false;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, app_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, app_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, app_on_key);
    event_unregister(EVENT_CODE_RESIZED, 0, app_on_resized);

    event_shutdown();
    input_shutdown();
    renderer_shutdown();
    platform_shutdown(&app_st->platform);
    shutdown_mem(&app_st->memory_sys_st);

    return true;
}

b8 app_on_event(u16 code, void *sender, void *listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("EVENT_CODE_APPLICATION_QUIT received, shutting down.");
            app_st->is_running = false;
            return true;
        }
    }

    return false;
}

b8 app_on_key(u16 code, void *sender, void *listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_KEY_PRESSED: {
            u16 key_code = context.data.u16[0];
            if (key_code == KEY_ESCAPE) {
                //technically firing an event to itself, but there may be other listeners
                event_context data = { };
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                return true;
            }
        } break;
        case EVENT_CODE_KEY_RELEASED:
            break;
    }
    return false;
}

b8 app_on_resized(u16 code, void *sender, void *listener_inst, event_context context) {
    if (code == EVENT_CODE_RESIZED) {
        u16 width = context.data.u16[0];
        u16 height = context.data.u16[1];

        //check if diff, if so, trigger a resize event
        if (width != app_st->width || height != app_st->height) {
            app_st->width = width;
            app_st->height = height;
            
            KDEBUG("Window resized: %i, %i", width, height);

            //handle minimization
            if (width == 0 || height == 0) {
                KINFO("Window minimized, suspending the app...");
                app_st->is_suspended = true;
                return true;
            }

            if (app_st->is_suspended) {
                KINFO("Window restored, resuming the app.");
                app_st->is_suspended = false;
            }

            app_st->game_inst->on_resize(app_st->game_inst, width, height);
            renderer_on_resized(width, height);
        }
    }

    //event purposely not handled to allow other listeners to get this too
    return false;
}

void app_get_framebuf_size(u32 *width, u32 *height) {
    *width = app_st->width;
    *height = app_st->height;
}