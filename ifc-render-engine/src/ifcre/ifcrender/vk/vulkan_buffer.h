#pragma once
#ifndef _IFCRE_VULKAN_BUFFER_H_
#define _IFCRE_VULKAN_BUFFER_H_

#include <vulkan/vulkan.h>
#include "vulkan_context.h"
#include "vulkan_util.h"
namespace ifcre {
    class VulkanBuffer {
    public:
        VulkanBuffer(VulkanContext* ctx, VkBufferUsageFlags flags, VkMemoryPropertyFlags properties) :
            m_vkContext(ctx), m_flags(flags), m_properties(properties) {}

        template<typename T>
        void create(T* data, uint32_t size) {
            m_size = size;
            if (size == 0)return;
            if (((m_flags & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) != 0)
                || ((m_flags & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) != 0)) {
                _createWithStaging((void*)data, sizeof(T) * size);
            }
            else if (m_flags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
                _createWithoutStaging(sizeof(T) * size);
            }
        }

        template<typename T>
        void update(T* src_data) {
            if (((m_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
                || ((m_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0))
            {
                auto ctx = *m_vkContext;
                void* data;
                uint32_t type_size = sizeof(T) * m_size;
                vkMapMemory(ctx.m_device, m_bufferMemory, 0, type_size, 0, &data);
                memcpy(data, src_data, type_size);
                vkUnmapMemory(ctx.m_device, m_bufferMemory);
            }
        }

        VkBuffer getBuffer() const { return m_buffer; }
        uint32_t getSize() const { return m_size; }

        void* cptr() {

        }

    private:
        void _createWithoutStaging(VkDeviceSize buffer_size);
        void _createWithStaging(void* data, VkDeviceSize buffer_size);
    private:
        VkBuffer m_buffer;
        VkDeviceMemory m_bufferMemory;
        VulkanContext* m_vkContext;
        VkBufferUsageFlags m_flags;
        VkMemoryPropertyFlags m_properties;
        uint32_t m_size;
    };

}

#endif