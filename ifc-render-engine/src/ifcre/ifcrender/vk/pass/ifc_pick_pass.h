#pragma once
#ifndef _IFCRE_PICK_PASS_H_
#define _IFCRE_PICK_PASS_H_

#include "../vulkan_pass_base.h"
#include <cstdint>
#include <glm/glm.hpp>
namespace ifcre {
	class VulkanIFCPickPass : public VulkanPassBase {
		enum AttachmentEnum :uint8_t {
			attach_comp_id = 0,
			attach_depth,
			attach_count
		};
	public:
		void initialize();
		void recreateFramebuffers();
		glm::ivec2 pick(uint32_t render_id, int32_t x, int32_t y);
	private:
		void _createAttachments();
		void _createDescriptorSetLayouts();
		void _createDescriptorSets();
		void _createRenderPass();
		void _createPipeline();
		void _createFramebuffers();

		VkFramebuffer m_pickFramebuffer;
	};
}

#endif