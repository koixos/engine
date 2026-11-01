#include "darray.h"

#include "core/kmemory.h"
#include "core/logger.h"

void* _darray_create(u64 len, u64 stride) {
    u64 header_size = DARR_FIELD_LEN * sizeof(u64);
    u64 arr_size = len * stride;
    u64 *new_arr = kallocate(header_size + arr_size, MEM_TAG_DARRAY);
    kset_mem(new_arr, 0, header_size + arr_size);
    new_arr[DARR_CAP] = len;
    new_arr[DARR_LEN] = 0;
    new_arr[DARR_STRIDE] = stride;
    return (void *)(new_arr + DARR_FIELD_LEN);
}

void _darray_destroy(void *arr) {
    u64 *header = (u64 *)arr - DARR_FIELD_LEN;
    u64 header_size = DARR_FIELD_LEN * sizeof(u64);
    u64 totalsize = header_size + header[DARR_CAP] * header[DARR_STRIDE];
    kfree(header, totalsize, MEM_TAG_DARRAY);
}

u64 _darray_field_get(void *arr, u64 field) {
    u64 *header = (u64 *)arr - DARR_FIELD_LEN;
    return header[field];
}

void _darray_field_set(void *arr, u64 field, u64 val) {
    u64 *header = (u64 *)arr - DARR_FIELD_LEN;
    header[field] = val;
}

void* _darray_resize(void *arr) {
    u64 len = darray_length(arr);
    u64 stride = darray_stride(arr);
    void *temp = _darray_create(
        (DARR_RESIZE_FACTOR * darray_capacity(arr)),
        stride
    );
    kcopy_mem(temp, arr, len * stride);

    _darray_field_set(temp, DARR_LEN, len);
    _darray_destroy(arr);

    return temp;
}

void* _darray_push(void *arr, const void *val_ptr) {
    u64 len = darray_length(arr);
    u64 stride = darray_stride(arr);
    if (len >= darray_capacity(arr))
        arr = _darray_resize(arr);
    
    u64 addr = (u64)arr;
    addr += len * stride;
    kcopy_mem((void *)addr, val_ptr, stride);
    _darray_field_set(arr, DARR_LEN, len + 1);
    return arr;
}

void _darray_pop(void *arr, void *dest) {
    u64 len = darray_length(arr);
    u64 stride = darray_stride(arr);
    u64 addr = (u64)arr;
    addr += (len - 1) * stride;
    
    if (dest)
        kcopy_mem(dest, (void *)addr, stride);
    
    kzero_mem((void *)addr, stride);
    _darray_field_set(arr, DARR_LEN, len - 1);
}

void* _darray_pop_at(void *arr, u64 ind, void *dest) {
    u64 len = darray_length(arr);
    u64 stride = darray_stride(arr);
    if (ind >= len) {
        KERROR("Index outside the bounds of this array! Length: %i, index: %i", len, ind);
        return arr;
    }

    u64 addr = (u64)arr;
    kcopy_mem(dest, (void *)(addr + (ind * stride)), stride);
    kzero_mem((void *)(addr + (ind * stride)), stride);

    //if not on last elem, snip out the entry and copy the rest inward
    if (ind != len - 1) {
        kcopy_mem(
            (void *)(addr + (ind * stride)),
            (void *)(addr + ((ind + 1) * stride)),
            (len - ind) * stride
        );
    }

    _darray_field_set(arr, DARR_LEN, len - 1);
    return arr;
}

void* _darray_insert_at(void *arr, u64 ind, void *val_ptr) {
    u64 len = darray_length(arr);
    u64 stride = darray_stride(arr);
    if (ind >= len) {
        KERROR("Index outside the bounds of this array! Length: %i, index: %i", len, ind);
        return arr;
    }

    if (len >= darray_capacity(arr))
        arr = _darray_resize(arr);
    
    u64 addr = (u64)arr;

    //if not at last elem, copy the rest outward
    if (ind != len - 1) {
        kcopy_mem(
            (void *)(addr + ((ind + 1) * stride)),
            (void *)(addr + (ind * stride)),
            (len - ind) * stride
        );
    }

    kcopy_mem((void *)(addr + (ind * stride)), val_ptr, stride);
    _darray_field_set(arr, DARR_LEN, len + 1);
    
    return arr;
}