﻿#pragma once
#ifndef IFCRE_ENUM_H_
#define IFCRE_ENUM_H_

namespace ifcre {
	enum class RenderTypeEnum {
		NORMAL_DEPTH_WRITE = 0x00,
		DEFAULT_SHADING = 0x01,
		OFFLINE_SHADING = 0x02,
		TRANSPARENCY_SHADING = 0x03,
		COMP_ID_WRITE = 0x04,
		BOUNDINGBOX_SHADING = 0x05,
		EDGE_SHADING = 0x06,
		CHOSEN_SHADING = 0x07,
		CHOSEN_TRANS_SHADING = 0x08,
		COLLISION_RENDER = 0x09,
		TEXT_RENDER = 0x0a
	};
	enum class RenderPartEnum {
		ALL = 0x01,
		DYNAMIC_ALL = 0x02,
		//NO_TRANS = 0x03,
		//TRANS = 0x04,
		BBX_LINE = 0x05,
		EDGE_LINE = 0x06,
		DYNAMIC_NO_TRANS = 0x07,
		DYNAMIC_TRANS = 0x08,
		DYNAMIC_EDGE_LINE = 0x09,
		CHOSEN_NO_TRANS = 0x0a,
		CHOSEN_TRANS = 0x0b,
		COLLISION = 0x0c
	};

	enum class RenderAPIEnum {
		OPENGL_RENDER_API = 0x01,
		VULKAN_RENDER_API = 0x02
	};
}

#endif
