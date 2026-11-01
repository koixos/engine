#include "clock.h"

#include "platform/platform.h"

void clock_start(clock *clk) {
    clk->start_time = platform_get_absolute_time();
    clk->elapsed = 0;
}

void clock_update(clock *clk) {
    if (clk->start_time != 0)
        clk->elapsed = platform_get_absolute_time() - clk->start_time;
}

void clock_stop(clock *clk) {
    clk->start_time = 0;
}
