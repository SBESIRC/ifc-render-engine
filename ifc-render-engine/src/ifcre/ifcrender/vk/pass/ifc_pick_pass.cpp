#include "ifc_pick_pass.h"
#include "../../uniform_obj.h"
#include "../vulkan_mesh.h"
#include "../vulkan_util.h"
#include "../../../common/ifc_util.h"
#include <array>
namespace ifcre {
	void VulkanIFCPickPass::initialize()
	{
		VulkanPassBase::initialize();

		_createAttachments();
		_createDescriptorSetLayouts();
		_createDescriptorSets();
		_createRenderPass();
		_createPipeline();
		_createFramebuffers();
	}

	void VulkanIFCPickPass::recreateFramebuffers()
	{
		auto& ctx = *m_vkContext;
		for (int i = 0; i < m_framebuffer.attachments.size(); ++i) {
			vkDestroyImageView(ctx.m_device, m_framebuffer.attachments[i].view, nullptr);
			vkDestroyImage(ctx.m_device, m_framebuffer.attachments[i].image, nullptr);
			vkFreeMemory(ctx.m_device, m_framebuffer.attachments[i].mem, nullptr);
		}
		vkDestroyFramebuffer(ctx.m_device, m_pickFramebuffer, nullptr);

		_createAttachments();
		_createFramebuffers();
	}

	void VulkanIFCPickPass::draw(uint32_t render_id)
	{
		// TODO

		//auto& ctx = *m_vkContext;
		//auto cur_cmd_buffer = m_commandInfo.curCmdBuffer;
		//auto mesh_it = m_vulkanResources->meshBufferMap.find(render_id);
		//if (mesh_it == m_vulkanResources->meshBufferMap.end()) {
		//	return;
		//}
		//auto& mesh_buffer = mesh_it->second;
		//VulkanBuffer& vertex_buffer = *(mesh_buffer.vertexBuffer);
		//VulkanBuffer& g_index_buffer = *(mesh_buffer.gIndexBuffer);
		//{
		//	VkRenderPassBeginInfo renderpass_begin_info{};
		//	renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		//	renderpass_begin_info.renderPass = m_framebuffer.render_pass;
		//	renderpass_begin_info.framebuffer = m_pickFramebuffer;
		//	renderpass_begin_info.renderArea.offset = { 0, 0 };
		//	renderpass_begin_info.renderArea.extent = ctx.m_swapchainExtent;

		//	std::array<VkClearValue, 2> clear_values;
		//	clear_values[0].color = { {-2.0f, 0.0f, 0.0f, 0.0f} };
		//	clear_values[1].depthStencil = { 1.0f, 0 };
		//	renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		//	renderpass_begin_info.pClearValues = clear_values.data();
		//	ctx.fp_vkCmdBeginRenderPass(cur_cmd_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		//}
		//ctx.fp_vkCmdBindPipeline(cur_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[0].pipeline);
		//ctx.fp_vkCmdSetViewport(cur_cmd_buffer, 0, 1, &m_commandInfo.viewport);
		//ctx.fp_vkCmdSetScissor(cur_cmd_buffer, 0, 1, &m_commandInfo.scissor);

		//VkBuffer vertex_buffers[] = { vertex_buffer.getBuffer() };
		//VkDeviceSize offsets[] = { 0 };
		//// render all
		//ctx.fp_vkCmdBindVertexBuffers(cur_cmd_buffer, 0, 1, vertex_buffers, offsets);
		//ctx.fp_vkCmdBindDescriptorSets(cur_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[0].layout, 0, 1, &m_descriptorInfos[0].descriptor_set, 0, nullptr);
		//ctx.fp_vkCmdBindIndexBuffer(cur_cmd_buffer, g_index_buffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		//ctx.fp_vkCmdDrawIndexed(cur_cmd_buffer, g_index_buffer.getSize(), 1, 0, 0, 0);

		//ctx.fp_vkCmdEndRenderPass(cur_cmd_buffer);
	}

