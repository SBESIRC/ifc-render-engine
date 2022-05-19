#include "vulkan_pass_base.h"
#include "vulkan_util.h"
#include "vulkan_mesh.h"
#include <array>
ifcre::VulkanContext* ifcre::VulkanPassBase::m_vkContext = nullptr;
VkDescriptorPool ifcre::VulkanPassBase::m_descriptorPool = VK_NULL_HANDLE;
ifcre::VulkanResourceHandle* ifcre::VulkanPassBase::m_vulkanResources = nullptr;

ifcre::RenderCommandInfo ifcre::VulkanPassBase::m_commandInfo;

namespace ifcre {

}