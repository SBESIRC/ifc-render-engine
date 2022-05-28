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

        _createCommandPool();
        _createCommandBuffers();
        _createDescriptorPool();
        _createSyncObjects();
        _createUniformBuffers();

        _setupPasses();
        _createInnerMesh();
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
        cmd_info.curCmdBuffer = m_commandBuffers[m_currentFrameIndex];
        cmd_info.imageIndex = image_index;
        cmd_info.p_currentFrameIndex = &m_currentFrameIndex;
        cmd_info.curInFlightFence = m_inFlightFences[m_currentFrameIndex];
        // std::cout<<"image_index: " << image_index << " currentFrameIndex: " << m_currentFrameIndex << "\n"; 

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // Swapchain has become incompatible with the surface.
            // Usually happens after a window resize.
            _recreateSwapchain();
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

        _updateUniform(scene);
        // --------------------- Draw Scene ----------------------
        auto& ifc_object = *scene.m_ifcObject;
        m_ifcBasePass.draw(ifc_object.render_id);
        //m_ifcPickPass.draw(ifc_object.render_id); TODO

        // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

        // end command buffer
        VK_CHECK_RESULT(m_vkContext.fp_vkEndCommandBuffer(m_commandBuffers[m_currentFrameIndex]));

        // reset fence
        m_vkContext.fp_vkResetFences(m_vkContext.m_device, 1, &m_inFlightFences[m_currentFrameIndex]);

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
            _recreateSwapchain();
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
        //mesh_buffer.edgeIndexBuffer = MAKE_SHARED_INDEX_BUFFER(ctx);
        //mesh_buffer.edgeIndexBuffer->create<uint32_t>(edge_indices.data(), edge_indices.size());
