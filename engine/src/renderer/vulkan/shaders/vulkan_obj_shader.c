#include "vulkan_obj_shader.h"

#include "core/logger.h"
#include "core/kmemory.h"

#include "renderer/vulkan/vulkan_shader_utils.h"
#include "renderer/vulkan/vulkan_pipeline.h"

#include "math/math_types.h"

#define BUILTIN_SHADER_NAME_OBJ "Builtin_ObjShader" //must be same name within the post-build file

b8 vulkan_obj_shader_create(vulkan_context *context, vulkan_obj_shader *out_shader) {
    //shader module init per stage
    char stage_type_strs[OBJ_SHADER_STAGE_COUNT][5] = { "vert", "frag" };
    VkShaderStageFlagBits stage_types[OBJ_SHADER_STAGE_COUNT] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT
    };

    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        if (!create_shader_module(
                context, 
                BUILTIN_SHADER_NAME_OBJ, 
                stage_type_strs[i], 
                stage_types[i],
                i,
                out_shader->stages
        )) {
            KERROR("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJ);
            return false;
        }
    }

    //TODO: descriptors
    
    /* pipeline creation */
    VkViewport vp;
    vp.x = 0.0f;
    vp.y = (f32)context->framebuffer_height;
    vp.width = (f32)context->framebuffer_width;
    vp.height = -(f32)context->framebuffer_height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;

    //attributes
    u32 offset = 0;
    const i32 attr_count = 1;
    VkVertexInputAttributeDescription attr_descs[1]; //[attr_count]
    
    //position
    VkFormat formats[1] = { //[attr_count]
        VK_FORMAT_R32G32B32_SFLOAT
    };

    u64 sizes[1] = { //[attr_count]
        sizeof(vec3)
    };

    for (u32 i = 0; i < attr_count; i++) {
        attr_descs[i].binding = 0;  //binding ind - should match binding desc
        attr_descs[i].location = i; //attr loc
        attr_descs[i].format = formats[i];
        attr_descs[i].offset = offset;
        offset += sizes[i];
    }

    //TODO: desc set layouts

    //stages - should match the # of shader->stages
    VkPipelineShaderStageCreateInfo stage_create_infos[OBJ_SHADER_STAGE_COUNT];
    kzero_mem(stage_create_infos, sizeof(stage_create_infos));
    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        //stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }
    /* ----------------- */

    if (!vulkan_pipeline_create(
        context,
        &context->main_rp,
        attr_count,
        attr_descs,
        0,
        0,
        OBJ_SHADER_STAGE_COUNT,
        stage_create_infos,
        vp,
        scissor,
        false,
        &out_shader->pipeline
    )) {
        KERROR("Failed to load graphics pipeline for object shader.");
        return false;
    }

    return true;
}

void vulkan_obj_shader_destroy(vulkan_context *context, struct vulkan_obj_shader *shader) {
    vulkan_pipeline_destroy(context, &shader->pipeline);
    //destroy shader modules
    for (u32 i = 0; i < OBJ_SHADER_STAGE_COUNT; i++) {
        vkDestroyShaderModule(context->dev.logical_dev, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }
}

void vulkan_obj_shader_use(vulkan_context *context, struct vulkan_obj_shader *shader) {
    u32 img_ind = context->img_ind;
    vulkan_pipeline_bind(&context->graphics_cmd_bufs[img_ind], VK_PIPELINE_BIND_POINT_GRAPHICS, &shader->pipeline);
}