#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "core/kmemory.h"

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
 
b8 app_create(game *game_inst) {
    if (inited) {
        KERROR("app_create called more than once.");
        return FALSE;
    }

    app_st.game_inst = game_inst;

    //init subsystems
    init_logging();

    //TODO: remove these
    KFATAL("imma doin -- %.2f", 3.14f);
    KERROR("imma doin testing -- %.2f", 3.14f);
    KWARN("imma doin testing -- %.2f", 3.14f);
    KINFO("imma doin testing -- %.2f", 3.14f);
    KDEBUG("imma doin testing -- %.2f", 3.14f);
    KTRACE("imma doin testing -- %.2f", 3.14f);

    app_st.is_running = TRUE;
    app_st.is_suspended = FALSE;

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
        }
    }

    app_st.is_running = FALSE;

    platform_shutdown(&app_st.platform);

    return TRUE;
}