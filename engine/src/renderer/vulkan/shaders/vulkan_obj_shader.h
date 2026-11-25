#pragma once

#include "renderer/vulkan/vulkan_types.inl"

#include "renderer/renderer_types.inl"

b8 vulkan_obj_shader_create(vulkan_context *context, vulkan_obj_shader *out_shader);
void vulkan_obj_shader_destroy(vulkan_context *context, vulkan_obj_shader *shader);
void vulkan_obj_shader_use(vulkan_context *context, vulkan_obj_shader *shader);
void vulkan_obj_shader_update_global_st(vulkan_context *context, vulkan_obj_shader *shader);

//TODO: temp
void vulkan_obj_shader_upd_obj(vulkan_context *context, vulkan_obj_shader *shader, mat4 model);
