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
		enum PickTypeEnum :uint8_t {
			pick_depth = 0x01,
			pick_comp_id = 0x02
		};
		void initialize();
		void recreateFramebuffers();
		void draw(uint32_t render_id);
		glm::ivec2 pick(uint32_t render_id, int32_t x, int32_t y, PickTypeEnum pick_type);
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