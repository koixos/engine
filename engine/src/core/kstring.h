#pragma once

#include "defines.h"

//returns length of the given str
KAPI u64 str_len(const char *str);

KAPI char* str_dup(const char *str);

//case-sensitive str comparison - TRUE if same, otherwise FALSE
KAPI b8 strs_equal(const char *str_a, const char *str_b);   

//performs str formatting to dest given format str and params
KAPI i32 string_format(char *dest, const char *format, ...);

/**
 * Performs variadic str formatting to dest given format str and va_list
 * @param dest The destination for the formatted str
 * @param format The str to be formatted
 * @param va_list The variadic arg list
 * @returns The size of the data written
 */
KAPI i32 string_format_v(char *dest, const char *format, void *va_list);