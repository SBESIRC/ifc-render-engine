#ifndef GALAXYSAILING_VULKAN_UTIL_H_
#define GALAXYSAILING_VULKAN_UTIL_H_

#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>
// std::cout << "Fatal : VkResult is \"" << vks::tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n";
#define VK_CHECK_RESULT(f)                                                                                           \
    {                                                                                                                \
        VkResult res = (f);                                                                                          \
        if (res != VK_SUCCESS)                                                                                       \
        {                                                                                                            \
            std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
            assert(res == VK_SUCCESS);                                                                               \
        }                                                                                                            \
    }
namespace galaxysailing
{
    class VulkanUtil
    {
    public:
        static uint32_t findMemoryType(VkPhysicalDevice      physical_device,
            uint32_t              type_filter,
            VkMemoryPropertyFlags properties_flag);

        static void createImage(VkPhysicalDevice physical_device,
            VkDevice device,
            uint32_t image_width,
            uint32_t image_height,
            VkFormat format,
            VkImageTiling image_tiling,
            VkImageUsageFlags image_usage_flags,
            VkMemoryPropertyFlags memory_property_flags,
            VkImage& image,
            VkDeviceMemory& memory,
            VkImageCreateFlags image_create_flags,
            uint32_t array_layers,
            uint32_t miplevels);
        static VkImageView createImageView(VkDevice device,
            VkImage& image,
            VkFormat format,
            VkImageAspectFlags image_aspect_flags,
            VkImageViewType view_type,
            uint32_t layout_count,
            uint32_t miplevels);
    };
}

#endif