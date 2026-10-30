#include "kmemory.h"
#include "core/logger.h"
#include "platform/platform.h"

//TODO: custom str lib
#include <string.h>
#include <stdio.h>

struct mem_stats {
    u64 total_allocated;
    u64 tagged_allocations[MEM_TAG_MAX_TAGS];
} mem_stats;

static const char *mem_tag_strs[MEM_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "DARRAY     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      "
};

static struct mem_stats stats;

void init_mem() {
    platform_zero_mem(&stats, sizeof(stats));
}

void shutdown_mem() {

}

void* kallocate(u64 size, mem_tag tag) {
    if (tag == MEM_TAG_UNKNOWN)
        KWARN("kallocate called using MEM_TAG_UNKNOWN, re-class this allocation");

    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;

    //TODO: mem alingment
    void *block = platform_allocate(size, FALSE);
    platform_zero_mem(block, size);
    return block;
}

void kfree(void *block, u64 size, mem_tag tag) {
    if (tag == MEM_TAG_UNKNOWN)
        KWARN("kfree called using MEM_TAG_UNKNOWN, re-class this allocation");

    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;

    //TODO: mem alingment
    platform_free(block, FALSE);
}

void* kzero_mem(void *block, u64 size) {
    return platform_zero_mem(block, size);
}

void* kcopy_mem(void *dest, const void *src, u64 size) {
    return platform_copy_mem(dest, src, size);
}

void* kset_mem(void *dest, i32 val, u64 size) {
    return platform_set_mem(dest, val, size);
}

char* get_mem_usage_str() {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buf[8000] = "System memory use (tagged):\n";
    u64 offset = strlen(buf);
    
    for (u32 i = 0; i < MEM_TAG_MAX_TAGS; i++) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[1] >= gib) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gib;
        } else if (stats.tagged_allocations[1] >= mib) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mib;
        } else if (stats.tagged_allocations[1] >= kib) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.tagged_allocations[i];
        }

        i32 written = snprintf(buf + offset, 8000, "  %s: %.2f%s\n", mem_tag_strs[i], amount, unit);
        offset += written;
    }

    //TODO: _strdup(buf) for Windows - check platform with ifndef
    char *out_str = strdup(buf);
    return out_str;
}