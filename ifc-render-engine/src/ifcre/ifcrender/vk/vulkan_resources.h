#pragma once
#ifndef _IFCRE_VULKAN_RESROUCES_H_
#define _IFCRE_VULKAN_RESROUCES_H_

#include <memory>
#include <vector>
#include <map>
#include "vulkan_buffer.h"
#include "../uniform_obj.h"
namespace ifcre {

    struct VulkanIFCMeshBuffer {
        std::shared_ptr<VulkanBuffer> gIndexBuffer;
        std::shared_ptr<VulkanBuffer> opaqueIndexBuffer;
        std::shared_ptr<VulkanBuffer> transparencyIndexBuffer;
        std::shared_ptr<VulkanBuffer> edgeIndexBuffer;

        std::shared_ptr<VulkanBuffer> vertexBuffer;
    };

    struct VulkanMeshBuffer {
        std::shared_ptr<VulkanBuffer> IndexBuffer;
        std::shared_ptr<VulkanBuffer> vertexBuffer;
    };


    struct VulkanResourceHandle {
        std::map<uint32_t, VulkanIFCMeshBuffer> meshBufferMap;
        std::vector<std::shared_ptr<VulkanBuffer>> uniformBufferMap;

        std::shared_ptr<VulkanBuffer> axisBuffer;
        VulkanMeshBuffer boundingBoxMeshBuffer;
        int32_t curBoundingBoxId = -1;
        

        template<class T>
        void update(T ubo, UniformBufferEnum e) {
            auto& trans_uniform_buffer = *(uniformBufferMap[e]);
            trans_uniform_buffer.update<T>(&ubo);
        }
    };
}

#endif