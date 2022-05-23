#include "ifc_base_pass.h"
#include "../vulkan_util.h"
#include "../vulkan_mesh.h"
#include "../../uniform_obj.h"
#include <array>
namespace ifcre {

    void VulkanIFCBasePass::draw(uint32_t mesh_id)
    {
        auto mesh_it = m_vulkanResources->meshBufferMap.find(mesh_id);
        if (mesh_it == m_vulkanResources->meshBufferMap.end()) {
            return;
        }
        auto& mesh_buffer = mesh_it->second;
        VulkanBuffer& vertex_buffer = *(mesh_buffer.vertexBuffer);
        VulkanBuffer& opaque_index_buffer = *(mesh_buffer.opaqueIndexBuffer);
        VulkanBuffer& transparency_index_bufer = *(mesh_buffer.transparencyIndexBuffer);
        VulkanBuffer& g_index_buffer = *(mesh_buffer.gIndexBuffer);

        auto& ctx = *m_vkContext;
        auto current_frame_index = m_commandInfo.currentFrameIndex;
        auto image_index = m_commandInfo.imageIndex;
        auto& cmd_buffer = m_commandInfo.cmdBuffer;
		{
			VkRenderPassBeginInfo renderpass_begin_info{};
			renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpass_begin_info.renderPass = m_framebuffer.render_pass;
			renderpass_begin_info.framebuffer = m_swapchainFramebuffers[image_index];
			renderpass_begin_info.renderArea.offset = { 0, 0 };
			renderpass_begin_info.renderArea.extent = ctx.m_swapchainExtent;

			std::array<VkClearValue, 2> clear_values;
			clear_values[0].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
			clear_values[1].depthStencil = { 1.0f, 0 };
			renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			renderpass_begin_info.pClearValues = clear_values.data();
			ctx.fp_vkCmdBeginRenderPass(cmd_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}
		ctx.fp_vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[render_pipeline_opaque].pipeline);
		ctx.fp_vkCmdSetViewport(cmd_buffer, 0, 1, &m_commandInfo.viewport);
		ctx.fp_vkCmdSetScissor(cmd_buffer, 0, 1, &m_commandInfo.scissor);

		VkBuffer vertex_buffers[] = { vertex_buffer.getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		ctx.fp_vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);
		ctx.fp_vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[render_pipeline_opaque].layout, 0, 1, &m_descriptorInfos[layout_base].descriptor_set, 0, nullptr);
		ctx.fp_vkCmdBindIndexBuffer(cmd_buffer, opaque_index_buffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		ctx.fp_vkCmdDrawIndexed(cmd_buffer, opaque_index_buffer.getSize(), 1, 0, 0, 0);

        ctx.fp_vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[render_pipeline_transparency].pipeline);
        ctx.fp_vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[render_pipeline_transparency].layout, 0, 1, &m_descriptorInfos[layout_base].descriptor_set, 0, nullptr);
		ctx.fp_vkCmdBindIndexBuffer(cmd_buffer, transparency_index_bufer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		ctx.fp_vkCmdDrawIndexed(cmd_buffer, transparency_index_bufer.getSize(), 1, 0, 0, 0);
		ctx.fp_vkCmdEndRenderPass(cmd_buffer);

    }

    void VulkanIFCBasePass::initialize()
    {
        //VulkanPassBase::initialize();
        assert(m_vkContext != nullptr);
        assert(m_descriptorPool != VK_NULL_HANDLE);
        assert(m_vulkanResources != nullptr);

        _createDescriptorSetLayouts();
        _createDescriptorSets();
        _createAttachments();
        _createRenderPass();
        _createPipeline();
        _createFramebuffers();
    }

    void VulkanIFCBasePass::recreateFramebuffers()
    {
        auto& ctx = *m_vkContext;

        for (int i = 0; i < m_framebuffer.attachments.size(); ++i) {
            vkDestroyImageView(ctx.m_device, m_framebuffer.attachments[i].view, nullptr);
            vkDestroyImage(ctx.m_device, m_framebuffer.attachments[i].image, nullptr);
            vkFreeMemory(ctx.m_device, m_framebuffer.attachments[i].mem, nullptr);
        }

        for (int i = 0; i < m_swapchainFramebuffers.size(); ++i) {
            vkDestroyFramebuffer(ctx.m_device, m_swapchainFramebuffers[i], nullptr);
        }
        _createAttachments();
        _createFramebuffers();
    }

    void VulkanIFCBasePass::_createAttachments()
    {
        auto& ctx = *m_vkContext;
        m_framebuffer.attachments.resize(2);
        m_framebuffer.attachments[0].format = ctx.m_swapchainImageFormat;
        m_framebuffer.attachments[1].format = ctx.m_depthImageFormat;

        m_framebuffer.width = ctx.m_swapchainExtent.width;
        m_framebuffer.height = ctx.m_swapchainExtent.height;

        // msaa color image
        {
            VulkanUtil::createImage(ctx.m_physicalDevice,
                ctx.m_device,
                m_framebuffer.width,
                m_framebuffer.height,
                m_framebuffer.attachments[0].format,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_framebuffer.attachments[0].image,
                m_framebuffer.attachments[0].mem,
                0,
                1,
                1,
                VK_SAMPLE_COUNT_4_BIT);
            m_framebuffer.attachments[0].view = VulkanUtil::createImageView(ctx.m_device
                , m_framebuffer.attachments[0].image
                , m_framebuffer.attachments[0].format
                , VK_IMAGE_ASPECT_COLOR_BIT
                , VK_IMAGE_VIEW_TYPE_2D
                , 1
                , 1);
        }

        // msaa depth image
        {
            VulkanUtil::createImage(ctx.m_physicalDevice,
                ctx.m_device,
                m_framebuffer.width,
                m_framebuffer.height,
                m_framebuffer.attachments[1].format,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_framebuffer.attachments[1].image,
                m_framebuffer.attachments[1].mem,
                0,
                1,
                1,
                VK_SAMPLE_COUNT_4_BIT);

            m_framebuffer.attachments[1].view = VulkanUtil::createImageView(ctx.m_device
                , m_framebuffer.attachments[1].image
                , m_framebuffer.attachments[1].format
                , VK_IMAGE_ASPECT_DEPTH_BIT
                , VK_IMAGE_VIEW_TYPE_2D
                , 1
                , 1);
        }
    }

    void VulkanIFCBasePass::_createDescriptorSetLayouts()
    {
        m_descriptorInfos.resize(layout_type_count);
        auto& ctx = *m_vkContext;
        {
            std::array<VkDescriptorSetLayoutBinding, 2> ubo_layout_bindings;
            // layout(std140, binding = 0)uniform TransformsUBO
            ubo_layout_bindings[0].binding = 0;
            ubo_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_layout_bindings[0].descriptorCount = 1;
            ubo_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            ubo_layout_bindings[0].pImmutableSamplers = nullptr;

            // layout(std140, binding = 1)uniform IFCRenderUBO
            ubo_layout_bindings[1].binding = 1;
            ubo_layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_layout_bindings[1].descriptorCount = 1;
            ubo_layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            ubo_layout_bindings[1].pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo layout_info{};
            layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_info.bindingCount = static_cast<uint32_t>(ubo_layout_bindings.size());
            layout_info.pBindings = ubo_layout_bindings.data();

            VK_CHECK_RESULT(vkCreateDescriptorSetLayout(ctx.m_device, &layout_info, nullptr, &m_descriptorInfos[layout_base].layout));
        }

        {

        }
    }

    void VulkanIFCBasePass::_createDescriptorSets()
    {
        auto& ctx = *m_vkContext;
        {
            VkDescriptorSetAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info.descriptorPool = m_descriptorPool;
            alloc_info.descriptorSetCount = 1U;
            alloc_info.pSetLayouts = &m_descriptorInfos[layout_base].layout;

            VK_CHECK_RESULT(vkAllocateDescriptorSets(ctx.m_device, &alloc_info, &m_descriptorInfos[layout_base].descriptor_set));

            // TODO vkUpdateDescriptorSets
            auto& uniform_buffer_map = m_vulkanResources->uniformBufferMap;

            VkDescriptorBufferInfo transforms_buffer_info{};
            transforms_buffer_info.buffer = uniform_buffer_map[uniform_buffer_transforms]->getBuffer();
            transforms_buffer_info.offset = 0;
            transforms_buffer_info.range = sizeof(TransformsUBO);

            VkDescriptorBufferInfo ifc_render_buffer_info{};
            ifc_render_buffer_info.buffer = uniform_buffer_map[uniform_buffer_ifc_render]->getBuffer();
            ifc_render_buffer_info.offset = 0;
            ifc_render_buffer_info.range = sizeof(IFCRenderUBO);

            std::array<VkWriteDescriptorSet, 2> write_desc_sets{};
            write_desc_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_desc_sets[0].dstSet = m_descriptorInfos[layout_base].descriptor_set;
            write_desc_sets[0].dstBinding = 0;
            write_desc_sets[0].dstArrayElement = 0;
            write_desc_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_desc_sets[0].descriptorCount = 1;
            write_desc_sets[0].pBufferInfo = &transforms_buffer_info;

            write_desc_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_desc_sets[1].dstSet = m_descriptorInfos[layout_base].descriptor_set;
            write_desc_sets[1].dstBinding = 1;
            write_desc_sets[1].dstArrayElement = 0;
            write_desc_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_desc_sets[1].descriptorCount = 1;
            write_desc_sets[1].pBufferInfo = &ifc_render_buffer_info;

            vkUpdateDescriptorSets(ctx.m_device, static_cast<uint32_t>(write_desc_sets.size()), write_desc_sets.data(), 0, nullptr);
        }

    }

    void VulkanIFCBasePass::_createRenderPass()
    {
        VulkanContext& ctx = *m_vkContext;

        // 1. attach desc
        // color attach
        VkAttachmentDescription color_attach{};
        color_attach.format = m_framebuffer.attachments[0].format;
        // color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attach.samples = VK_SAMPLE_COUNT_4_BIT;
        color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        color_attach.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // depth attach
        VkAttachmentDescription depth_attach{};
        depth_attach.format = m_framebuffer.attachments[1].format;
        // depth_attach.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attach.samples = VK_SAMPLE_COUNT_4_BIT;
        depth_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attach.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attach.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription color_attach_resolve{};
        color_attach_resolve.format = ctx.m_swapchainImageFormat;
        color_attach_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attach_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attach_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attach_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attach_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attach_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attach_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // 2. attach ref
        // layout(location = 0) out vec4 FragColor
        VkAttachmentReference color_attach_ref{};
        color_attach_ref.attachment = 0;
        color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depth_attach_ref{};
        depth_attach_ref.attachment = 1;
        depth_attach_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentReference color_attach_resolve_ref{};
        color_attach_resolve_ref.attachment = 2;
        color_attach_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // 3. subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attach_ref;
        subpass.pDepthStencilAttachment = &depth_attach_ref;
        subpass.pResolveAttachments = &color_attach_resolve_ref;

        // 4. subpass dependency
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        // 5. create renderpass
        std::array<VkAttachmentDescription, 3> attachments = {
            color_attach,
            depth_attach,
            color_attach_resolve
        };
        VkRenderPassCreateInfo render_pass_ci{};
        render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_ci.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_ci.pAttachments = attachments.data();
        render_pass_ci.subpassCount = 1;
        render_pass_ci.pSubpasses = &subpass;
        render_pass_ci.dependencyCount = 1;
        render_pass_ci.pDependencies = &dependency;

        VK_CHECK_RESULT(vkCreateRenderPass(ctx.m_device, &render_pass_ci, nullptr, &m_framebuffer.render_pass));
    }

    void VulkanIFCBasePass::_createPipeline()
    {
        auto& ctx = *m_vkContext;
        m_renderPipeline.resize(render_pipeline_count);
        auto vert_code = VulkanUtil::compileFile("shaders/test.vert", shaderc_glsl_vertex_shader);
        auto frag_code = VulkanUtil::compileFile("shaders/test.frag", shaderc_glsl_fragment_shader);
        VkShaderModule vert_shader_module = VulkanUtil::createShaderModule(ctx.m_device, vert_code);
        VkShaderModule frag_shader_module = VulkanUtil::createShaderModule(ctx.m_device, frag_code);
        {
            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vert_shader_module;
            vertShaderStageInfo.pName = "main";
            vertShaderStageInfo.pSpecializationInfo = nullptr;

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = frag_shader_module;
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            auto bindingDesc = IFCVulkanVertex::getBindingDescription();
            auto attriDesc = IFCVulkanVertex::getAttributeDescriptions();
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = attriDesc.size();
            vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
            vertexInputInfo.pVertexAttributeDescriptions = attriDesc.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            // VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
            // VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
            // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
            // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
            // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)ctx.m_swapchainExtent.width;
            viewport.height = (float)ctx.m_swapchainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = ctx.m_swapchainExtent;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
            // VK_POLYGON_MODE_LINE : polygon edges are drawn as lines
            // VK_POLYGON_MODE_POINT : polygon vertices are drawn as points
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            //rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            //rasterizer.depthBiasClamp = 0.0f; // Optional
            //rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            // multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
            //multisampling.minSampleShading = 1.0f; // Optional
            //multisampling.pSampleMask = nullptr; // Optional
            //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            //multisampling.alphaToOneEnable = VK_FALSE; // Optional

            // After a fragment shader has returned a color
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &m_descriptorInfos[layout_base].layout;
            pipelineLayoutInfo.pushConstantRangeCount = 0;

            VK_CHECK_RESULT(vkCreatePipelineLayout(ctx.m_device, &pipelineLayoutInfo, nullptr, &m_renderPipeline[render_pipeline_opaque].layout));

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional

            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {}; // Optional

            VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

            VkPipelineDynamicStateCreateInfo dynamic_state_ci{};
            dynamic_state_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic_state_ci.dynamicStateCount = 2;
            dynamic_state_ci.pDynamicStates = dynamic_states;

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.layout = m_renderPipeline[render_pipeline_opaque].layout;
            pipelineInfo.renderPass = m_framebuffer.render_pass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pDynamicState = &dynamic_state_ci;

            VK_CHECK_RESULT(vkCreateGraphicsPipelines(ctx.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_renderPipeline[render_pipeline_opaque].pipeline));
            
            // transparency pipeline
            VK_CHECK_RESULT(vkCreatePipelineLayout(ctx.m_device, &pipelineLayoutInfo, nullptr, &m_renderPipeline[render_pipeline_transparency].layout));

            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            pipelineInfo.pColorBlendState = &colorBlending;
            VK_CHECK_RESULT(vkCreateGraphicsPipelines(ctx.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_renderPipeline[render_pipeline_transparency].pipeline));
        }


        // ---------------------clean up shader-------------------------------
        vkDestroyShaderModule(ctx.m_device, frag_shader_module, nullptr);
        vkDestroyShaderModule(ctx.m_device, vert_shader_module, nullptr);
    }

    void VulkanIFCBasePass::_createFramebuffers()
    {
        auto& ctx = *m_vkContext;
        m_swapchainFramebuffers.resize(ctx.m_swapchainImageViews.size());

        for (size_t i = 0; i < ctx.m_swapchainImageViews.size(); ++i)
        {
            std::array<VkImageView, 3> attachments = {
                m_framebuffer.attachments[0].view
                , m_framebuffer.attachments[1].view
                , ctx.m_swapchainImageViews[i]

            };

            VkFramebufferCreateInfo framebuffer_ci{};
            framebuffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_ci.renderPass = m_framebuffer.render_pass;
            framebuffer_ci.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_ci.pAttachments = attachments.data();
            framebuffer_ci.width = ctx.m_swapchainExtent.width;
            framebuffer_ci.height = ctx.m_swapchainExtent.height;
            framebuffer_ci.layers = 1;

            VK_CHECK_RESULT(vkCreateFramebuffer(ctx.m_device, &framebuffer_ci, nullptr, &m_swapchainFramebuffers[i]));
        }
    }

}