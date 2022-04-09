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
		void depthFunc(GLFuncEnum func);

	// ----- ----- ----- ----- ----- ----- ----- -----

		uint32_t addModel(SharedPtr<GLVertexBuffer> vertex_buffer);

		void setModelViewMatrix(const glm::mat4& mv);
		void setViewMatrix(const glm::mat4& view);
		void setProjectionMatrix(const glm::mat4& projection);

	// --------------- render ----------------------

		void render(uint32_t render_id, RenderTypeEnum type);

		void renderAxis(const glm::mat4& m, const glm::vec3& pick_center, const glm::vec3& model_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos = glm::vec3(0.0f, 0.0f, 10.0f));
		// for offscreen
		void postRender(uint32_t col_tex_id, uint32_t depth_normal_tex_id = -1);
		void postRender(RenderWindow& w);
	// ----- ----- ----- ----- ----- ----- ----- -----

	private:
		Map<uint32_t, SharedPtr<GLVertexBuffer>> m_vertex_buffer_map;

	private:

		glm::mat4 m_modelview;
		glm::mat4 m_view;
		glm::mat4 m_projection;

		// --------------- glsl program -----------------
		UniquePtr<GLSLProgram> m_offscreen_program;
		UniquePtr<GLSLProgram> m_normal_depth_program;
		UniquePtr<GLSLProgram> m_test_shader;
		UniquePtr<GLSLProgram> m_axis_shader;

		// ----- ----- ----- ----- ----- ----- ----- -----

		
		// 1. 
		// 2. 
		// 3. 
		// 4. 
	};
};

#endif