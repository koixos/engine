#pragma once

#include "defines.h"

typedef struct clock {
    f64 start_time;
    f64 elapsed;
} clock;

//starts the provided clk - resets elapsed time
void clock_start(clock *clk);

//updates the provided clk - should be called just before checking elapsed time
//has no effect on non-started clks
void clock_update(clock *clk);

//stops the provided clk - does not reset elapsed time
void clock_stop(clock *clk);

