#include "gl_render.h"
#include "gl/gl_uniform_buffer.h"

#include "gl/shader_consts.h"
#include "../common/ifc_util.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace ifcre {

	// ------------ construction ---------------------
	GLRender::GLRender()
	{
		// mvp, trans_inv_model
		m_uniform_buffer_map.transformsUBO = make_shared<GLUniformBuffer>(sizeof(glm::mat4) * 4 + sizeof(glm::vec4) * 7);
		m_uniform_buffer_map.ifcRenderUBO = make_shared<GLUniformBuffer>(32);
		m_uniform_buffer_map.transformMVPUBO = make_shared<GLUniformBuffer>(sizeof(glm::mat4) * 2 + sizeof(glm::vec4) * 7);

		m_uniform_buffer_map.transformsUBO->bindRange(0);
		m_uniform_buffer_map.ifcRenderUBO->bindRange(1);
		m_uniform_buffer_map.transformMVPUBO->bindRange(2);

#ifdef _DEBUG
		// program init
		String v_image_effect = util::read_file("shaders/image_effect.vert");
		String f_image_effect = util::read_file("shaders/image_effect.frag");
		m_offscreen_program = make_unique<GLSLProgram>(v_image_effect.c_str(), f_image_effect.c_str());

		String v_normal_depth_write = util::read_file("shaders/normal_depth_write.vert");
		String f_normal_depth_write = util::read_file("shaders/normal_depth_write.frag");
		m_normal_depth_program = make_unique<GLSLProgram>(v_normal_depth_write.c_str(), f_normal_depth_write.c_str());

		String v_comp_id_write = util::read_file("shaders/comp_id_write.vert");
		String f_comp_id_write = util::read_file("shaders/comp_id_write.frag");
		m_comp_id_program = make_unique<GLSLProgram>(v_comp_id_write.c_str(), f_comp_id_write.c_str());

		String v_axis = util::read_file("shaders/axis.vert");
		String f_axis = util::read_file("shaders/axis.frag");
		m_axis_shader = make_unique<GLSLProgram>(v_axis.c_str(), f_axis.c_str());

		String v_clp_plane = util::read_file("shaders/clp_plane.vert");
		String f_clp_plane = util::read_file("shaders/clp_plane.frag");
		m_clip_plane_shader = make_unique<GLSLProgram>(v_clp_plane.c_str(), f_clp_plane.c_str());

		String v_test = util::read_file("shaders/test.vert");
		String f_test = util::read_file("shaders/test.frag");
		m_test_shader = make_unique<GLSLProgram>(v_test.c_str(), f_test.c_str());

		String v_chosen_test = util::read_file("shaders/chosen_test.vert");
		String f_chosen_test = util::read_file("shaders/chosen_test.frag");
		m_chosen_shader = make_unique<GLSLProgram>(v_chosen_test.c_str(), f_chosen_test.c_str());

		String v_bbx = util::read_file("shaders/bbx.vert");
		String f_bbx = util::read_file("shaders/bbx.frag");
		m_select_bbx_shader = make_unique<GLSLProgram>(v_bbx.c_str(), f_bbx.c_str());

		String v_edge = util::read_file("shaders/edge.vert");
		String f_edge = util::read_file("shaders/edge.frag");
		m_edge_shader = make_unique<GLSLProgram>(v_edge.c_str(), f_edge.c_str());

		String v_collision = util::read_file("shaders/collision.vert");
		String f_collision = util::read_file("shaders/collision.frag");
		m_collision_shader = make_unique<GLSLProgram>(v_collision.c_str(), f_collision.c_str());

		String v_text = util::read_file("shaders/text.vert");
		String f_text = util::read_file("shaders/text.frag");
		m_text_shader = make_unique<GLSLProgram>(v_text.c_str(), f_text.c_str());

#else 
		// program init
		m_offscreen_program = make_unique<GLSLProgram>(sc::v_image_effect, sc::f_image_effect);
		m_normal_depth_program = make_unique<GLSLProgram>(sc::v_normal_depth_write, sc::f_normal_depth_write);
		m_comp_id_program = make_unique<GLSLProgram>(sc::v_comp_id_write, sc::f_comp_id_write);
		m_axis_shader = make_unique<GLSLProgram>(sc::v_axis, sc::f_axis);
		m_test_shader = make_unique<GLSLProgram>(sc::v_test, sc::f_test);
		m_chosen_shader = make_unique<GLSLProgram>(sc::v_chosen_test, sc::f_chosen_test);
		m_select_bbx_shader = make_unique<GLSLProgram>(sc::v_bbx, sc::f_bbx);
		m_edge_shader = make_unique<GLSLProgram>(sc::v_edge, sc::f_edge);
		m_clip_plane_shader = make_unique<GLSLProgram>(sc::v_clp_plane, sc::f_clp_plane);
		m_collision_shader = make_unique<GLSLProgram>(sc::v_collision, sc::f_collision);
		m_text_shader = make_unique<GLSLProgram>(sc::v_text, sc::f_text);
#endif

		m_test_shader->bindUniformBlock("TransformsUBO", 0);
		m_test_shader->bindUniformBlock("IFCRenderUBO", 1);

		m_chosen_shader->bindUniformBlock("TransformsUBO", 0);
		m_chosen_shader->bindUniformBlock("IFCRenderUBO", 1);

		m_collision_shader->bindUniformBlock("TransformMVPUBO", 2);

		m_comp_id_program->bindUniformBlock("TransformMVPUBO", 2);
		m_axis_shader->bindUniformBlock("TransformMVPUBO", 2);
		m_select_bbx_shader->bindUniformBlock("TransformMVPUBO", 2);
		m_edge_shader->bindUniformBlock("TransformMVPUBO", 2);
		m_clip_plane_shader->bindUniformBlock("TransformMVPUBO", 2);
		// ----- ----- ----- ----- ----- -----

		// -------------- render init --------------
		glLineWidth(1.5f);
		_defaultConfig();

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
			glClearColor(color.x, color.y, color.z, color.w); // 设置背景颜色
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除颜色、深度缓冲
			glClearDepthf(1.0);
			m_normal_depth_program->use();
			m_normal_depth_program->setMat4("mvp", m_projection * m_modelview);
			m_normal_depth_program->setMat3("t_inv_model", glm::transpose(glm::inverse(m_modelview)));
			break;
		}
		case COMP_ID_WRITE: {
			auto& color = m_depnor_value;
			glClearColor(color.x, color.y, color.z, color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除颜色、深度缓冲
			glClearDepthf(1.0);
			m_comp_id_program->use();
			m_comp_id_program->setMat4("mvp", m_projection * m_modelview);
			break;
		}
		case DEFAULT_SHADING: {
			auto& color = m_bg_color;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除颜色、深度缓冲
			m_test_shader->use();

			m_test_shader->setMat4("modelview", m_modelview);
			m_test_shader->setMat4("model", m_model);
			m_test_shader->setMat4("view", m_view);
			m_test_shader->setMat4("projection", m_projection);
			m_test_shader->setVec3("cameraDirection", m_camera_front);
			//m_test_shader->setMat4("view", m_camera->getViewMatrix());
			break;
		}
		case TRANSPARENCY_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// 清除颜色、深度缓冲
			glBlendEquation(GL_FUNC_ADD);

			m_test_shader->use();

			m_test_shader->setMat4("modelview", m_modelview);
			m_test_shader->setMat4("model", m_model);
			m_test_shader->setMat4("view", m_view);
			m_test_shader->setMat4("projection", m_projection);
			m_test_shader->setVec3("cameraDirection", m_camera_front);
			m_test_shader->setFloat("alpha", m_alpha);
			break;
		}
		default:break;

		}
		SharedPtr<GLVertexBuffer> vb = it->second;
		vb->draw();
		glDisable(GL_BLEND);
	}

	void GLRender::render(uint32_t render_id, RenderTypeEnum type, const uint32_t local_render_id = 1)
	{
		auto& vb_map = m_vertex_buffer_map;
		auto it = vb_map.find(render_id);
		if (it == vb_map.end()) {
			printf("The render_id[%u] is not existed.\n", render_id);
			return;
		}
		auto& transformUBO = *m_uniform_buffer_map.transformsUBO;
		auto& ifcRenderUBO = *m_uniform_buffer_map.ifcRenderUBO;
		auto& transformMVPUBO = *m_uniform_buffer_map.transformMVPUBO;
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
		case COMP_ID_WRITE: {
			auto& color = m_depnor_value;
			//glClearColor(color.x, color.y, color.z, color.w);
			glClearColor(-2, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearDepthf(1.0);
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformMVPUBO.update(64, 64, glm::value_ptr(m_init_model));
			transformMVPUBO.update(128, 16, glm::value_ptr(m_clip_plane));
			transformMVPUBO.update(144, 96, m_clip_box.data());
			m_comp_id_program->use();
			break;
		}
		case DEFAULT_SHADING: {
			auto& color = m_bg_color;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			transformUBO.update(0, 64, glm::value_ptr(m_modelview));
			transformUBO.update(64, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformUBO.update(128, 48, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(m_model)))));
			transformUBO.update(176, 16, glm::value_ptr(m_clip_plane));
			transformUBO.update(192, 64, glm::value_ptr(m_init_model));
			transformUBO.update(256, 96, m_clip_box.data());

			ifcRenderUBO.update(4, 4, &m_compId);
			ifcRenderUBO.update(8, 4, &m_hoverCompId);
			ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));

			m_test_shader->use();
			break;
		}
		case TRANSPARENCY_SHADING: {
			glEnable(GL_BLEND); //启用混合（可以使用透明物体）
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 按比例颜色混合
			glBlendEquation(GL_FUNC_ADD);//设置运算符 默认 源 和 target数值相加

			transformUBO.update(0, 64, glm::value_ptr(m_modelview));
			transformUBO.update(64, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformUBO.update(128, 48, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(m_model)))));
			transformUBO.update(176, 16, glm::value_ptr(m_clip_plane));
			transformUBO.update(192, 64, glm::value_ptr(m_init_model));
			transformUBO.update(256, 96, m_clip_box.data());

			ifcRenderUBO.update(0, 4, &m_alpha);
			ifcRenderUBO.update(4, 4, &m_compId);
			ifcRenderUBO.update(8, 4, &m_hoverCompId);
			ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));

			m_test_shader->use();
			break;
		}
		case BOUNDINGBOX_SHADING: {
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformMVPUBO.update(64, 64, glm::value_ptr(m_init_model));
			transformMVPUBO.update(128, 16, glm::value_ptr(m_clip_plane));
			transformMVPUBO.update(144, 96, m_clip_box.data());
			m_select_bbx_shader->use();
			//m_select_bbx_shader->setMat4("modelview", m_modelview);
			//m_select_bbx_shader->setMat4("projection", m_projection);
			break;
		}
		case EDGE_SHADING: {
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformMVPUBO.update(64, 64, glm::value_ptr(m_init_model));
			transformMVPUBO.update(128, 16, glm::value_ptr(m_clip_plane));
			transformMVPUBO.update(144, 96, m_clip_box.data());
			m_edge_shader->use();
			break;
		}
		case CHOSEN_SHADING: {
			transformUBO.update(0, 64, glm::value_ptr(m_modelview));
			transformUBO.update(64, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformUBO.update(128, 48, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(m_model)))));
			transformUBO.update(176, 16, glm::value_ptr(m_clip_plane));
			transformUBO.update(192, 64, glm::value_ptr(m_init_model));
			transformUBO.update(256, 96, m_clip_box.data());

			ifcRenderUBO.update(4, 4, &m_compId);
			ifcRenderUBO.update(8, 4, &m_hoverCompId);
			ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));

			m_chosen_shader->use();
			break;
		}
		case CHOSEN_TRANS_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			transformUBO.update(0, 64, glm::value_ptr(m_modelview));
			transformUBO.update(64, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformUBO.update(128, 48, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(m_model)))));
			transformUBO.update(176, 16, glm::value_ptr(m_clip_plane));
			transformUBO.update(192, 64, glm::value_ptr(m_init_model));
			transformUBO.update(256, 96, m_clip_box.data());

			ifcRenderUBO.update(4, 4, &m_compId);
			ifcRenderUBO.update(8, 4, &m_hoverCompId);
			ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));

			m_chosen_shader->use();
			break;
		}
		case COLLISION_RENDER: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
			transformMVPUBO.update(64, 64, glm::value_ptr(m_init_model));
			transformMVPUBO.update(128, 16, glm::value_ptr(m_clip_plane));
			transformMVPUBO.update(144, 96, m_clip_box.data());
			m_collision_shader->use();
			m_collision_shader->setVec2("u_resolution", glm::vec2(1600.f, 900.f));
			m_collision_shader->setFloat("alpha", 0.02f);
			break;
		}
		default:break;
		}
		SharedPtr<GLVertexBuffer> vb = it->second;

		vb->run_draw_func(local_render_id);
		//switch (local_render_id)
		//{
		//case ALL: {
		//	vb->draw();
		//	break;
		//}
		//case DYNAMIC_ALL: {
		//	vb->drawByDynamicEbo();
		//	break;
		//}
		//case NO_TRANS: {//draw no trans only
		//	vb->drawNoTrans();
		//	break;
		//}
		//case TRANS: {//draw trans only
		//	vb->drawTrans();
		//	break;
		//}
		//case BBX_LINE: {//draw bbx line
		//	vb->drawBBXLines();
		//	break;
		//}
		//case EDGE_LINE: {//draw edges
		//	vb->drawEdges();
		//	break;
		//}
		//case DYNAMIC_NO_TRANS: {//draw dynamic no trans
		//	vb->drawByDynamicEbo_no_trans();
		//	break;
		//}
		//case DYNAMIC_TRANS: {//draw dynamic trans
		//	vb->drawByDynamicEbo_trans();
		//	break;
		//}
		//case DYNAMIC_EDGE_LINE: {
		//	vb->drawByDynamicEdge();
		//	break;
		//}
		//case CHOSEN_NO_TRANS: {
		//	
		//	break;
		//}
		//case CHOSEN_TRANS: {
		//	break;
		//}
		//case COLLISION: {
		//	break;
		//}
		//default: {
		//	vb->drawByAddedEbo(local_render_id);
		//	break;
		//}
		//}
		glDisable(GL_BLEND);
	}

	void GLRender::renderClipBox(const bool hidden, ClipBox clip_box) {
		static uint32_t plane_vao, plane_vbo, plane_ebo;
		static Vector<uint32_t> cube_element_buffer_object = { 0,1,2,0,2,3,4,5,1,4,1,0,6,5,1,6,1,2,7,4,0,7,0,3,7,6,2,7,2,3,4,5,6,4,6,7 };
		static bool firstPlane = true;
		if (firstPlane) { // 传数据
			float k = .5f;
			float coord_plane[] = {
				-k,-k, -k,
				 k,-k, -k,
				 k,-k, k,
				-k,-k, k,
				-k, k, -k,
				 k, k, -k,
				 k, k, k,
				-k, k, k
			};
			glGenVertexArrays(1, &plane_vao);
			glGenBuffers(1, &plane_vbo);
			glGenBuffers(1, &plane_ebo);
			glBindVertexArray(plane_vao);
			glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_plane), &coord_plane, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer_object.size() * sizeof(uint32_t), cube_element_buffer_object.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			firstPlane = false;
		}

		if (!hidden) { // 渲染
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			auto& transformMVPUBO = *m_uniform_buffer_map.transformMVPUBO;
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * mirror_model * clip_box.toMat()));
			m_clip_plane_shader->use();
			glBindVertexArray(plane_vao);
			glDepthMask(GL_FALSE);
			glDrawElements(GL_TRIANGLES, cube_element_buffer_object.size(), GL_UNSIGNED_INT, 0);
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			_defaultConfig();
		}
	}

	void GLRender::renderText(IFCModel& ifc_model, glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height)
	{
		const wchar_t text[] = L"Ifc-Render-Engine-For-Tianha-Corp";
		const wchar_t text2[] = L"中文测试字样";
		m_text_shader->use();
		m_text_shader->setVec3("textColor", color);
		m_text_shader->setVec2("offset", glm::vec2(position));
		m_text_shader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast <float>(window_height))); // ortho正交投影
		glDisable(DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
		//textdata.render_text(text, glm::vec3(position), scale);
		texturefont.drawText(text);

		m_text_shader->setVec2("offset", glm::vec2(position) + glm::vec2(100.f, 100.f));
		texturefont.drawText(text2);
		/*static bool lockk = false;
		if (!lockk) {
			texturefont.init_bmp();
			lockk = true;
		}*/
		glDepthMask(GL_TRUE);
		_defaultConfig();
	}

	//void GLRender::renderAxis(IFCModel& ifc_model, const glm::vec3& pick_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos)
	//{
	//	static bool first = true;
	//	static uint32_t axis_vao;
	//	if (first) {
	//		float coord_axis[] = {
	//			0.0, 0.0, 0.0,
	//			1.0, 0.0, 0.0,	// x-axis
	//			0.0, 0.0, 0.0,
	//			0.0, 1.0, 0.0,	// y-axis
	//			0.0, 0.0, 0.0,
	//			0.0, 0.0, 1.0	// z-axis
	//		};
	//		uint32_t axis_vbo;
	//		glGenVertexArrays(1, &axis_vao);
	//		glGenBuffers(1, &axis_vbo);
	//		glBindVertexArray(axis_vao);
	//		glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);
	//		glBufferData(GL_ARRAY_BUFFER, sizeof(coord_axis), &coord_axis, GL_STATIC_DRAW);
	//		glEnableVertexAttribArray(0);
	//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//		first = false;
	//	}
	//	glm::vec3 model_center = ifc_model.getModelCenter();
	//	glm::mat4 model = ifc_model.getModelMatrix();
	//	float scale_factor = ifc_model.getScaleFactor();

	//	//model[3][0] = model[3][1] = model[3][2] = 0;
	//	glm::mat4 trans_center(1.0f);
	//	glm::mat4 trans_click_center(1.0f);
	//	trans_center = glm::translate(trans_center, model_center);
	//	model = model * trans_center;
	//	glm::vec3 world_pos(model[3][0], model[3][1], model[3][2]);

	//	float len_ref = glm::length(init_view_pos);
	//	float len = glm::length(view_pos - pick_center);
	//	//printf("%f\n", scale_factor);
	//	float scale = len / len_ref / scale_factor * 0.25f;
	//	model = glm::scale(model, glm::vec3(scale, scale, scale));

	//	trans_click_center = glm::translate(trans_click_center, pick_center - world_pos);
	//	model = trans_click_center * model;

	//	auto& transformMVPUBO = *m_uniform_buffer_map.transformMVPUBO;
	//	transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * model));
	//	m_axis_shader->use();
	//	glBindVertexArray(axis_vao);
	//	glDisable(DEPTH_TEST);
	//	glDepthFunc(GL_ALWAYS);
	//	glDrawArrays(GL_LINES, 0, 6);
	//	_defaultConfig();

	//}

	void GLRender::renderAxis(IFCModel& ifc_model, const glm::vec3& pick_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos)
	{
		static bool first = true;
		static uint32_t axis_vao;
		if (first) { // 初始化 (一般只运行一次。除非物体频繁改变)
			float coord_axis[] = {
				0.0, 0.0, 0.0,
				100.0, 0.0, 0.0,	// x-axis
				0.0, 0.0, 0.0,
				0.0, 100.0, 0.0,	// y-axis
				0.0, 0.0, 0.0,
				0.0, 0.0, 100.0		// z-axis
			};
			uint32_t axis_vbo;
			glGenVertexArrays(1, &axis_vao);
			glGenBuffers(1, &axis_vbo); // 创建一个缓冲
			glBindVertexArray(axis_vao); // 绑定VAO
			glBindBuffer(GL_ARRAY_BUFFER, axis_vbo); // 设置缓冲类型
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_axis), &coord_axis, GL_STATIC_DRAW); // 把用户定义的数据复制到当前绑定缓冲(显存)
			glEnableVertexAttribArray(0); // 以顶点属性位置值作为参数，启用顶点属性
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上） 从此时绑定到GL_ARRAY_BUFFER的VBO获取数据
			first = false;
		}
		// 绘制物体（渲染循环）
		glm::vec3 model_center = ifc_model.getModelCenter();
		glm::mat4 model = ifc_model.getModelMatrix();
		float scale_factor = ifc_model.getScaleFactor();

		//model[3][0] = model[3][1] = model[3][2] = 0;
		glm::mat4 trans_center(1.0f);
		glm::mat4 trans_click_center(1.0f);
		trans_center = glm::translate(trans_center, model_center);
		model = model * trans_center;
		glm::vec3 world_pos(model[3][0], model[3][1], model[3][2]);

		float len_ref = glm::length(init_view_pos);
		glm::vec3 center = glm::vec3(0, 0, 0);
		//glm::vec3 center = ifc_model.getModelCenter();
		float len = glm::length(view_pos - center);
		//printf("%f\n", scale_factor);
		float scale = len / len_ref / scale_factor * 0.25f;
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		trans_click_center = glm::translate(trans_click_center, center - world_pos);
		model = trans_click_center * model;

		auto& transformMVPUBO = *m_uniform_buffer_map.transformMVPUBO;
		transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * model));
		m_axis_shader->use();
		glBindVertexArray(axis_vao); // 使用上面那一套VAO
		//glDisable(DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glDepthFunc(GL_ALWAYS);
		glDrawArrays(GL_LINES, 0, 6); // 使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元
		_defaultConfig();

	}

	void GLRender::postRender(uint32_t col_tex_id, uint32_t depth_normal_tex_id)
	{
		static bool first = true;
		static uint32_t off_vao;
		if (first) {
			float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords	// 位置 纹理坐标
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
		static unsigned int intermediateFBO;
		static unsigned int screenTexture;

		if (!first) {
			float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords	// 位置 纹理坐标
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
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);						// 读入顶点数据
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));		//读入纹理数据

			first = true;
		}

		glActiveTexture(GL_TEXTURE0); // texture 0 // 在绑定之前激活相应的纹理单元
		glBindTexture(GL_TEXTURE_2D, w.getColorTexId());// 获取颜色纹理序号 并绑定
		if (w.getDepthNormalTexId() != -1) {
			glActiveTexture(GL_TEXTURE1); // texture 1 // 在绑定之前激活相应的纹理单元
			glBindTexture(GL_TEXTURE_2D, w.getDepthNormalTexId());// 获取深度纹理序号 并绑定
		}
		m_offscreen_program->use(); // 激活着色器程序 //进行渲染
		m_offscreen_program->setInt("screenTexture", 0);
		m_offscreen_program->setInt("depthNormalTexture", 1);

		glm::vec2 win_size = w.getWindowSize(); //获取存储屏幕大小
		glm::vec2 win_texel_size = glm::vec2(1.0 / win_size.x, 1.0 / win_size.y); //获取纹理到屏幕缩放比例
		m_offscreen_program->setVec2("screenTexTexelSize", win_texel_size);//以uniform形式传给vertex shader

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(off_vao);
		glClear(GL_COLOR_BUFFER_BIT); // 清空颜色缓冲
		glDrawArrays(GL_TRIANGLES, 0, 6); // 渲染两个三角形
		_defaultConfig();
	}

	void GLRender::_defaultConfig()
	{
		glBindVertexArray(0);		// 当我们打算绘制物体的时候就拿出相应的VAO，绑定它，绘制完物体后，再解绑VAO
		glEnable(GL_DEPTH_TEST);	// 启用深度测试
		glDepthFunc(GL_LESS);		// 丢弃深度值大于等于当前深度缓冲值的所有片段 （默认设置，画家）
		glEnable(GL_MULTISAMPLE);	// 启用多重采样
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
			glDisable(GL_CULL_FACE);  //启用面剔除 
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

	void GLRender::enableBlend() {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	void GLRender::disableBlend() {
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}

	void GLRender::enableMultiSample() {
		glEnable(GL_MULTISAMPLE);
	}
	void GLRender::disableMultiSample() {
		glDisable(GL_MULTISAMPLE);
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

	void GLRender::ModelVertexUpdate(uint32_t render_id, const Vector<Real>& vertices) {
		m_vertex_buffer_map[render_id]->updateVertexAttributes(vertices);
	}

	void GLRender::DynamicUpdate(uint32_t render_id, const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& no_trans_indices, const Vector<uint32_t>& trans_indices, const Vector<uint32_t>& edge_indices) {
		m_vertex_buffer_map[render_id]->uploadDynamicElementBuffer(dynamic_all_ebo, no_trans_indices, trans_indices, edge_indices);
	}

	void GLRender::ChosenGeomUpdate(uint32_t render_id, const Vector<uint32_t>& chosen_no_trans_ebo, const Vector<uint32_t>& chosen_trans_ebo) {
		m_vertex_buffer_map[render_id]->uploadChosenElementBuffer(chosen_no_trans_ebo, chosen_trans_ebo);
	}

	void GLRender::setViewMatrix(const glm::mat4& view) {
		m_view = view;
	}

	void GLRender::setModelMatrix(const glm::mat4& model) {
		m_model = model;
	}

	void GLRender::setInitModelMatrix(const glm::mat4& init_model) {
		m_init_model = init_model;
	}

	void GLRender::setMirrorModelMatrix(const glm::mat4& mirror) {
		mirror_model = mirror;
	}

	void GLRender::setModelViewMatrix(const glm::mat4& mv)
	{
		m_modelview = mv;
	}
	void GLRender::setProjectionMatrix(const glm::mat4& projection)
	{
		m_projection = projection;
	}
	void GLRender::setAlpha(const float& alpha) {
		m_alpha = alpha;
	}
	void GLRender::setCameraDirection(const glm::vec3& m_front) {
		m_camera_front = m_front;
	}
	void GLRender::setCompId(const int& comp_id)
	{
		m_compId = comp_id;
	}
	void GLRender::setHoverCompId(const int& comp_id)
	{
		m_hoverCompId = comp_id;
	}

	void GLRender::setClippingPlane(const glm::vec4& clip_plane) {
		m_clip_plane = clip_plane;
	}

	void GLRender::setClippingBox(const Vector<glm::vec4>& clip_box) {
		m_clip_box = clip_box;
	}
	glm::vec4 GLRender::get_test_matrix(const glm::vec4& a) const {
		return m_projection * m_modelview * a;
	}

	glm::vec3 GLRender::get_pixel_pos_in_screen(const glm::vec4& model_pos, const int& window_width, const int& window_height) const {
		glm::vec4 sxaswd1 = get_test_matrix(model_pos);
		sxaswd1 /= sxaswd1.w;
		sxaswd1.x = (1. + sxaswd1.x) / 2 * window_width;
		sxaswd1.y = (1. + sxaswd1.y) / 2 * window_height;
		return glm::vec3(sxaswd1);
	}
} 