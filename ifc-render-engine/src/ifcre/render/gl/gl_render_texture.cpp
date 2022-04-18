#include "gl_render_texture.h"
#include <glad/glad.h>
#include<glm/glm.hpp>

namespace ifcre {
	// ------------------ construct ----------------------
	GLRenderTexture::GLRenderTexture(int32_t w, int32_t h, GLRTDepthFormatEnum depth, bool open_aa)
		: m_width(w)
		, m_height(h)
		, m_rt_depth_format(depth)
		, m_filter_mode(FILTER_BILINEAR)
		, m_wrap_mode(WRAP_CLAMP)
		, m_rt_format(RGBA32)
		, m_open_aa(open_aa)
	{

		if (open_aa) {
			glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_tex_id);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex_id);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			//glTextureStorage2DMultisample(m_tex_id, 4, GL_RGBA8, w, h, false);

			//glTextureParameteri(m_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTextureParameteri(m_tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glCreateTextures(GL_TEXTURE_2D, 1, &m_tex_id);
			glTextureStorage2D(m_tex_id, 1, GL_RGBA8, w, h);

			glTextureParameteri(m_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (depth != DEPTH0) {
			//glCreateTextures(open_aa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &m_depth_id);
			switch (depth) {
			case DEPTH_WRITE_ONLY:
				glCreateRenderbuffers(1, &m_depth_id);
				if (open_aa) {
					//glNamedRenderbufferStorage(m_depth_id, GL_DEPTH24_STENCIL8, w, h);
					glNamedRenderbufferStorageMultisample(m_depth_id, 4, GL_DEPTH24_STENCIL8, w, h);
				}
				else {
					glNamedRenderbufferStorage(m_depth_id, GL_DEPTH24_STENCIL8, w, h);
				}
				//glNamedRenderbufferStorage(m_depth_id, GL_DEPTH_COMPONENT, w, h);
				break;
			case DEPTH16:
				glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
				glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT16, w, h);
				//glTextureSubImage2D(mfb.depth_id, 0, 0, 0, w, h, GL_DEPTH_COMPONENT16, GL_UNSIGNED_INT, NULL);
				break;
			case DEPTH24:
				glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
				glTextureStorage2D(m_depth_id, 1, GL_DEPTH24_STENCIL8, w, h);
				//glTextureSubImage2D(m_depth_id, 0, 0, 0, w, h, GL_RG, GL_UNSIGNED_INT_24_8, NULL);
				break;
			case DEPTH32:
				glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_id);
				if (open_aa) {
					//glTextureStorage2DMultisample(m_depth_id, 4, GL_DEPTH_COMPONENT32, w, h, true);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_id);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32, w, h, GL_TRUE);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
				}
				else {
					glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT32, w, h);
				}
				//glTextureStorage2D(m_depth_id, 1, GL_DEPTH_COMPONENT, w, h);
				//glTextureSubImage2D(m_depth_id, 0, 0, 0, w, h, GL_RED, GL_FLOAT, NULL);
				break;
			default:break;
			}
		}
	}

	GLRenderTexture::~GLRenderTexture()
	{
		glDeleteTextures(1, &m_tex_id);
		if (m_depth_id != -1) {
			if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glDeleteRenderbuffers(1, &m_depth_id);
			}
			else {
				glDeleteTextures(1, &m_depth_id);
			}
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
			if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glNamedFramebufferRenderbuffer(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);
			}                                                                                                                           
			else if(m_rt_depth_format != DEPTH0){
				if (m_rt_depth_format == DEPTH24) {
					glNamedFramebufferTexture(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_depth_id, 0);
				}
				else {
					glNamedFramebufferTexture(fbo_id, GL_DEPTH_ATTACHMENT, m_depth_id, 0);
				}
			}
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_tex_id, 0);
			if (m_rt_depth_format == DEPTH_WRITE_ONLY) {
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_id);
			}
			else if (m_rt_depth_format != DEPTH0) {
				/*if (m_rt_depth_format == DEPTH24) {
					glNamedFramebufferTexture(fbo_id, GL_DEPTH_STENCIL_ATTACHMENT, m_depth_id, 0);
				}
				else {
					glNamedFramebufferTexture(fbo_id, GL_DEPTH_ATTACHMENT, m_depth_id, 0);
				}*/
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- 
}