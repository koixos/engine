#include "kstring.h"
#include "kmemory.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

u64 str_len(const char *str) {
    //TODO: write your own
    return strlen(str);
}

b8 strs_equal(const char *str_a, const char *str_b) {
    //TODO: write ur own
    return strcmp(str_a, str_b) == 0;
}

i32 string_format(char *dest, const char *format, ...) {
    if (dest) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}

i32 string_format_v(char *dest, const char *format, void *va_list) {
    if (dest) {
        //dont question too much <3
        char buf[32000];
        i32 written = vsnprintf(buf, 32000, format, va_list);
        buf[written] = 0;
        kcopy_mem(dest, buf, written + 1);
        return written;
    }
    return -1;
}

char* str_dup(const char *str) {
    u64 len = str_len(str);
    char *copy = kallocate(len + 1, MEM_TAG_STRING);
    kcopy_mem(copy, str, len + 1);
    return copy;
}