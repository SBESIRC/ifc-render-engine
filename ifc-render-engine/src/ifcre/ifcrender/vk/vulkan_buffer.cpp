#include "vulkan_buffer.h"

namespace ifcre {
    void VulkanBuffer::_createWithStaging(void* src_data, VkDeviceSize buffer_size)
    {
        auto& ctx = *m_vkContext;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        VulkanUtil::createBuffer(ctx.m_physicalDevice
            , ctx.m_device
            , buffer_size
            , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            , staging_buffer, staging_buffer_memory);

        void* data = nullptr;
        vkMapMemory(ctx.m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, src_data, (size_t)buffer_size);
        vkUnmapMemory(ctx.m_device, staging_buffer_memory);

        VulkanUtil::createBuffer(ctx.m_physicalDevice
            , ctx.m_device
            , buffer_size
            , m_flags
            , m_properties
            , m_buffer
            , m_bufferMemory);

        VulkanUtil::copyBuffer(m_vkContext, staging_buffer, m_buffer, 0, 0, buffer_size);

        vkDestroyBuffer(ctx.m_device, staging_buffer, nullptr);
        vkFreeMemory(ctx.m_device, staging_buffer_memory, nullptr);
    }

    void VulkanBuffer::_createWithoutStaging(VkDeviceSize buffer_size) {
        auto& ctx = *m_vkContext;
        VulkanUtil::createBuffer(ctx.m_physicalDevice
            , ctx.m_device
            , buffer_size
            , m_flags
            , m_properties
            , m_buffer
            , m_bufferMemory);
    }
}