#include "vulkan_manager.h"
#include "vulkan_util.h"
#include "../uniform_obj.h"
#include "../../common/ifc_util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

#define MAKE_SHARED_UNIFORM_BUFFER(ctx_ptr) std::make_shared<VulkanBuffer>(ctx_ptr, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
#define MAKE_SHARED_INDEX_BUFFER(ctx_ptr) std::make_shared<VulkanBuffer>(ctx_ptr, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
#define MAKE_SHARED_VERTEX_BUFFER(ctx_ptr) std::make_shared<VulkanBuffer>(ctx_ptr, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

namespace ifcre
{
    void VulkanManager::initialize(GLFWwindow* window)
    {
        m_vkContext.initialize(window);

        createCommandPool();
        createCommandBuffers();
        createDescriptorPool();
        createSyncObjects();
        createUniformBuffers();

        setupPasses();
    }

    void VulkanManager::renderFrame(Scene& scene)
    {

        VK_CHECK_RESULT(m_vkContext.fp_vkWaitForFences(m_vkContext.m_device, 1, &m_inFlightFences[m_currentFrameIndex], VK_TRUE, UINT64_MAX));
        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(m_vkContext.m_device
            , m_vkContext.m_swapchain
            , UINT64_MAX
            , m_imageAvailableSemaphores[m_currentFrameIndex]
            , VK_NULL_HANDLE
            , &image_index);

        auto& cmd_info = VulkanPassBase::m_commandInfo;
        cmd_info.cmdBuffer = m_commandBuffers[m_currentFrameIndex];
        cmd_info.imageIndex = image_index;
        cmd_info.currentFrameIndex = m_currentFrameIndex;
        // std::cout<<"image_index: " << image_index << " currentFrameIndex: " << m_currentFrameIndex << "\n"; 

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // Swapchain has become incompatible with the surface.
            // Usually happens after a window resize.
            recreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // begin command buffer
        VkCommandBufferBeginInfo cmd_buffer_begin_info{};
        cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buffer_begin_info.flags = 0;
        cmd_buffer_begin_info.pInheritanceInfo = nullptr;
        VK_CHECK_RESULT(m_vkContext.fp_vkBeginCommandBuffer(m_commandBuffers[m_currentFrameIndex], &cmd_buffer_begin_info));

        // TODO Draw
        auto& camera = *scene.m_editCamera;
        auto& ifc_object = *scene.m_ifcObject;
        IFCRenderUBO ifc_render_ubo{};
        ifc_render_ubo.alpha = 0.5;
        ifc_render_ubo.cameraPos = camera.getViewPos();
        _updateTransformsUniform(ifc_object.getModelMatrix(), camera.getViewMatrix(), camera.getProjMatrix());
        _updateIFCRenderUniform(ifc_render_ubo);
        m_ifcBasePass.draw(ifc_object.render_id);

        // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

        // end command buffer
        VK_CHECK_RESULT(m_vkContext.fp_vkEndCommandBuffer(m_commandBuffers[m_currentFrameIndex]));

        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &m_imageAvailableSemaphores[m_currentFrameIndex];
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_commandBuffers[m_currentFrameIndex];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &m_renderFinishedSemaphores[m_currentFrameIndex];

        // reset fence
        m_vkContext.fp_vkResetFences(m_vkContext.m_device, 1, &m_inFlightFences[m_currentFrameIndex]);
        VK_CHECK_RESULT(vkQueueSubmit(m_vkContext.m_graphicsQueue
            , 1
            , &submit_info
            , m_inFlightFences[m_currentFrameIndex]));

        // present swapchain
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_currentFrameIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_vkContext.m_swapchain;
        presentInfo.pImageIndices = &image_index;
        // It's not necessary if you're only using a single swap chain
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(m_vkContext.m_presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            recreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            VK_CHECK_RESULT(result);
        }

        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_maxFramesInFlight;
    }

