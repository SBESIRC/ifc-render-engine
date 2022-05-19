#pragma once
#ifndef _IFCRE_VULKAN_MESH_H_
#define _IFCRE_VULKAN_MESH_H_

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>
namespace ifcre {
    struct IFCVulkanVertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 kd;
        int32_t compId;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription binding_desc{};
            binding_desc.binding = 0;
            binding_desc.stride = sizeof(IFCVulkanVertex);
            binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return binding_desc;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 4> attribute_descs{};

            attribute_descs[0].binding = 0;
            attribute_descs[0].location = 0;
            attribute_descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descs[0].offset = offsetof(IFCVulkanVertex, pos);

            attribute_descs[1].binding = 0;
            attribute_descs[1].location = 1;
            attribute_descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descs[1].offset = offsetof(IFCVulkanVertex, normal);

            attribute_descs[2].binding = 0;
            attribute_descs[2].location = 2;
            attribute_descs[2].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descs[2].offset = offsetof(IFCVulkanVertex, kd);

            attribute_descs[3].binding = 0;
            attribute_descs[3].location = 3;
            attribute_descs[3].format = VK_FORMAT_R32_SINT;
            attribute_descs[3].offset = offsetof(IFCVulkanVertex, compId);

            return attribute_descs;
        }
    };

}

#endif