	glm::ivec2 VulkanIFCPickPass::pick(uint32_t render_id, int32_t x, int32_t y, PickTypeEnum pick_type)
	{
		glm::ivec2 res;
		auto mesh_it = m_vulkanResources->meshBufferMap.find(render_id);
		if (mesh_it == m_vulkanResources->meshBufferMap.end()) {
			return glm::ivec2(-1, -1);
		}
		auto& mesh_buffer = mesh_it->second;
		VulkanBuffer& vertex_buffer = *(mesh_buffer.vertexBuffer);
		VulkanBuffer& g_index_buffer = *(mesh_buffer.gIndexBuffer);

		auto& ctx = *m_vkContext;
		auto cur_frame_index = *m_commandInfo.p_currentFrameIndex;
		auto cur_cmd_buffer = m_commandInfo.curCmdBuffer;
		auto cur_in_flight_fence = m_commandInfo.curInFlightFence;
		VK_CHECK_RESULT(ctx.fp_vkWaitForFences(ctx.m_device, 1, &cur_in_flight_fence, VK_TRUE, UINT64_MAX));

		VkCommandBufferBeginInfo cmd_buffer_begin_info{};
		cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmd_buffer_begin_info.flags = 0;
		cmd_buffer_begin_info.pInheritanceInfo = nullptr;
		VK_CHECK_RESULT(ctx.fp_vkBeginCommandBuffer(cur_cmd_buffer, &cmd_buffer_begin_info));

		{
			std::array<VkImageMemoryBarrier,2> transfer_to_render_barriers{};
			transfer_to_render_barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			transfer_to_render_barriers[0].pNext = nullptr;
			transfer_to_render_barriers[0].srcAccessMask = 0;
			transfer_to_render_barriers[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			transfer_to_render_barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			transfer_to_render_barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			transfer_to_render_barriers[0].srcQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			transfer_to_render_barriers[0].dstQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			transfer_to_render_barriers[0].image = m_framebuffer.attachments[attach_comp_id].image;
			transfer_to_render_barriers[0].subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

			transfer_to_render_barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			transfer_to_render_barriers[1].pNext = nullptr;
			transfer_to_render_barriers[1].srcAccessMask = 0;
			transfer_to_render_barriers[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			transfer_to_render_barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			transfer_to_render_barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			transfer_to_render_barriers[1].srcQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			transfer_to_render_barriers[1].dstQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			transfer_to_render_barriers[1].image = m_framebuffer.attachments[attach_depth].image;
			transfer_to_render_barriers[1].subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
			vkCmdPipelineBarrier(cur_cmd_buffer,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				static_cast<uint32_t>(transfer_to_render_barriers.size()),
				transfer_to_render_barriers.data());
		}

		{
			VkRenderPassBeginInfo renderpass_begin_info{};
			renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpass_begin_info.renderPass = m_framebuffer.render_pass;
			renderpass_begin_info.framebuffer = m_pickFramebuffer;
			renderpass_begin_info.renderArea.offset = { 0, 0 };
			renderpass_begin_info.renderArea.extent = ctx.m_swapchainExtent;

			std::array<VkClearValue, 2> clear_values;
			clear_values[0].color = { {-2.0f, 0.0f, 0.0f, 0.0f} };
			clear_values[1].depthStencil = { 1.0f, 0 };
			renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			renderpass_begin_info.pClearValues = clear_values.data();
			ctx.fp_vkCmdBeginRenderPass(cur_cmd_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}
		ctx.fp_vkCmdBindPipeline(cur_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[0].pipeline);
		ctx.fp_vkCmdSetViewport(cur_cmd_buffer, 0, 1, &m_commandInfo.viewport);
		ctx.fp_vkCmdSetScissor(cur_cmd_buffer, 0, 1, &m_commandInfo.scissor);

		VkBuffer vertex_buffers[] = { vertex_buffer.getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		// render all
		ctx.fp_vkCmdBindVertexBuffers(cur_cmd_buffer, 0, 1, vertex_buffers, offsets);
		ctx.fp_vkCmdBindDescriptorSets(cur_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderPipeline[0].layout, 0, 1, &m_descriptorInfos[0].descriptor_set, 0, nullptr);
		ctx.fp_vkCmdBindIndexBuffer(cur_cmd_buffer, g_index_buffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		ctx.fp_vkCmdDrawIndexed(cur_cmd_buffer, g_index_buffer.getSize(), 1, 0, 0, 0);

		ctx.fp_vkCmdEndRenderPass(cur_cmd_buffer);
		ctx.fp_vkEndCommandBuffer(cur_cmd_buffer);

		ctx.fp_vkResetFences(ctx.m_device, 1, &cur_in_flight_fence);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 0;
		submit_info.pWaitSemaphores = nullptr;
		submit_info.pWaitDstStageMask = nullptr;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cur_cmd_buffer;
		submit_info.signalSemaphoreCount = 0;
		submit_info.pSignalSemaphores = nullptr;
		VK_CHECK_RESULT(vkQueueSubmit(ctx.m_graphicsQueue
			, 1
			, &submit_info
			, cur_in_flight_fence));

		auto new_index = (*m_commandInfo.p_currentFrameIndex + 1) % m_commandInfo.maxFramesInFlight;
		*m_commandInfo.p_currentFrameIndex = new_index;

		// implicit host read barrier
		VK_CHECK_RESULT(ctx.fp_vkWaitForFences(ctx.m_device,
		                                                           m_commandInfo.maxFramesInFlight,
		                                                           m_commandInfo.inFlightFences,
		                                                           VK_TRUE,
		                                                           UINT64_MAX));
		//vkQueueWaitIdle(ctx.m_graphicsQueue);

		// 
		if((pick_type & PickTypeEnum::pick_depth) != 0)
		{
			VkCommandBuffer command_buffer = ctx.beginSingleTimeCommand();
			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { x, y, 0 };
			region.imageExtent = { 1, 1, 1 };

			uint32_t buffer_size = 4;
			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			VulkanUtil::createBuffer(ctx.m_physicalDevice,
				ctx.m_device,
				buffer_size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				staging_buffer,
				staging_buffer_memory);

			VkImage depth_image = m_framebuffer.attachments[attach_depth].image;
			VkImageMemoryBarrier copy_to_buffer_barrier{};
			copy_to_buffer_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_to_buffer_barrier.pNext = nullptr;
			copy_to_buffer_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			copy_to_buffer_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			copy_to_buffer_barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			copy_to_buffer_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			copy_to_buffer_barrier.srcQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			copy_to_buffer_barrier.dstQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			copy_to_buffer_barrier.image = depth_image;
			copy_to_buffer_barrier.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&copy_to_buffer_barrier);

			vkCmdCopyImageToBuffer(command_buffer,
				depth_image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				staging_buffer,
				1,
				&region);
			ctx.endSingleTimeCommand(command_buffer);
	

			float* depth_data = nullptr;
			vkMapMemory(ctx.m_device, staging_buffer_memory, 0, buffer_size, 0, (void**)&depth_data);
			vkUnmapMemory(ctx.m_device, staging_buffer_memory);

			vkDestroyBuffer(ctx.m_device, staging_buffer, nullptr);
			vkFreeMemory(ctx.m_device, staging_buffer_memory, nullptr);

			res.x = util::float_as_int(depth_data[0]);
		}
		
		if ((pick_type & PickTypeEnum::pick_comp_id) != 0)
		{
			VkCommandBuffer command_buffer = ctx.beginSingleTimeCommand();
			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { x, y, 0 };
			region.imageExtent = { 1, 1, 1 };

			uint32_t buffer_size = 4;
			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			VulkanUtil::createBuffer(ctx.m_physicalDevice,
				ctx.m_device,
				buffer_size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				staging_buffer,
				staging_buffer_memory);

			VkImage comp_id_image = m_framebuffer.attachments[attach_comp_id].image;
			VkImageMemoryBarrier copy_to_buffer_barrier{};
			copy_to_buffer_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			copy_to_buffer_barrier.pNext = nullptr;
			copy_to_buffer_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			copy_to_buffer_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			copy_to_buffer_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			copy_to_buffer_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			copy_to_buffer_barrier.srcQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			copy_to_buffer_barrier.dstQueueFamilyIndex = ctx.m_queueFamilyIndices.graphicsFamily.value();
			copy_to_buffer_barrier.image = comp_id_image;
			copy_to_buffer_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&copy_to_buffer_barrier);

			vkCmdCopyImageToBuffer(command_buffer,
				comp_id_image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				staging_buffer,
				1,
				&region);
			ctx.endSingleTimeCommand(command_buffer);


			int32_t* comp_id_data = nullptr;
			vkMapMemory(ctx.m_device, staging_buffer_memory, 0, buffer_size, 0, (void**)&comp_id_data);
			vkUnmapMemory(ctx.m_device, staging_buffer_memory);

			vkDestroyBuffer(ctx.m_device, staging_buffer, nullptr);
			vkFreeMemory(ctx.m_device, staging_buffer_memory, nullptr);

			res.y = comp_id_data[0];
		}

		return res;
	}

	void VulkanIFCPickPass::_createAttachments()
	{
		auto& ctx = *m_vkContext;
		m_framebuffer.attachments.resize(attach_count);
		m_framebuffer.attachments[attach_comp_id].format = VK_FORMAT_R32_SINT;
		m_framebuffer.attachments[attach_depth].format = ctx.m_depthImageFormat;

		m_framebuffer.width = ctx.m_swapchainExtent.width;
		m_framebuffer.height = ctx.m_swapchainExtent.height;

		// picking comp id image
		{
			VulkanUtil::createImage(ctx.m_physicalDevice,
				ctx.m_device,
				m_framebuffer.width,
				m_framebuffer.height,
				m_framebuffer.attachments[attach_comp_id].format,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_framebuffer.attachments[attach_comp_id].image,
				m_framebuffer.attachments[attach_comp_id].mem,
				0,
				1,
				1,
				VK_SAMPLE_COUNT_1_BIT);
			m_framebuffer.attachments[attach_comp_id].view = VulkanUtil::createImageView(ctx.m_device
				, m_framebuffer.attachments[attach_comp_id].image
				, m_framebuffer.attachments[attach_comp_id].format
				, VK_IMAGE_ASPECT_COLOR_BIT
				, VK_IMAGE_VIEW_TYPE_2D
				, 1
				, 1);
		}

		// picking depth image
		{
			VulkanUtil::createImage(ctx.m_physicalDevice,
				ctx.m_device,
				m_framebuffer.width,
				m_framebuffer.height,
				m_framebuffer.attachments[attach_depth].format,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_framebuffer.attachments[attach_depth].image,
				m_framebuffer.attachments[attach_depth].mem,
				0,
				1,
				1,
				VK_SAMPLE_COUNT_1_BIT);

			m_framebuffer.attachments[attach_depth].view = VulkanUtil::createImageView(ctx.m_device
				, m_framebuffer.attachments[attach_depth].image
				, m_framebuffer.attachments[attach_depth].format
				, VK_IMAGE_ASPECT_DEPTH_BIT
				, VK_IMAGE_VIEW_TYPE_2D
				, 1
				, 1);
		}
	}
    void VulkanIFCPickPass::_createDescriptorSetLayouts()
    {
        m_descriptorInfos.resize(1);
        auto& ctx = *m_vkContext;

        std::array<VkDescriptorSetLayoutBinding, 1> ubo_layout_bindings;
        // layout(std140, binding = 0)uniform TransformMVPUBO
        ubo_layout_bindings[0].binding = 0;
        ubo_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_bindings[0].descriptorCount = 1;
        ubo_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_bindings[0].pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(ubo_layout_bindings.size());
        layout_info.pBindings = ubo_layout_bindings.data();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(ctx.m_device, &layout_info, nullptr, &m_descriptorInfos[0].layout));
    }
    void VulkanIFCPickPass::_createDescriptorSets()
    {
        auto& ctx = *m_vkContext;

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = m_descriptorPool;
        alloc_info.descriptorSetCount = 1U;
        alloc_info.pSetLayouts = &m_descriptorInfos[0].layout;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(ctx.m_device, &alloc_info, &m_descriptorInfos[0].descriptor_set));

        // TODO vkUpdateDescriptorSets
        auto& uniform_buffer_map = m_vulkanResources->uniformBufferMap;

        VkDescriptorBufferInfo transform_mvp_buffer_info{};
        transform_mvp_buffer_info.buffer = uniform_buffer_map[uniform_buffer_transform_mvp]->getBuffer();
        transform_mvp_buffer_info.offset = 0;
        transform_mvp_buffer_info.range = sizeof(TransformMVPUBO);

        std::array<VkWriteDescriptorSet, 1> write_desc_sets{};
        write_desc_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc_sets[0].dstSet = m_descriptorInfos[0].descriptor_set;
        write_desc_sets[0].dstBinding = 0;
        write_desc_sets[0].dstArrayElement = 0;
        write_desc_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_desc_sets[0].descriptorCount = 1;
        write_desc_sets[0].pBufferInfo = &transform_mvp_buffer_info;

        vkUpdateDescriptorSets(ctx.m_device, static_cast<uint32_t>(write_desc_sets.size()), write_desc_sets.data(), 0, nullptr);
    }
	void VulkanIFCPickPass::_createRenderPass()
	{
		auto& ctx = *m_vkContext;
		VkAttachmentDescription color_attachment_description{};
		color_attachment_description.format = m_framebuffer.attachments[attach_comp_id].format;
		color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depth_attachment_description{};
		depth_attachment_description.format = m_framebuffer.attachments[attach_depth].format;
		depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::array<VkAttachmentDescription,2> attachments = { color_attachment_description, depth_attachment_description };

		VkAttachmentReference color_attachment_reference{};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_reference{};
		depth_attachment_reference.attachment = 1;
		depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_reference;
		subpass.pDepthStencilAttachment = &depth_attachment_reference;

		VkRenderPassCreateInfo render_pass_ci{};
        render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_ci.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_ci.pAttachments = attachments.data();
        render_pass_ci.subpassCount = 1;
        render_pass_ci.pSubpasses = &subpass;
        render_pass_ci.dependencyCount = 0;
        render_pass_ci.pDependencies = NULL;

        VK_CHECK_RESULT(vkCreateRenderPass(ctx.m_device, &render_pass_ci, nullptr, &m_framebuffer.render_pass));
	}

	void VulkanIFCPickPass::_createPipeline()
	{
		auto& ctx = *m_vkContext;

        m_renderPipeline.resize(1);
#ifdef _DEBUG
		auto vert_code = VulkanUtil::compileFile("shaders/comp_id_write.vert", shaderc_glsl_vertex_shader);
		auto frag_code = VulkanUtil::compileFile("shaders/comp_id_write.frag", shaderc_glsl_fragment_shader);
#else
		auto vert_code = VulkanUtil::compileString(new_sc::v_comp_id_write, shaderc_glsl_vertex_shader, true);
		auto frag_code = VulkanUtil::compileString(new_sc::f_comp_id_write, shaderc_glsl_fragment_shader, true);
#endif
        VkShaderModule vert_shader_module = VulkanUtil::createShaderModule(ctx.m_device, vert_code);
        VkShaderModule frag_shader_module = VulkanUtil::createShaderModule(ctx.m_device, frag_code);
        
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
		 multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
		pipelineLayoutInfo.pSetLayouts = &m_descriptorInfos[0].layout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		VK_CHECK_RESULT(vkCreatePipelineLayout(ctx.m_device, &pipelineLayoutInfo, nullptr, &m_renderPipeline[0].layout));

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
		pipelineInfo.layout = m_renderPipeline[0].layout;
		pipelineInfo.renderPass = m_framebuffer.render_pass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamic_state_ci;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(ctx.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_renderPipeline[0].pipeline));


        // ---------------------clean up shader-------------------------------
        vkDestroyShaderModule(ctx.m_device, frag_shader_module, nullptr);
        vkDestroyShaderModule(ctx.m_device, vert_shader_module, nullptr);
	}
	void VulkanIFCPickPass::_createFramebuffers()
	{
		auto& ctx = *m_vkContext;

		std::array<VkImageView, 2> attachments = {
			m_framebuffer.attachments[attach_comp_id].view
			, m_framebuffer.attachments[attach_depth].view
		};

		VkFramebufferCreateInfo framebuffer_ci{};
		framebuffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_ci.renderPass = m_framebuffer.render_pass;
		framebuffer_ci.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebuffer_ci.pAttachments = attachments.data();
		framebuffer_ci.width = ctx.m_swapchainExtent.width;
		framebuffer_ci.height = ctx.m_swapchainExtent.height;
		framebuffer_ci.layers = 1;

		VK_CHECK_RESULT(vkCreateFramebuffer(ctx.m_device, &framebuffer_ci, nullptr, &m_pickFramebuffer));
	}
}