#include "gl_render.h"

#include "gl/shader_consts.h"
#include "../common/ifc_util.h"
namespace ifcre {
// ------------ construction ---------------------
	GLRender::GLRender()
	{
		m_offscreen_shader = make_unique<GLSLProgram>(v_offscreen, f_offscreen);
		m_offscreen_shader->use();
		m_offscreen_shader->setInt("screenTexture", 0);

		m_test_shader = make_unique<GLSLProgram>(v_test, f_test);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));

		glm::mat4 view(1.0f);
		glm::vec3 eyepos(0.0f, 0.0f, 3.0f);
		glm::vec3 front(0.0f, 0.0f, -1.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(eyepos, eyepos + front, up);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
		m_test_shader->use();
		m_test_shader->setMat4("model", model);
		m_test_shader->setMat4("view", view);
		m_test_shader->setMat4("projection", projection);
	}
// ----- ----- ----- ----- ----- ----- ----- ----- 

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

	void GLRender::render(uint32_t render_id)
	{
		auto& vb_map = m_vertex_buffer_map;
		auto it = vb_map.find(render_id);
		if (it == vb_map.end()) {
			printf("The render_id[%u] is not existed.\n", render_id);
			return;
		}

		m_test_shader->use();
		SharedPtr<GLVertexBuffer> vb = it->second;
		vb->draw();
	}
	void GLRender::render(uint32_t render_id, Vector<glm::vec3> bbx,Real zoom_parameter) {
		auto& vb_map = m_vertex_buffer_map;
		auto it = vb_map.find(render_id);
		if (it == vb_map.end()) {
			printf("The render_id[%u] is not existed.\n", render_id);
			return;
		}

		m_test_shader->use();
		auto scal = zoom_parameter / (bbx[1].x - bbx[0].x);
		std::cout << zoom_parameter << std::endl;
		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(scal, scal, scal));
		model = glm::translate(model, glm::vec3(-(bbx[0].x + bbx[1].x) / 2, -(bbx[0].y + bbx[1].y) / 2, -(bbx[0].z + bbx[1].z) / 2));
		m_test_shader->setMat4("model", model);
		SharedPtr<GLVertexBuffer> vb = it->second;
		vb->draw();
	}

	void GLRender::postRender(uint32_t col_tex_id)
	{
		static bool first = false;
		static uint32_t off_vao;
		if (!first) {
			float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
			};
			uint32_t off_vbo;
			glGenVertexArrays(1, &off_vao);
			glGenBuffers(1, &off_vbo);
			glBindVertexArray(off_vao);
			glBindBuffer(GL_ARRAY_BUFFER, off_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			
			first = true;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, col_tex_id);
		m_offscreen_shader->use();
		glBindVertexArray(off_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}

	void GLRender::enableTest(GLTestEnum test)
	{
		switch (test) {
			case 0x01: {
				glEnable(GL_DEPTH_TEST);
				break;
			}
			case 0x02: {
				glEnable(GL_STENCIL_TEST);
				break;
			}
			case 0x03: {
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_STENCIL_TEST);
				break;
			}
			default:break;
		}
	}

	void GLRender::disableTest(GLTestEnum test)
	{
		switch (test) {
		case 0x01: {
			glDisable(GL_DEPTH_TEST);
			break;
		}
		case 0x02: {
			glDisable(GL_STENCIL_TEST);
			break;
		}
		case 0x03: {
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			break;
		}
		default:break;
		}
	}

	uint32_t GLRender::addModel(SharedPtr<GLVertexBuffer> vertex_buffer)
	{
		uint32_t id = util::get_next_globalid();
		m_vertex_buffer_map.insert(make_pair(id, vertex_buffer));
		return id;
	}

}