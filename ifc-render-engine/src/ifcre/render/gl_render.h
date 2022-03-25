#pragma once

#ifndef IFCRE_GL_RENDER_H_
#define IFCRE_GL_RENDER_H_

#include <glad/glad.h>
#include <stdint.h>
#include "../common/std_types.h"
#include "gl/render_window.h"

namespace ifcre {

	enum GLClearEnum : uint8_t{
		CLEAR_COLOR = 0x01,
		CLEAR_DEPTH = 0x02,
		CLEAR_STENCIL = 0x04
	};

	struct GLColor {
		Real r;
		Real g;
		Real b;
		Real a;
	};

	

	class GLRender{
	public: 
		GLRender() {}

		void clearFrameBuffer(GLClearEnum clear, GLColor* color = nullptr, Real depth = 1.0f);

		// ----------- low-level interfaces ---------------
		void bindVao();

		uint32_t createVao();

		void render();
		// ----- ----- ----- ----- ----- ----- ----- -----
	private:


	};
};

#endif