    uint32_t VulkanManager::addIFCMesh(IFCModelPayload& payload)
    {
        VulkanContext* ctx = &m_vkContext;
        VulkanIFCMeshBuffer mesh_buffer;
        auto& g_indices = *payload.g_indices;
        auto& opaque_indices = *payload.opaque_indices;
        auto& transparency_indices = *payload.transparency_indices;
        auto& edge_indices = *payload.edge_indices;
        auto& vertices = *payload.vertices;

        mesh_buffer.vertexBuffer = MAKE_SHARED_VERTEX_BUFFER(ctx);
        mesh_buffer.vertexBuffer->create<Real>(vertices.data(), vertices.size());

        mesh_buffer.gIndexBuffer = MAKE_SHARED_INDEX_BUFFER(ctx);
        mesh_buffer.gIndexBuffer->create<uint32_t>(g_indices.data(), g_indices.size());

        mesh_buffer.opaqueIndexBuffer = MAKE_SHARED_INDEX_BUFFER(ctx);
        mesh_buffer.opaqueIndexBuffer->create<uint32_t>(opaque_indices.data(), opaque_indices.size());

        mesh_buffer.transparencyIndexBuffer = MAKE_SHARED_INDEX_BUFFER(ctx);
        mesh_buffer.transparencyIndexBuffer->create<uint32_t>(transparency_indices.data(), transparency_indices.size());

        //std::cout << "opaque_indices size: " << opaque_indices.size() << "\n";
        //std::cout << "transparency_indices size: " << transparency_indices.size() << "\n";

        //std::cout << "g_indices size: " << g_indices.size() << "\n";

#ifndef _DEBUG
        mesh_buffer.edgeIndexBuffer = MAKE_SHARED_INDEX_BUFFER(ctx);
        mesh_buffer.edgeIndexBuffer->create<uint32_t>(edge_indices.data(), edge_indices.size());
#endif

        uint32_t id = util::get_next_globalid();
        m_vulkanResources.meshBufferMap.emplace(id, mesh_buffer);

        return id;
    }

    float VulkanManager::getDepthValue(Scene& scene, int32_t x, int32_t y)
    {
        float res = 0.0f;
        VkCommandBuffer command_buffer = m_vkContext.beginSingleTimeCommand();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { x, y, 0 };
        region.imageExtent = {1, 1, 1 };

        uint32_t buffer_size = 4;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        VulkanUtil::createBuffer(m_vkContext.m_physicalDevice,
            m_vkContext.m_device,
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory);

        VkImage depth_image = m_ifcBasePass.getDepthAttach();
        //VkImageMemoryBarrier copy_to_buffer_barrier{};
        //copy_to_buffer_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //copy_to_buffer_barrier.pNext = nullptr;
        //copy_to_buffer_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //copy_to_buffer_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        //copy_to_buffer_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //copy_to_buffer_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //copy_to_buffer_barrier.srcQueueFamilyIndex = m_vkContext.m_queueFamilyIndices.graphicsFamily.value();
        //copy_to_buffer_barrier.dstQueueFamilyIndex = m_vkContext.m_queueFamilyIndices.graphicsFamily.value();
        //copy_to_buffer_barrier.image = depth_image;
        //copy_to_buffer_barrier.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
        //vkCmdPipelineBarrier(command_buffer,
        //    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        //    VK_PIPELINE_STAGE_TRANSFER_BIT,
        //    0,
        //    0,
        //    nullptr,
        //    0,
        //    nullptr,
        //    1,
        //    &copy_to_buffer_barrier);
        vkCmdCopyImageToBuffer(command_buffer,
            depth_image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            staging_buffer,
            1,
            &region);
        m_vkContext.endSingleTimeCommand(command_buffer);

        float* data = nullptr;
        vkMapMemory(m_vkContext.m_device, staging_buffer_memory, 0, buffer_size, 0, (void**)&data);
        res = data[0];
        vkUnmapMemory(m_vkContext.m_device, staging_buffer_memory);

        vkDestroyBuffer(m_vkContext.m_device, staging_buffer, nullptr);
        vkFreeMemory(m_vkContext.m_device, staging_buffer_memory, nullptr);
        return res;
    }

    // ----------------------- initialize -------------------------

    void VulkanManager::createCommandPool() {
        VkCommandPoolCreateInfo pool_create_info{};
        pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_create_info.queueFamilyIndex = m_vkContext.m_queueFamilyIndices.graphicsFamily.value();
        pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK_RESULT(vkCreateCommandPool(m_vkContext.m_device, &pool_create_info, nullptr, &m_commandPool));
    }

    void VulkanManager::createCommandBuffers() {
        m_commandBuffers.resize(m_maxFramesInFlight);
        VkCommandBufferAllocateInfo cmd_buffer_allocate_info{};
        cmd_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_buffer_allocate_info.commandPool = m_commandPool;
        cmd_buffer_allocate_info.commandBufferCount = 1U;

        for (size_t i = 0; i < m_maxFramesInFlight; ++i) {
            VK_CHECK_RESULT(vkAllocateCommandBuffers(m_vkContext.m_device, &cmd_buffer_allocate_info, &m_commandBuffers[i]));
        }
    }

