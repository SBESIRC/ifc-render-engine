#ifndef _IFCRE_VULKAN_CONTEXT_H_
#define _IFCRE_VULKAN_CONTEXT_H_
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN 1
#endif

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace ifcre
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanContext
    {
    public:
        GLFWwindow* m_window = nullptr;
        VkInstance m_instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices m_queueFamilyIndices;
        VkDevice m_device = VK_NULL_HANDLE;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;
        VkCommandPool m_commandPool;

        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
        VkFormat m_depthImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_swapchainExtent;
        std::vector<VkImage> m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        VkImage m_depthImage = VK_NULL_HANDLE;
        VkImageView m_depthImageView = VK_NULL_HANDLE;
        VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;

        VkSampleCountFlags m_maxMSAASample;

        PFN_vkWaitForFences         fp_vkWaitForFences;
        PFN_vkResetFences           fp_vkResetFences;
        PFN_vkResetCommandPool      fp_vkResetCommandPool;
        PFN_vkBeginCommandBuffer    fp_vkBeginCommandBuffer;
        PFN_vkEndCommandBuffer      fp_vkEndCommandBuffer;
        PFN_vkCmdBeginRenderPass    fp_vkCmdBeginRenderPass;
        PFN_vkCmdNextSubpass        fp_vkCmdNextSubpass;
        PFN_vkCmdEndRenderPass      fp_vkCmdEndRenderPass;
        PFN_vkCmdBindPipeline       fp_vkCmdBindPipeline;
        PFN_vkCmdSetViewport        fp_vkCmdSetViewport;
        PFN_vkCmdSetScissor         fp_vkCmdSetScissor;
        PFN_vkCmdBindVertexBuffers  fp_vkCmdBindVertexBuffers;
        PFN_vkCmdBindIndexBuffer    fp_vkCmdBindIndexBuffer;
        PFN_vkCmdBindDescriptorSets fp_vkCmdBindDescriptorSets;
        PFN_vkCmdDrawIndexed        fp_vkCmdDrawIndexed;
        PFN_vkCmdDraw   fp_vkCmdDraw;
        PFN_vkCmdClearAttachments   fp_vkCmdClearAttachments;

    public:
        void initialize(GLFWwindow* window);
        void clear();

        void createSwapchain();
        void createSwapchainImageViews();
        void createDepthResources();
        void clearSwapchain();

        VkCommandBuffer beginSingleTimeCommand();
        void endSingleTimeCommand(VkCommandBuffer cmd_buffer);

    private:
        void createInstance();
        void setupDebugMessenger();
        void createWindowSurface();
        void setupPhysicalDevice();
        void createLogicalDevice();

        void createCommandPool();

    private:
 #ifdef _DEBUG
        bool m_enableValidationLayer = true;
#else
        bool m_enableValidationLayer = false;
#endif
        const std::vector<char const*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        uint32_t m_vulkanAPIVersion;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        const VkQueueFlagBits m_queueFlag = VK_QUEUE_GRAPHICS_BIT;

        // helper
    private:
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkResult createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void destroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkQueueFlagBits flags);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapchainSurfaceFormatFromDetails(std::vector<VkSurfaceFormatKHR>& available_surface_formats);
        VkPresentModeKHR chooseSwapchainPresentModeFromDetails(std::vector<VkPresentModeKHR>& available_present_modes);
        VkExtent2D chooseSwapchainExtentFromDetails(VkSurfaceCapabilitiesKHR capabilities);
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    };

}


#endif