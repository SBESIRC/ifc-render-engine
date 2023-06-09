#pragma once
#ifndef _IFCRE_VULKAN_BASE_PASS_H_
#define _IFCRE_VULKAN_BASE_PASS_H_

#include "../vulkan_pass_base.h"
namespace ifcre {

	class VulkanIFCBasePass : public VulkanPassBase {
		enum LayoutType : uint8_t
		{
			layout_base = 0,
			layout_edge,
			layout_bounding_box,
			layout_axis,
			layout_type_count
		};

		enum RenderPipelineEnum : uint8_t {
			render_pipeline_opaque = 0,
			render_pipeline_transparency,
			render_pipeline_edge,
			render_pipeline_bounding_box,
			render_pipeline_axis,
			render_pipeline_count
		};
		enum AttachmentEnum :uint8_t {
			attach_multi_color = 0,
			attach_multi_depth,
			attach_count
		};
	public:

		void draw(uint32_t mesh_id, bool bbx_draw);
		void initialize();
		void recreateFramebuffers();
	private:
		void _createAttachments();
		void _createDescriptorSetLayouts();
		void _createDescriptorSets();
		void _createRenderPass();
		void _createPipeline();
		void _createFramebuffers();

		std::vector<VkFramebuffer> m_swapchainFramebuffers;
	};

	

};

#endif