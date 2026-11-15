#pragma once

#include "defines.h"

typedef struct linear_allocator {
    u64 total_size;
    u64 allocated;
    void *mem;
    b8 owns_mem;
} linear_allocator;

KAPI void linear_allocator_create(u64 total_size, void *mem, linear_allocator *out_allocator);
KAPI void linear_allocator_destroy(linear_allocator *allocator);

KAPI void* linear_allocator_allocate(linear_allocator *allocator, u64 size);
KAPI void linear_allocator_free_all(linear_allocator *allocator);