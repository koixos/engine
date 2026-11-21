#pragma once

#include "defines.h"

typedef struct clock {
    f64 start_time;
    f64 elapsed;
} clock;

/**
 * Starts the provided clk - resets elapsed time
 */
void clock_start(clock *clk);

/**
 * Updates the provided clk - should be called just before checking elapsed time,
 *  and has no effect on non-started clks
 */
void clock_update(clock *clk);

/**
 * Stops the provided clk - does not reset elapsed time
 */
void clock_stop(clock *clk);