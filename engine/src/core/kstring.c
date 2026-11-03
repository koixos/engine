#include "kstring.h"
#include "kmemory.h"

#include <string.h>

u64 str_len(const char *str) {
    //TODO: write your own
    return strlen(str);
}

char* str_dup(const char *str) {
    u64 len = str_len(str);
    char *copy = kallocate(len + 1, MEM_TAG_STRING);
    kcopy_mem(copy, str, len + 1);
    return copy;
}

b8 strs_equal(const char *str_a, const char *str_b) {
    //TODO: write ur own
    return strcmp(str_a, str_b) == 0;
}