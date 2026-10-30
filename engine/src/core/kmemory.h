#pragma once

#include "defines.h"

typedef enum mem_tag {
    //for temporary use, should be assigned one of the below or have a new tag created
    MEM_TAG_UNKNOWN,
    MEM_TAG_ARRAY,
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

void init_mem();

void shutdown_mem();

KAPI void* kallocate(u64 size, mem_tag tag);

KAPI void kfree(void *block, u64 size, mem_tag tag);

KAPI void* kzero_mem(void *block, u64 size);

KAPI void* kcopy_mem(void *dest, const void *src, u64 size);

KAPI void* kset_mem(void *dest, i32 val, u64 size);

KAPI char* get_mem_usage_str();