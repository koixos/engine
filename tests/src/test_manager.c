#include "test_manager.h"

#include <core/logger.h>
#include <core/kstring.h>
#include <core/clock.h>

#include <containers/darray.h>

typedef struct test_entry {
    PFN_test func;
    char *desc;
} test_entry;

static test_entry *tests;

void test_manager_init() {
    tests = darray_create(test_entry);
}

void test_manager_register_test(PFN_test test, char *desc) {
    test_entry e;
    e.func = test;
    e.desc = desc;
    darray_push(tests, e);
}

void test_manager_run_tests() {
    u32 passed = 0, failed = 0, skipped = 0;
    u32 count = darray_length(tests);

    clock total_time;
    clock_start(&total_time);

    for (u32 i = 0; i < count; i++) {
        clock test_time;
        clock_start(&test_time);
        u8 res = tests[i].func();
        clock_update(&test_time);

        if (res == true) {
            ++passed;
        } else if (res == BYPASS) {
            KWARN("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        } else {
            KERROR("[FAILED]: %s", tests[i].desc);
            ++failed;
        }

        char stat[20];
        string_format(stat, failed ? "*** %d FAILED ***": "SUCCESS", failed);
        clock_update(&total_time);
        KINFO("Executed %d of %d (skipped %d) %s (%.6f sec / %.6f sec total)",
            i+1, count, skipped, stat, test_time.elapsed, total_time.elapsed);
    }

    clock_stop(&total_time);
    KINFO("Results: %d passed, %d failed, %d skipped.", passed, failed, skipped);
}
