#pragma once

#include "defines.h"

KAPI u64 str_len(const char *str);

KAPI char* str_dup(const char *str);

KAPI b8 strs_equal(const char *str_a, const char *str_b);   //case-sensitive