    void VulkanManager::createDescriptorPool() {
        auto& ctx = m_vkContext;
        std::array<VkDescriptorPoolSize, 1> descpool_size;
        descpool_size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descpool_size[0].descriptorCount = 1 * static_cast<uint32_t>(ctx.m_swapchainImages.size());

        VkDescriptorPoolCreateInfo descpool_ci{};
        descpool_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descpool_ci.poolSizeCount = static_cast<uint32_t>(descpool_size.size());
        descpool_ci.pPoolSizes = descpool_size.data();
        descpool_ci.maxSets = static_cast<uint32_t>(ctx.m_swapchainImages.size());

        VK_CHECK_RESULT(vkCreateDescriptorPool(ctx.m_device, &descpool_ci, nullptr, &m_descriptorPool));
    }

    void VulkanManager::createSyncObjects() {
        // semaphonre for GPU-GPU synchronized
        // fence for CPU-GPU synchronized
        m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
        m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
        m_inFlightFences.resize(m_maxFramesInFlight);
        // imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // default: UNSIGNALED
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < m_maxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(m_vkContext.m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_vkContext.m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_vkContext.m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanManager::createUniformBuffers()
    {
        VulkanContext* ctx = &m_vkContext;
        auto& uniform_buffer_map = m_vulkanResources.uniformBufferMap;
        uniform_buffer_map.resize(uniform_buffer_count);
        TransformsUBO transformsUBO{};
        IFCRenderUBO ifcRenderUBO{};

        uniform_buffer_map[uniform_buffer_transforms] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_transforms]->create<TransformsUBO>(&transformsUBO, 1);

        uniform_buffer_map[uniform_buffer_ifc_render] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_ifc_render]->create<IFCRenderUBO>(&ifcRenderUBO, 1);
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- 

    // ----------------------- swapchain --------------------------
    void VulkanManager::recreateSwapchain()
    {
        // ------------------- Handling minimization -----------------------
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_vkContext.m_window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(m_vkContext.m_window, &width, &height);
            glfwWaitEvents();
        }
        // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- 

            // vkDeviceWaitIdle(m_vkContext.m_device);
        VK_CHECK_RESULT(m_vkContext.fp_vkWaitForFences(m_vkContext.m_device
            , m_maxFramesInFlight
            , m_inFlightFences.data()
            , VK_TRUE
            , UINT64_MAX));

        m_vkContext.clearSwapchain();

        m_vkContext.createSwapchain();
        m_vkContext.createSwapchainImageViews();
        m_vkContext.createDepthResources();

        // todo recreate other framebuffer
        auto& cmd_info = VulkanPassBase::m_commandInfo;
        cmd_info.viewport.y = 0.0f;
        cmd_info.viewport.width = (float)width;
        cmd_info.viewport.height = (float)height;
        cmd_info.viewport.minDepth = 0.0f;
        cmd_info.viewport.maxDepth = 1.0f;
        cmd_info.scissor.offset = { 0, 0 };
        cmd_info.scissor.extent = m_vkContext.m_swapchainExtent;

        //m_testPass.recreateFramebuffers();
    }

    void VulkanManager::setupPasses()
    {
        auto& ctx = m_vkContext;
        auto& cmd_info = VulkanPassBase::m_commandInfo;
        cmd_info.commandPool = m_commandPool;

        cmd_info.viewport.x = 0.0f;
        cmd_info.viewport.y = 0.0f;
        cmd_info.viewport.width = (float)ctx.m_swapchainExtent.width;
        cmd_info.viewport.height = (float)ctx.m_swapchainExtent.height;
        cmd_info.viewport.minDepth = 0.0f;
        cmd_info.viewport.maxDepth = 1.0f;
        cmd_info.scissor.offset = { 0, 0 };
        cmd_info.scissor.extent = ctx.m_swapchainExtent;
        
        RenderPassHelperInfo renderPassInfo;
        renderPassInfo.context = &m_vkContext;
        renderPassInfo.resources = &m_vulkanResources;
        renderPassInfo.descriptorPool = m_descriptorPool;
        VulkanPassBase::setRenderPassInfo(renderPassInfo);

        m_ifcBasePass.initialize();
    }

    void VulkanManager::_updateTransformsUniform(const glm::mat4& model_matrix, const glm::mat4& view, const glm::mat4& projection)
    {
        TransformsUBO ubo;
        ubo.model = model_matrix;
        ubo.proj_view_model = projection * view * model_matrix;
        ubo.transpose_inv_model = glm::transpose(glm::inverse(glm::mat3(model_matrix)));

        auto& trans_uniform_buffer = *(m_vulkanResources.uniformBufferMap[uniform_buffer_transforms]);
        trans_uniform_buffer.update<TransformsUBO>(&ubo);
    }

    void VulkanManager::_updateIFCRenderUniform(IFCRenderUBO& ubo)
    {
        auto& ifc_render_uniform_buffer = *(m_vulkanResources.uniformBufferMap[uniform_buffer_ifc_render]);
        ifc_render_uniform_buffer.update<IFCRenderUBO>(&ubo);
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}