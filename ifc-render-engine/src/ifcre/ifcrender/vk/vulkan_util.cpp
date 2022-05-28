#include "vulkan_util.h"
#include "vulkan_context.h"
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
namespace ifcre
{
    uint32_t VulkanUtil::findMemoryType(VkPhysicalDevice physical_device,
        uint32_t type_filter,
        VkMemoryPropertyFlags properties_flag)
    {
        VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
        for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
        {
            if (type_filter & (1 << i) &&
                (physical_device_memory_properties.memoryTypes[i].propertyFlags & properties_flag) == properties_flag)
            {
                return i;
            }
        }
        throw std::runtime_error("findMemoryType");
    }
    void VulkanUtil::createBuffer(VkPhysicalDevice physical_device,
        VkDevice device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& buffer_memory)
    {
        VkBufferCreateInfo buffer_create_info{};
        buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_create_info.size = size;
        buffer_create_info.usage = usage;                           // use as a vertex/staging/index buffer
        buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // not sharing among queue families

        VK_CHECK_RESULT(vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer));

        VkMemoryRequirements buffer_memory_requirements; // for allocate_info.allocationSize and
                                                         // allocate_info.memoryTypeIndex
        vkGetBufferMemoryRequirements(device, buffer, &buffer_memory_requirements);

        VkMemoryAllocateInfo buffer_memory_allocate_info{};
        buffer_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        buffer_memory_allocate_info.allocationSize = buffer_memory_requirements.size;
        buffer_memory_allocate_info.memoryTypeIndex =
            VulkanUtil::findMemoryType(physical_device, buffer_memory_requirements.memoryTypeBits, properties);

        VK_CHECK_RESULT(vkAllocateMemory(device, &buffer_memory_allocate_info, nullptr, &buffer_memory));

        // bind buffer with buffer memory
        vkBindBufferMemory(device, buffer, buffer_memory, 0); // offset = 0
    }
    void VulkanUtil::copyBuffer(class VulkanContext* context,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize srcOffset,
        VkDeviceSize dstOffset,
        VkDeviceSize size)
    {
        assert(context);
        VkCommandBuffer command_buffer = context->beginSingleTimeCommand();

        VkBufferCopy copyRegion = { srcOffset, dstOffset, size };
        vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);

        context->endSingleTimeCommand(command_buffer);
    }
    void VulkanUtil::createImage(VkPhysicalDevice physical_device,
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
        uint32_t miplevels,
        VkSampleCountFlagBits samples)
    {
        VkImageCreateInfo image_create_info{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.flags = image_create_flags;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = image_width;
        image_create_info.extent.height = image_height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = miplevels;
        image_create_info.arrayLayers = array_layers;
        image_create_info.format = format;
        image_create_info.tiling = image_tiling;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = image_usage_flags;
        image_create_info.samples = samples;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &image_create_info, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, memory_property_flags);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, memory, 0);
    }

    VkImageView VulkanUtil::createImageView(VkDevice device,
        VkImage& image,
        VkFormat format,
        VkImageAspectFlags image_aspect_flags,
        VkImageViewType view_type,
        uint32_t layout_count,
        uint32_t miplevels)
    {
        VkImageViewCreateInfo image_view_create_info{};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = image;
        image_view_create_info.viewType = view_type;
        image_view_create_info.format = format;
        image_view_create_info.subresourceRange.aspectMask = image_aspect_flags;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = miplevels;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = layout_count;

        VkImageView image_view;
        VK_CHECK_RESULT(vkCreateImageView(device, &image_view_create_info, nullptr, &image_view));
        return image_view;
    }

    std::vector<uint32_t> VulkanUtil::compileFile(const std::string& filename,
        shaderc_shader_kind kind,
        bool optimize)
    {
        static auto read_file = [](const std::string& fn) -> std::string {
            std::ifstream fin(fn, std::ios::in | std::ios::binary);

            if (!fin.is_open()) {
                throw std::runtime_error("failed to open file!");
            }
            std::stringstream ss;
            ss << fin.rdbuf();

            return ss.str();
        };

        std::string source = read_file(filename);

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        // options.AddMacroDefinition("MY_DEFINE", "1");
        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(source, kind, filename.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            // std::cerr << module.GetErrorMessage();
            return std::vector<uint32_t>();
        }

        return { module.cbegin(), module.cend() };
    }

    std::vector<uint32_t> VulkanUtil::compileString(const std::string& src, shaderc_shader_kind kind, bool optimize)
    {

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        // Like -DMY_DEFINE=1
        // options.AddMacroDefinition("MY_DEFINE", "1");
        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        //std::cout << src << "\n";
        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(src, kind, "shader_consts.h", options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
             std::cerr << module.GetErrorMessage();
            return std::vector<uint32_t>();
        }

        return { module.cbegin(), module.cend() };
    }

    VkShaderModule VulkanUtil::createShaderModule(VkDevice device, const std::vector<uint32_t>& code) {
        VkShaderModuleCreateInfo shader_module_ci{};
        shader_module_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_ci.codeSize = code.size() * 4;
        shader_module_ci.pCode = code.data();

        VkShaderModule shaderModule;
        VK_CHECK_RESULT(vkCreateShaderModule(device, &shader_module_ci, nullptr, &shaderModule));

        return shaderModule;
    }
}