#pragma once
#ifndef IFCRE_GL_ENUM_H_
#define IFCRE_GL_ENUM_H_

#include <cstdint>

namespace ifcre {


	enum GLClearEnum : uint8_t {
		CLEAR_COLOR = 0x01,
		CLEAR_DEPTH = 0x02,
		CLEAR_STENCIL = 0x04
	};

	enum GLTestEnum : uint8_t {
		DEPTH_TEST = 0x01,
		STENCIL_TEST = 0x02
	};

	enum GLFuncEnum : uint8_t {
		ALWAYS_FUNC = 0x00,
		NEVER_FUNC = 0x01,
		LESS_FUNC = 0x02,
		EQUAL_FUNC=0x03,
		LEQUAL_FUNC = 0x04,
		GREATER_FUNC = 0x05,
		NOTEQUAL_FUNC = 0x06,
		GEQUAL_FUNC = 0x07
	};
	
	enum GLTexFilterEnum : uint8_t {
		FILTER_NEAREST = 0x01,
		FILTER_BILINEAR = 0x02,
		FILTER_TRILINEAR = 0x03,
	};

	enum GLWrapEnum : uint8_t {
		WRAP_REPEAT = 0x01,
		WRAP_CLAMP = 0x02
	};

	enum GLRTColorFormatEnum : uint8_t {
		COLOR_RGBA8 = 0x01,
		COLOR_R32I = 0x02
	};

	enum GLRTDepthFormatEnum : uint8_t {
		DEPTH_WRITE_ONLY = 0x00,
		DEPTH0 = 0x01,
		DEPTH16 = 0x02,
		DEPTH24 = 0x03,
		DEPTH32 = 0x04
	};
}

#endif