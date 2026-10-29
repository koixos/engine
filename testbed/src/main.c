#include <core/logger.h>
#include <core/asserts.h>

int main(void) {
    KFATAL("imma doin -- %.2f", 3.14f);
    KERROR("imma doin testing -- %.2f", 3.14f);
    KWARN("imma doin testing -- %.2f", 3.14f);
    KINFO("imma doin testing -- %.2f", 3.14f);
    KDEBUG("imma doin testing -- %.2f", 3.14f);
    KTRACE("imma doin testing -- %.2f", 3.14f);

    KASSERT(1 == 0);
    
    return 0;
}