#pragma once

#include "vulkan_types.inl"

/**
 * Returns the str representation of the result.
 * @param res The result to get the string for
 * @param get_extended Indicates whether to also return an extended result
 * @returns The error code and/or extended error msg in str form. Defaults to success for unknown result types. 
 */
const char* vulkan_result_string(VkResult res, b8 get_extended);

/**
 * Indicates if the passed result is a success or an error as defined by the VUlkan specification.
 * @returns True if success; otherwise false. Defaults to true for unknown resulty types.
 */
b8 vulkan_result_is_success(VkResult res);