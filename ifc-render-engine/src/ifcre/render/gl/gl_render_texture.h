#pragma once
#ifndef IFCRE_GL_RENDER_TEXTURE_H_
#define IFCRE_GL_RENDER_TEXTURE_H_

#include <cstdint>
#include "gl_enum.h"
#include "../../common/std_types.h"

namespace ifcre {
	class GLRenderTexture {
	public:
		GLRenderTexture(int32_t w, int32_t h, GLRTDepthFormatEnum depth, bool open_aa = false, GLRTColorFormatEnum color = COLOR_RGBA8);
		~GLRenderTexture();

		uint32_t getTexId();
		uint32_t getDepthId();

		bool isDepthWriteOnly();
		bool hasStencil();

		bool isOpenAA();

		void attach(uint32_t fbo_id);

	private:
		int32_t m_width;
		int32_t m_height;
		// texture id
		uint32_t m_tex_id;
		// numbter of bits in depth buffer(0 16 24). Note that only 24 bit depth has stencil buffer 
		uint32_t m_depth_id = -1;

		GLTexFilterEnum m_filter_mode;
		GLWrapEnum m_wrap_mode;
		GLRTColorFormatEnum m_rt_format;
		GLRTDepthFormatEnum m_rt_depth_format;

		bool m_open_aa;
		
	};
}

#endif