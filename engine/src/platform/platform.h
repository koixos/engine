#pragma once

#include "defines.h"

typedef struct platform_state {
    void *internal_st;
} platform_state;

b8 platform_startup(
    platform_state *st,
    const char *app_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

void platform_shutdown(platform_state *st);

b8 platform_pump_msgs(platform_state *st);

void* platform_allocate(u64 size, b8 aligned);

void platform_free(void *block, b8 aligned);

void* platform_zero_mem(void *block, u64 size);

void* platform_copy_mem(void *dest, const void *src, u64 size);

void* platform_set_mem(void *dest, i32 val, u64 size);

void platform_console_write(const char *msg, u8 clr);

void platform_console_write_error(const char *msg, u8 clr);

f64 platform_get_absolute_time();

//sleep on the thread for provided ms - this blocks main thread
// should only be used to give time back to OS for unused update power
// thus it is not exported
void platform_sleep(u64 ms);