#pragma once
#ifndef _IFCRE_VULKAN_MANAGER_H_
#define _IFCRE_VULKAN_MANAGER_H_

#include "vulkan_context.h"
#include "vulkan_buffer.h"
#include "vulkan_resources.h"
#include "vulkan_pass_base.h"
#include "pass/ifc_base_pass.h"
#include "pass/ifc_pick_pass.h"
#include "../scene.h"
#include "../uniform_obj.h"
#include <vector>
#include <map>
#include <memory>
namespace ifcre {
    extern VulkanIFCBasePass;

    struct IFCModelPayload {
        std::vector<uint32_t>* g_indices;
        std::vector<uint32_t>* opaque_indices;
        std::vector<uint32_t>* transparency_indices;
        std::vector<uint32_t>* edge_indices;

        std::vector<float>* vertices;
    };

    class VulkanManager {
    public:
        void initialize(GLFWwindow* window);
        void renderFrame(Scene& scene);

        uint32_t addIFCMesh(IFCModelPayload& payload);
        float getDepthValue(Scene& scene, int32_t x, int32_t y);
        int32_t getCompIdValue(Scene& scene, int32_t x, int32_t y);
        glm::ivec2 getCompIdAndDepthValue(Scene& scene, int32_t x, int32_t y);
    private:

        // vulkan device context
        VulkanContext m_vkContext;
        uint32_t const m_maxFramesInFlight = 3;
        uint32_t m_currentFrameIndex = 0;

        // sync object
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

        // command
        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;

        // Pass
        VulkanIFCBasePass m_ifcBasePass;
        VulkanIFCPickPass m_ifcPickPass;

        // descriptor pool
        VkDescriptorPool m_descriptorPool;

        VulkanResourceHandle m_vulkanResources;
    private:
        void _createCommandPool();
        void _createCommandBuffers();
        void _createDescriptorPool();
        void _createSyncObjects();
        void _createUniformBuffers();
        void _recreateSwapchain();
        
        void _setupPasses();
        void _createInnerMesh();
        void _updateUniform(Scene& scene);
    };
}

#endif