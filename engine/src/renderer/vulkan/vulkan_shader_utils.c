#include "vulkan_shader_utils.h"

#include "core/kstring.h"
#include "core/logger.h"
#include "core/kmemory.h"

#include "platform/filesystem.h"

b8 create_shader_module(
    vulkan_context *context, 
    const char *name, 
    const char *type_str, 
    VkShaderStageFlagBits shader_stage_flag,
    u32 stage_ind,
    vulkan_shader_stage *shader_stages
) {
    char file_name[512];
    string_format(file_name, "assets/shaders/%s.%s.spv", name, type_str);

    kzero_mem(&shader_stages[stage_ind].create_info, sizeof(VkShaderModuleCreateInfo));
    shader_stages[stage_ind].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    //obtain file handle
    file_handle handle;
    if (!filesystem_open(file_name, FILE_MODE_READ, true, &handle)) {
        KERROR("Unable to open shader module: '%s'", file_name);
        return false;
    }

    //read the entire file as binary
    u64 size = 0;
    u8 *file_buf = 0;
    if (!filesystem_read_all_bytes(&handle, &file_buf, &size)) {
        KERROR("Unable to binary read shader module: '%s'", file_name);
        return false;
    }

    shader_stages[stage_ind].create_info.codeSize = size;
    shader_stages[stage_ind].create_info.pCode = (u32 *)file_buf;

    filesystem_close(&handle);

    VK_CHECK(vkCreateShaderModule(
        context->dev.logical_dev,
        &shader_stages[stage_ind].create_info,
        context->allocator,
        &shader_stages[stage_ind].handle
    ));

    //shader stage info
    kzero_mem(&shader_stages[stage_ind].shader_stage_create_info, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[stage_ind].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[stage_ind].shader_stage_create_info.stage = shader_stage_flag;
    shader_stages[stage_ind].shader_stage_create_info.module = shader_stages[stage_ind].handle;
    shader_stages[stage_ind].shader_stage_create_info.pName = "main";   //entry point of the shader module
                                                                        // main is std name
    if (file_buf) {
        kfree(file_buf, sizeof(u8) * size, MEM_TAG_STRING);
        file_buf = 0;
    }

    return true;
}