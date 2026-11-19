#include "vulkan_obj_shader.h"

#include "core/logger.h"

#include "renderer/vulkan/vulkan_shader_utils.h"

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

    //descriptors
    
    return true;
}

void vulkan_obj_shader_destroy(vulkan_context *context, struct vulkan_obj_shader *shader) {

}

void vulkan_obj_shader_use(vulkan_context *context, struct vulkan_obj_shader *shader) {

}