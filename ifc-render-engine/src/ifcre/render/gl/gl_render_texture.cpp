#include "gl_render_texture.h"
#include <glad/glad.h>
#include<glm/glm.hpp>

namespace ifcre {
	// ------------------ construct ----------------------
	GLRenderTexture::GLRenderTexture(int32_t w, int32_t h, GLRTDepthFormatEnum depth, bool open_aa, GLRTColorFormatEnum color)
		: m_width(w)
		, m_height(h)
		, m_rt_depth_format(depth)
		, m_filter_mode(FILTER_BILINEAR)
		, m_wrap_mode(WRAP_CLAMP)
		, m_rt_format(color)
		, m_open_aa(open_aa) // 仅在msaa的时候为true
	{

		if (open_aa) {
			glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_tex_id);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex_id);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		}
		else {
			glCreateTextures(GL_TEXTURE_2D, 1, &m_tex_id);
			if (color == COLOR_RGBA8) {
				glTextureStorage2D(m_tex_id, 1, GL_RGBA8, w, h);
			}
			else if (color == COLOR_R32I) {  // 只有 id 存储类是32位
				glTextureStorage2D(m_tex_id, 1, GL_R32I, w, h);
			}

			glTextureParameteri(m_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		//if (depth != DEPTH0) {
		//	switch (depth) {
		//	case DEPTH_WRITE_ONLY:
				glCreateRenderbuffers(1, &m_depth_id);
				if (open_aa) {
					glNamedRenderbufferStorageMultisample(m_depth_id, 4, GL_DEPTH24_STENCIL8, w, h);
				}
				else {
					glNamedRenderbufferStorage(m_depth_id, GL_DEPTH24_STENCIL8, w, h);
				}
				//break;
			//case DEPTH16:
			//	glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
			//	glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT16, w, h);
			//	break;
			//case DEPTH24:
			//	glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
			//	glTextureStorage2D(m_depth_id, 1, GL_DEPTH24_STENCIL8, w, h);
			//	break;
			//case DEPTH32:
			//	glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
			//	if (open_aa) {
			//		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_id);
			//		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32, w, h, GL_TRUE);
			//		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			//	}
			//	else {
			//		glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT32, w, h);
			//	}
			//	break;
		//	default:break;
		//	}
		//}
	}

	GLRenderTexture::~GLRenderTexture()
	{
		glDeleteTextures(1, &m_tex_id);
		if (m_depth_id != -1) {
			//if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glDeleteRenderbuffers(1, &m_depth_id);
			//}
			//else {
			//	glDeleteTextures(1, &m_depth_id);
			//}
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

	bool GLRenderTexture::isDepthWriteOnly()
	{
		return m_rt_depth_format == DEPTH_WRITE_ONLY;
	}

	bool GLRenderTexture::hasStencil()
	{
		return m_rt_depth_format == DEPTH24;
	}

	bool GLRenderTexture::isOpenAA()
	{
		return m_open_aa;
	}

	void GLRenderTexture::attach(uint32_t fbo_id)
	{
		if (!m_open_aa) {
			glNamedFramebufferTexture(fbo_id, GL_COLOR_ATTACHMENT0, m_tex_id, 0);
			//if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glNamedFramebufferRenderbuffer(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);
			//}
			//else if (m_rt_depth_format != DEPTH0) {
			//	if (m_rt_depth_format == DEPTH24) {
			//		glNamedFramebufferTexture(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_depth_id, 0);
			//	}
			//	else {
			//		glNamedFramebufferTexture(fbo_id, GL_DEPTH_ATTACHMENT, m_depth_id, 0);
			//	}
			//}
		}
		else {
			glNamedFramebufferTexture(fbo_id, GL_COLOR_ATTACHMENT0, m_tex_id, 0);
			//if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glNamedFramebufferRenderbuffer(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);
			//}
			//else if (m_rt_depth_format != DEPTH0) {
			//	if (m_rt_depth_format == DEPTH24) {
			//		glNamedFramebufferTexture(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_depth_id, 0);
			//	}
			//	else {
			//		glNamedFramebufferTexture(fbo_id, GL_DEPTH_ATTACHMENT, m_depth_id, 0);
			//	}
			//}
		}
	}

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- 
}