#endif

        uint32_t id = util::get_next_globalid();
        m_vulkanResources.meshBufferMap.emplace(id, mesh_buffer);

        return id;
    }

    float VulkanManager::getDepthValue(Scene& scene, int32_t x, int32_t y)
    {
        auto& ifc_obj = *scene.m_ifcObject;
        glm::ivec2 res = m_ifcPickPass.pick(ifc_obj.render_id, x, y, VulkanIFCPickPass::PickTypeEnum::pick_depth);
        return util::int_as_float(res.x);
    }

    int32_t VulkanManager::getCompIdValue(Scene& scene, int32_t x, int32_t y)
    {
        auto& ifc_obj = *scene.m_ifcObject;
        glm::ivec2 res = m_ifcPickPass.pick(ifc_obj.render_id, x, y, VulkanIFCPickPass::PickTypeEnum::pick_comp_id);
        return res.y;
    }

    glm::ivec2 VulkanManager::getCompIdAndDepthValue(Scene& scene, int32_t x, int32_t y)
    {
        auto& ifc_obj = *scene.m_ifcObject;
        uint32_t flag = VulkanIFCPickPass::PickTypeEnum::pick_comp_id | VulkanIFCPickPass::PickTypeEnum::pick_depth;
        glm::ivec2 res = m_ifcPickPass.pick(ifc_obj.render_id, x, y, (VulkanIFCPickPass::PickTypeEnum)flag);
        return res;
    }

    

    // ----------------------- initialize -------------------------

    void VulkanManager::_createCommandPool() {
        VkCommandPoolCreateInfo pool_create_info{};
        pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_create_info.queueFamilyIndex = m_vkContext.m_queueFamilyIndices.graphicsFamily.value();
        pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK_RESULT(vkCreateCommandPool(m_vkContext.m_device, &pool_create_info, nullptr, &m_commandPool));
    }

    void VulkanManager::_createCommandBuffers() {
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

    void VulkanManager::_createDescriptorPool() {
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

    void VulkanManager::_createSyncObjects() {
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

    void VulkanManager::_createUniformBuffers()
    {
        VulkanContext* ctx = &m_vkContext;
        auto& uniform_buffer_map = m_vulkanResources.uniformBufferMap;
        uniform_buffer_map.resize(uniform_buffer_count);
        TransformsUBO transforms_ubo{};
        IFCRenderUBO ifc_render_ubo{};
        TransformMVPUBO transform_mvp_ubo{};

        uniform_buffer_map[uniform_buffer_transforms] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_transforms]->create<TransformsUBO>(&transforms_ubo, 1);

        uniform_buffer_map[uniform_buffer_ifc_render] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_ifc_render]->create<IFCRenderUBO>(&ifc_render_ubo, 1);

        uniform_buffer_map[uniform_buffer_transform_mvp] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_transform_mvp]->create<TransformMVPUBO>(&transform_mvp_ubo, 1);

        uniform_buffer_map[uniform_buffer_transform_mvp_axis] = MAKE_SHARED_UNIFORM_BUFFER(ctx);
        uniform_buffer_map[uniform_buffer_transform_mvp_axis]->create<TransformMVPUBO>(&transform_mvp_ubo, 1);
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- 

    // ----------------------- swapchain --------------------------
    void VulkanManager::_recreateSwapchain()
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


        m_ifcBasePass.recreateFramebuffers();
        m_ifcPickPass.recreateFramebuffers();
    }

    void VulkanManager::_setupPasses()
    {
        auto& ctx = m_vkContext;
        auto& cmd_info = VulkanPassBase::m_commandInfo;
        cmd_info.commandPool = m_commandPool;
        cmd_info.maxFramesInFlight = m_maxFramesInFlight;
        cmd_info.inFlightFences = m_inFlightFences.data();

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
        m_ifcPickPass.initialize();
    }

    void VulkanManager::_createInnerMesh()
    {
        float coord_axis[] = {
                0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,	// x-axis
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.0,	// y-axis
                0.0, 0.0, 0.0,
                0.0, 0.0, 1.0	// z-axis
        };
        VulkanContext* ctx = &m_vkContext;
        m_vulkanResources.axisBuffer = MAKE_SHARED_VERTEX_BUFFER(ctx);
        m_vulkanResources.axisBuffer->create<float>(coord_axis, 18);
    }

    void VulkanManager::_updateUniform(Scene& scene)
    {
        auto& camera = *scene.m_editCamera;
        auto& ifc_object = *scene.m_ifcObject;

        auto proj_view_matrix = camera.getProjMatrix() * camera.getViewMatrix();
        IFCRenderUBO ifc_render_ubo{};
        ifc_render_ubo.alpha = 0.5;
        ifc_render_ubo.cameraPos = camera.getViewPos();
        ifc_render_ubo.c_comp = scene.m_compId.clicked;
        ifc_render_ubo.h_comp = scene.m_compId.hovered;

        TransformsUBO transforms_ubo{};
        transforms_ubo.model = ifc_object.getModelMatrix();
        transforms_ubo.transpose_inv_model = glm::transpose(glm::inverse(glm::mat3(transforms_ubo.model)));
        transforms_ubo.proj_view_model = proj_view_matrix * transforms_ubo.model;

        TransformMVPUBO transform_mvp_ubo{};
        transform_mvp_ubo.proj_view_model = transforms_ubo.proj_view_model;

        // cal axis model matrix
        TransformMVPUBO transform_mvp_axis_ubo{};
        {
            auto& ifc_model = *scene.m_ifcObject;
            glm::vec3 model_center = ifc_model.getModelCenter();
            glm::mat4 model = ifc_model.getModelMatrix();
            float scale_factor = ifc_model.getScaleFactor();

            glm::mat4 trans_center(1.0f);
            glm::mat4 trans_click_center(1.0f);
            trans_center = glm::translate(trans_center, model_center);
            model = model * trans_center;
            glm::vec3 world_pos(model[3][0], model[3][1], model[3][2]);

            //float len_ref = glm::length(init_view_pos);
            float len_ref = 10.0f;
            float len = glm::length(scene.m_editCamera->getViewPos() - scene.m_pickWorldPos);
            //printf("%f\n", scale_factor);
            float scale = len / len_ref / scale_factor * 0.25f;
            model = glm::scale(model, glm::vec3(scale, scale, scale));

            trans_click_center = glm::translate(trans_click_center, scene.m_pickWorldPos - world_pos);
            model = trans_click_center * model;
            transform_mvp_axis_ubo.proj_view_model = proj_view_matrix * model;
        }
        // ----- ----- ----- ----- ----- ----- ----- ----- 


        m_vulkanResources.update<TransformsUBO>(transforms_ubo, uniform_buffer_transforms);
        m_vulkanResources.update<IFCRenderUBO>(ifc_render_ubo, uniform_buffer_ifc_render);
        m_vulkanResources.update<TransformMVPUBO>(transform_mvp_ubo, uniform_buffer_transform_mvp);

        m_vulkanResources.update<TransformMVPUBO>(transform_mvp_axis_ubo, uniform_buffer_transform_mvp_axis);
    }

    // ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

}