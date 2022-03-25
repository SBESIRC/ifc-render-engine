#include "gl_render.h"

namespace ifcre {

	void GLRender::clearFrameBuffer(GLClearEnum clear, GLColor* color, Real depth)
	{
		switch (clear) {
			case 0x01: {
				auto& c = *color;
				glClearColor(c.r, c.g, c.b, c.a);
				glClear(GL_COLOR_BUFFER_BIT);
				break;
			}
			case 0x03: {
				auto& c = *color;
				glClearColor(c.r, c.g, c.b, c.a);
				glClearDepthf(depth);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				break;
			}
			case 0x07: {
				break;
			}
			default:break;
		}
	}

	void GLRender::bindVao()
	{

	}

	uint32_t GLRender::createVao()
	{
		//glCreateVertexArrays(1, );
		//return uint32_t();
	}

	void GLRender::render()
	{
	}

}