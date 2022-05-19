#pragma once
#ifndef _IFCRE_UNIFORM_OBJ_H_
#define _IFCRE_UNIFORM_OBJ_H_

#include <glm/glm.hpp>
namespace ifcre {

	enum UniformBufferEnum : uint8_t {
		uniform_buffer_transforms = 0,
		uniform_buffer_ifc_render,
		uniform_buffer_count
	};

	struct TransformsUBO {
		glm::mat4 model;								// 0 ~ 64
		glm::mat4 proj_view_model;				// 64 ~ 128
		glm::mat3 transpose_inv_model;		// 128 ~ 176
	};

	struct IFCRenderUBO {
		float alpha;							// 0 ~ 4
		int c_comp;							// 4 ~ 8
		int h_comp;							// 8 ~ 12
		glm::vec3 cameraPos;			// 16 ~ 30
	};
}

#endif