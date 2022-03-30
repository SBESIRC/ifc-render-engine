#pragma once

#ifndef IFCRE_GL_RENDER_H_
#define IFCRE_GL_RENDER_H_

#include <glad/glad.h>
#include <stdint.h>
#include "../common/std_types.h"
#include "render_window.h"
#include "gl/glsl_program.h"
#include "gl/gl_vertex_buffer.h"
#include "gl/gl_enum.h"
#include "gl_camera.h"

#include "../common/ifc_enum.h"

namespace ifcre {

	struct GLColor {
		Real r;
		Real g;
		Real b;
		Real a;
	};

	class GLRender{
	public: 
		GLRender();

	// ----------- low-level interfaces ---------------
		void clearFrameBuffer(GLClearEnum clear, GLColor* color = nullptr, Real depth = 1.0f);

		void enableTest(GLTestEnum test);
		void disableTest(GLTestEnum test);

	// ----- ----- ----- ----- ----- ----- ----- -----

		uint32_t addModel(SharedPtr<GLVertexBuffer> vertex_buffer);

		void setModelViewMatrix(glm::mat4& mv);

	// --------------- render ----------------------

		void render(uint32_t render_id, RenderTypeEnum type);

		// for offscreen
		void postRender(uint32_t col_tex_id);
	// ----- ----- ----- ----- ----- ----- ----- -----

	private:
		Map<uint32_t, SharedPtr<GLVertexBuffer>> m_vertex_buffer_map;

	private:

		glm::mat4 m_mv;

		// --------------- glsl program -----------------
		UniquePtr<GLSLProgram> m_offscreen_program;
		UniquePtr<GLSLProgram> m_normal_depth_program;
		UniquePtr<GLSLProgram> m_test_shader;

		// ----- ----- ----- ----- ----- ----- ----- -----

		
		// 1. 
		// 2. 
		// 3. 
		// 4. 
	};
};

#endif