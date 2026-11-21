#include "test_manager.h"

#include "memory/linear_allocator_tests.h"

#include <core/logger.h>

int main() {
    //alw init the manager first
    test_manager_init();

    /* add test registrations below */

    linear_allocator_register_tests();

    KDEBUG("Starting tests...");
    test_manager_run_tests();

    return 0;
}