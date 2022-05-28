#pragma once
#ifndef _IFCRE_VULKAN_PASS_H_
#define _IFCRE_VULKAN_PASS_H_
 
#include "vulkan_context.h"
#include "vulkan_buffer.h"
#include "vulkan_resources.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "../shader_consts.h"

namespace ifcre {
    extern VkDescriptorPool;

    struct RenderPassHelperInfo {
        VulkanContext* context;
        VulkanResourceHandle* resources;
        VkDescriptorPool descriptorPool;
    };

	struct RenderCommandInfo
	{
		VkCommandPool commandPool;
		VkViewport viewport;
		VkRect2D scissor;
        uint32_t* p_currentFrameIndex;
        VkCommandBuffer curCmdBuffer;
        int32_t imageIndex;
        VkFence curInFlightFence;
        int32_t maxFramesInFlight;
        VkFence* inFlightFences;
	};


    class VulkanPassBase {
    public:
        struct FrameBufferAttachment
        {
            VkImage        image;
            VkDeviceMemory mem;
            VkImageView    view;
            VkFormat       format;
        };

        struct Framebuffer
        {
            int32_t width;
            int32_t height;
            VkRenderPass  render_pass;

            std::vector<FrameBufferAttachment> attachments;
        };

        struct Descriptor
        {
            VkDescriptorSetLayout layout;
            VkDescriptorSet       descriptor_set;
        };

        struct RenderPipeline
        {
            VkPipelineLayout layout;
            VkPipeline       pipeline;
        };


        virtual void initialize() {
            assert(m_vkContext != nullptr);
            assert(m_descriptorPool != VK_NULL_HANDLE);
            assert(m_vulkanResources != nullptr);
        }

        static void setRenderPassInfo(const RenderPassHelperInfo& info) {
            m_vkContext = info.context;
            m_vulkanResources = info.resources;
            m_descriptorPool = info.descriptorPool;
        }

        static RenderCommandInfo m_commandInfo;

    protected:
        std::vector<Descriptor> m_descriptorInfos;
        std::vector<RenderPipeline> m_renderPipeline;
        Framebuffer m_framebuffer;

        static VkDescriptorPool m_descriptorPool;
        static VulkanContext* m_vkContext;
        static VulkanResourceHandle* m_vulkanResources;
        
    };

}

#endif