#pragma once

#include "defines.h"

typedef enum mem_tag {
    //tagging for temp use: should be assigned one of the below or have a new tag created
    MEM_TAG_UNKNOWN,
    MEM_TAG_ARRAY,
    MEM_TAG_LINEAR_ALLOCATOR,
    MEM_TAG_DARRAY,
    MEM_TAG_DICT,
    MEM_TAG_RING_QUEUE,
    MEM_TAG_BST,
    MEM_TAG_STRING,
    MEM_TAG_APPLICATION,
    MEM_TAG_JOB,
    MEM_TAG_TEXTURE,
    MEM_TAG_MATERIAL_INSTANCE,
    MEM_TAG_RENDERER,
    MEM_TAG_GAME,
    MEM_TAG_TRANSFORM,
    MEM_TAG_ENTITY,
    MEM_TAG_ENTITY_NODE,
    MEM_TAG_SCENE,

    MEM_TAG_MAX_TAGS,
} mem_tag;

void init_mem_sys(u64 *mem_requirement, void *st);
void mem_sys_shutdown(void *st);

void* kallocate(u64 size, mem_tag tag);
void kfree(void *block, u64 size, mem_tag tag);
void* kzero_mem(void *block, u64 size);
void* kcopy_mem(void *dest, const void *src, u64 size);
void* kset_mem(void *dest, i32 val, u64 size);

char* get_mem_usage_str();
u64 get_mem_alloc_count();