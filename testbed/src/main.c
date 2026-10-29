#include <core/logger.h>
#include <core/asserts.h>

//TODO: test
#include <platform/platform.h>

int main(void) {
    KFATAL("imma doin -- %.2f", 3.14f);
    KERROR("imma doin testing -- %.2f", 3.14f);
    KWARN("imma doin testing -- %.2f", 3.14f);
    KINFO("imma doin testing -- %.2f", 3.14f);
    KDEBUG("imma doin testing -- %.2f", 3.14f);
    KTRACE("imma doin testing -- %.2f", 3.14f);

    //KASSERT(1 == 0);

    platform_state st;
    if (platform_startup(&st, "Engine Testbed", 100, 100, 1200, 720)) {
        while (TRUE) {
            platform_pump_msgs(&st);
        }
        
    }
    platform_shutdown(&st);

    return 0;
}