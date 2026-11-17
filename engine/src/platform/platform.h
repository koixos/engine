#pragma once

#include "defines.h"

b8 platform_sys_startup(
    u64 *mem_requirement,
    void *st,
    const char *app_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height);

void platform_sys_shutdown(void *st);

b8 platform_pump_msgs();

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