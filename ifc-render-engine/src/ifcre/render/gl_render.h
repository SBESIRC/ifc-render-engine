#pragma once

#ifndef IFCRE_GL_RENDER_H_
#define IFCRE_GL_RENDER_H_

#include <glad/glad.h>
#include <stdint.h>
#include "../common/std_types.h"
#include "gl/render_window.h"
#include "gl/glsl_program.h"
#include "gl/vertex_buffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ifcre {

	enum GLClearEnum : uint8_t{
		CLEAR_COLOR = 0x01,
		CLEAR_DEPTH = 0x02,
		CLEAR_STENCIL = 0x04
	};

	enum GLTestEnum : uint8_t {
		DEPTH_TEST = 0x01,
		STENCIL_TEST = 0x02
	};

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

// --------------- render ----------------------

		void render(uint32_t render_id);
		void render(uint32_t render_id, Vector<glm::vec3> bbx, Real zoom_parameter);

		void postRender(uint32_t col_tex_id);
// --------------- shader set ----------------------
		void setMat4(const std::string& name, const glm::mat4& mat)const {
			m_test_shader->setMat4(name, mat);
		}
		void setVec3(const std::string& name, const glm::vec3& value)const {
			m_test_shader->setVec3(name, value);
		}
		void setInt(const std::string& name,int value)const {
			m_test_shader->setInt(name, value);
		}
// ----- ----- ----- ----- ----- ----- ----- -----

	private:
		Map<uint32_t, SharedPtr<GLVertexBuffer>> m_vertex_buffer_map;

	private:
		UniquePtr<GLSLProgram> m_offscreen_shader;
		UniquePtr<GLSLProgram> m_test_shader;

	};
};

#endif