#pragma once

#include "defines.h"

/**
 * Mem Layout
 *   u64 cap = # of elements that can be held
 *   u64 len = # of elements currently contained
 *   u64 stride = size of each element in bytes
 */

enum {
    DARR_CAP,
    DARR_LEN,
    DARR_STRIDE,
    DARR_FIELD_LEN
};

KAPI void* _darray_create(u64 len, u64 stride);

KAPI void _darray_destroy(void *arr);

KAPI u64 _darray_field_get(void *arr, u64 field);

KAPI void _darray_field_set(void *arr, u64 field, u64 val);

KAPI void* _darray_resize(void *arr);

KAPI void* _darray_push(void *arr, const void *val_ptr);

KAPI void _darray_pop(void *arr, void *dest);

KAPI void* _darray_pop_at(void *arr, u64 ind, void *dest);

KAPI void* _darray_insert_at(void *arr, u64 ind, void *val_ptr);

#define DARR_DEFAULT_CAP 1
#define DARR_RESIZE_FACTOR 2

#define darray_create(type) \
    _darray_create(DARR_DEFAULT_CAP, sizeof(type))

#define darray_reserve(type, cap) \
    _darray_create(cap, sizeof(type))

#define darray_destroy(arr) _darray_destroy(arr)

#define darray_push(arr, val)               \
    {                                       \
        typeof(val) temp = val;             \
        arr = _darray_push(arr, &temp);     \
    }

#define darray_pop(arr, val_ptr) _darray_pop(arr, val_ptr)

#define darray_insert_at(arr, ind, val)             \
    {                                               \
        typeof(val) temp = val;                     \
        arr = _darray_insert_at(arr, ind, &temp);   \
    }

#define darray_pop_at(arr, ind, val_ptr) _darray_pop_at(arr, ind, val_ptr)

#define darray_clear(arr) \
    _darray_field_set(arr, DARR_LEN, 0)

#define darray_capacity(arr) \
    _darray_field_get(arr, DARR_CAP)

#define darray_length(arr) \
    _darray_field_get(arr, DARR_LEN)

#define darray_stride(arr) \
    _darray_field_get(arr, DARR_STRIDE)

#define darray_length_set(arr, val) \
    _darray_field_set(arr, DARR_LEN, val)