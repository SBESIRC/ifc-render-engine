#include "gl_render.h"

#include "gl/shader_consts.h"
#include "../common/ifc_util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace ifcre {
// ------------ construction ---------------------
	GLRender::GLRender()
	{
		// program init
		String v_image_effect = util::read_file("shaders/image_effect.vert");
		String f_image_effect = util::read_file("shaders/image_effect.frag");
		m_offscreen_program = make_unique<GLSLProgram>(v_image_effect.c_str(), f_image_effect.c_str());
		m_offscreen_program->use();
		//m_offscreen_program->setInt("screenTexture", 0);
		String v_nd = util::read_file("shaders/normal_depth_write.vert");
		String f_nd = util::read_file("shaders/normal_depth_write.frag");
		m_normal_depth_program = make_unique<GLSLProgram>(v_nd.c_str(), f_nd.c_str());
		m_normal_depth_program->use();

		String v_axis = util::read_file("shaders/axis.vert");
		String f_axis = util::read_file("shaders/axis.frag");
		m_axis_shader = make_unique<GLSLProgram>(v_axis.c_str(), f_axis.c_str());


		m_test_shader = make_unique<GLSLProgram>(v_test, f_test);
		//glm::mat4 model(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));

		//glm::mat4 view(1.0f);
		//glm::vec3 eyepos(0.0f, 0.0f, 5.0f);
		//glm::vec3 front(0.0f, 0.0f, -1.0f);
		//glm::vec3 up(0.0f, 1.0f, 0.0f);
		//view = glm::lookAt(eyepos, eyepos + front, up);

		//glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
		//m_test_shader->use();
		//m_test_shader->setMat4("model", model);
		//m_test_shader->setMat4("view", view);
		//m_test_shader->setMat4("projection", projection);
		// ----- ----- ----- ----- ----- -----

		// -------------- render init --------------
		glLineWidth(3.0f);

		// ----- ----- ----- ----- ----- ----- -----

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

	void GLRender::render(uint32_t render_id, RenderTypeEnum type)
	{
		auto& vb_map = m_vertex_buffer_map;
		auto it = vb_map.find(render_id);
		if (it == vb_map.end()) {
			printf("The render_id[%u] is not existed.\n", render_id);
			return;
		}
		switch (type) {
		case NORMAL_DEPTH_WRITE: {
			auto& color = m_depnor_value;
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearDepthf(1.0);
			m_normal_depth_program->use();
			m_normal_depth_program->setMat4("mvp",  m_projection * m_modelview);
			m_normal_depth_program->setMat3("t_inv_model", glm::transpose(glm::inverse(m_modelview)));
			break;
		}
		case DEFAULT_SHADING: {
			auto& color = m_bg_color;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_test_shader->use();
			m_test_shader->setMat4("modelview", m_modelview);
			m_test_shader->setMat4("projection", m_projection);
			//m_test_shader->setMat4("view", m_camera->getViewMatrix());
			break;
		}

		default:break;
			
		}
		SharedPtr<GLVertexBuffer> vb = it->second;
		vb->draw();
	}

	void GLRender::renderAxis(const glm::mat4& m, const glm::vec3& pick_center, const glm::vec3& model_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos)
	{
		static bool first = true;
		static uint32_t axis_vao;
		if (first) {
			float coord_axis[] = {
				0.0, 0.0, 0.0,
				1.0, 0.0, 0.0,	// x-axis
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0,	// y-axis
				0.0, 0.0, 0.0,
				0.0, 0.0, 1.0	// z-axis
			};
			uint32_t axis_vbo;
			glGenVertexArrays(1, &axis_vao);
			glGenBuffers(1, &axis_vbo);
			glBindVertexArray(axis_vao);
			glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_axis), &coord_axis, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			first = false;
		}
		m_axis_shader->use();
		glm::mat4 model = m;
		//model[3][0] = model[3][1] = model[3][2] = 0;
		glm::mat4 trans_center(1.0f);
		glm::mat4 trans_click_center(1.0f);
		trans_center = glm::translate(trans_center, model_center);
		model = model * trans_center;
		glm::vec3 world_pos(model[3][0], model[3][1], model[3][2]);
		
		float len_ref = glm::length(init_view_pos - model_center);
		float len = glm::length(view_pos - pick_center);
		float scale = len / len_ref * 15;
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		trans_click_center = glm::translate(trans_click_center, pick_center - world_pos);
		model = trans_click_center * model;

		m_axis_shader->setMat4("mvp", m_projection * m_view * model);
		glBindVertexArray(axis_vao);
		glDisable(DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glDrawArrays(GL_LINES, 0, 6);
		_defaultConfig();

	}

	void GLRender::postRender(uint32_t col_tex_id, uint32_t depth_normal_tex_id)
	{
		static bool first = true;
		static uint32_t off_vao;
		if (first) {
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
			
			first = false;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, col_tex_id);
		if (depth_normal_tex_id != -1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depth_normal_tex_id);
		}
		m_offscreen_program->use();
		m_offscreen_program->setInt("screenTexture", 0);
		m_offscreen_program->setInt("depthNormalTexture", 1);
		
		glBindVertexArray(off_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		_defaultConfig();

	}

	void GLRender::postRender(RenderWindow& w)
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
		glBindTexture(GL_TEXTURE_2D, w.getColorTexId());
		if (w.getDepthNormalTexId() != -1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, w.getDepthNormalTexId());
		}
		m_offscreen_program->use();
		m_offscreen_program->setInt("screenTexture", 0);
		m_offscreen_program->setInt("depthNormalTexture", 1);
		glm::vec2 win_size = w.getWindowSize();
		glm::vec2 win_texel_size = glm::vec2(1.0 / win_size.x, 1.0 / win_size.y);
		m_offscreen_program->setVec2("screenTexTexelSize", win_texel_size);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(off_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		_defaultConfig();
	}

	void GLRender::_defaultConfig()
	{
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
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

	void GLRender::depthFunc(GLFuncEnum func)
	{
		switch (func) {
		case ALWAYS_FUNC: glDepthFunc(GL_ALWAYS); break;
		case NEVER_FUNC: glDepthFunc(GL_NEVER); break;
		case LESS_FUNC: glDepthFunc(GL_LESS); break;
		case EQUAL_FUNC: glDepthFunc(GL_EQUAL); break;
		case LEQUAL_FUNC: glDepthFunc(GL_LEQUAL); break;
		case GREATER_FUNC: glDepthFunc(GL_GREATER); break;
		case NOTEQUAL_FUNC: glDepthFunc(GL_NOTEQUAL); break;
		case GEQUAL_FUNC: glDepthFunc(GL_GEQUAL); break;
		}
	}

	uint32_t GLRender::addModel(SharedPtr<GLVertexBuffer> vertex_buffer)
	{
		uint32_t id = util::get_next_globalid();
		m_vertex_buffer_map.insert(make_pair(id, vertex_buffer));
		return id;
	}

	void GLRender::setViewMatrix(const glm::mat4& view) {
		m_view = view;
	}

	void GLRender::setModelViewMatrix(const glm::mat4& mv)
	{
		m_modelview = mv;
	}
	void GLRender::setProjectionMatrix(const glm::mat4& projection)
	{
		m_projection = projection;
	}
} 