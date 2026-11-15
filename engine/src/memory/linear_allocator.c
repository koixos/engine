#include "linear_allocator.h"

#include "core/kmemory.h"
#include "core/logger.h"

void linear_allocator_create(u64 total_size, void *mem, linear_allocator *out_allocator) {
    if (!out_allocator)
        return;

    out_allocator->total_size = total_size;
    out_allocator->allocated = 0;
    out_allocator->owns_mem = (mem == 0);

    if (mem)
        out_allocator->mem = mem;
    else
        out_allocator->mem = kallocate(total_size, MEM_TAG_LINEAR_ALLOCATOR);
}

void linear_allocator_destroy(linear_allocator *allocator) {
    if (!allocator)
        return;

    allocator->total_size = 0;
    allocator->allocated = 0;

    if (allocator->owns_mem && allocator->mem)
        kfree(allocator->mem, allocator->total_size, MEM_TAG_LINEAR_ALLOCATOR);
    
    allocator->mem = 0;
    allocator->total_size = 0;
    allocator->owns_mem = false;
}

void* linear_allocator_allocate(linear_allocator *allocator, u64 size) {
    if (allocator && allocator->mem) {
        if (allocator->allocated + size > allocator->total_size) {
            u64 remaining = allocator->total_size - allocator->allocated;
            KFATAL("linear_allocator_allocate - tried to allocate %lluB while only %lluB remained!",
                size, remaining);
            return 0;
        }

        void *block = allocator->mem + allocator->allocated;
        allocator->allocated += size;
        return block;
    }

    KERROR("linear_allocator_allocate - provided allocator not initialized!");
    return 0;
}

void linear_allocator_free_all(linear_allocator *allocator) {
    if (allocator && allocator->mem) {
        allocator->allocated = 0;
        kzero_mem(allocator->mem, allocator->total_size);
    }
}