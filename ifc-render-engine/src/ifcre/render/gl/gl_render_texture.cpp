#include "gl_render_texture.h"
#include <glad/glad.h>

namespace ifcre {
	// ------------------ construct ----------------------
	GLRenderTexture::GLRenderTexture(int32_t w, int32_t h, GLRTDepthFormatEnum depth)
		: m_width(w)
		, m_height(h)
		, m_rt_depth_format(depth)
		, m_filter_mode(FILTER_BILINEAR)
		, m_wrap_mode(WRAP_CLAMP)
		, m_rt_format(RGBA32)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_tex_id);
		glTextureStorage2D(m_tex_id, 1, GL_RGBA8, w, h);
		
		glTextureParameteri(m_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (depth != DEPTH0) {
			glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
			switch (depth) {
			case DEPTH_WRITE_ONLY:
				glCreateRenderbuffers(1, &m_depth_id);
				glNamedRenderbufferStorage(m_depth_id, GL_DEPTH24_STENCIL8, w, h);
				break;
			case DEPTH16:
				glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT16, w, h);
				//glTextureSubImage2D(mfb.depth_id, 0, 0, 0, w, h, GL_DEPTH_COMPONENT16, GL_UNSIGNED_INT, NULL);
				break;
			case DEPTH24:
				glTextureStorage2D(m_depth_id, 1, GL_DEPTH24_STENCIL8, w, h);
				//glTextureSubImage2D(m_depth_id, 0, 0, 0, w, h, GL_RG, GL_UNSIGNED_INT_24_8, NULL);
				break;
			default:break;
			}
		}
	}

	GLRenderTexture::~GLRenderTexture()
	{
		glDeleteTextures(1, &m_tex_id);
		if (m_depth_id != -1) {
			glDeleteTextures(1, &m_depth_id);
		}
	}

	uint32_t GLRenderTexture::getTexId()
	{
		return m_tex_id;
	}

	uint32_t GLRenderTexture::getDepthId()
	{
		return m_depth_id;
	}

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- 
}