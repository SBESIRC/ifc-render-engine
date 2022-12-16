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
		m_uniform_buffer_map.transformsUBO = make_shared<GLUniformBuffer>(sizeof(glm::mat4) * 3 + sizeof(glm::vec4) * 8 + 4);
		m_uniform_buffer_map.ifcRenderUBO = make_shared<GLUniformBuffer>(48);
		//m_uniform_buffer_map.transformMVPUBO = make_shared<GLUniformBuffer>(sizeof(glm::mat4) * 2 + sizeof(glm::vec4) * 8 + 4);
		m_uniform_buffer_map.StoreyOffsetTransformUBO = make_shared<GLUniformBuffer>(sizeof(glm::mat4) * 100 + sizeof(int) * 100);

		m_uniform_buffer_map.transformsUBO->bindRange(0);
		m_uniform_buffer_map.ifcRenderUBO->bindRange(1);
		//m_uniform_buffer_map.transformMVPUBO->bindRange(2);
		m_uniform_buffer_map.StoreyOffsetTransformUBO->bindRange(3);

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

		String v_clp_ui_plane = util::read_file("shaders/clp_ui_plane.vert");
		String f_clp_ui_plane = util::read_file("shaders/clp_ui_plane.frag");
		m_clip_plane_UI_shader = make_unique<GLSLProgram>(v_clp_ui_plane.c_str(), f_clp_ui_plane.c_str());

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

		String v_gizmo = util::read_file("shaders/gizmo.vert");
		String f_gizmo = util::read_file("shaders/gizmo.frag");
		m_gizmo_shader = make_unique<GLSLProgram>(v_gizmo.c_str(), f_gizmo.c_str());

		String v_gizmo_ui = util::read_file("shaders/gizmo_ui.vert");
		String f_gizmo_ui = util::read_file("shaders/gizmo_ui.frag");
		m_gizmo_UI_shader = make_unique<GLSLProgram>(v_gizmo_ui.c_str(), f_gizmo_ui.c_str());

		String v_text = util::read_file("shaders/text.vert");
		String f_text = util::read_file("shaders/text.frag");
		m_text_shader = make_unique<GLSLProgram>(v_text.c_str(), f_text.c_str());

		String v_grid = util::read_file("shaders/grid.vert");
		String f_grid = util::read_file("shaders/grid.frag");
		m_grid_shader = make_unique<GLSLProgram>(v_grid.c_str(), f_grid.c_str());

		String v_text3d = util::read_file("shaders/text3d.vert");
		m_text3d_shader = make_unique<GLSLProgram>(v_text3d.c_str(), f_text.c_str());

		// ------------- drawing match shader test ------------
		String v_drawing = util::read_file("shaders/drawing_match.vert");
		String f_drawing = util::read_file("shaders/drawing_match.frag");
		m_drawing_match_shader = make_unique<GLSLProgram>(v_drawing.c_str(), f_drawing.c_str());

		// ------------- tile-view's drawing ------------
		String v_tile_drawing = util::read_file("shaders/tile_view_drawing.vert");
		String f_tile_drawing = util::read_file("shaders/tile_view_drawing.frag");
		m_tile_view_drawing_shader = make_unique<GLSLProgram>(v_tile_drawing.c_str(), f_tile_drawing.c_str());

		String v_skybox = util::read_file("shaders/skybox.vert");
		String f_skybox = util::read_file("shaders/skybox.frag");
		m_skybox_shader = make_unique<GLSLProgram>(v_skybox.c_str(), f_skybox.c_str());
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
		m_clip_plane_UI_shader = make_unique<GLSLProgram>(sc::v_clp_ui_plane, sc::f_clp_ui_plane);
		m_collision_shader = make_unique<GLSLProgram>(sc::v_collision, sc::f_collision);
		m_gizmo_shader = make_unique<GLSLProgram>(sc::v_gizmo, sc::f_gizmo);
		m_gizmo_UI_shader = make_unique<GLSLProgram>(sc::v_gizmo_ui, sc::f_gizmo_ui);
		m_text_shader = make_unique<GLSLProgram>(sc::v_text, sc::f_text);
		m_grid_shader = make_unique<GLSLProgram>(sc::v_grid, sc::f_grid);
		m_text3d_shader = make_unique<GLSLProgram>(sc::v_text3d, sc::f_text);
		m_drawing_match_shader = make_unique<GLSLProgram>(sc::v_drawing_match, sc::f_drawing_match);
		m_skybox_shader = make_unique<GLSLProgram>(sc::v_skybox, sc::f_skybox);
		m_tile_view_drawing_shader = make_unique<GLSLProgram>(sc::v_tile_view_drawing, sc::f_tile_view_drawing);
#endif

		m_test_shader->bindUniformBlock("TransformsUBO", 0);
		m_test_shader->bindUniformBlock("IFCRenderUBO", 1);
		m_test_shader->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_chosen_shader->bindUniformBlock("TransformsUBO", 0);
		m_chosen_shader->bindUniformBlock("IFCRenderUBO", 1);
		m_chosen_shader->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_collision_shader->bindUniformBlock("TransformsUBO", 0);
		m_collision_shader->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_edge_shader->bindUniformBlock("TransformsUBO", 0);
		m_edge_shader->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_comp_id_program->bindUniformBlock("TransformsUBO", 0);
		m_comp_id_program->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_select_bbx_shader->bindUniformBlock("TransformsUBO", 0);
		//m_select_bbx_shader->bindUniformBlock("StoreyOffsetTransformUBO", 3);

		m_axis_shader->bindUniformBlock("TransformsUBO", 0);

		m_clip_plane_shader->bindUniformBlock("TransformsUBO", 0);
		m_clip_plane_UI_shader->bindUniformBlock("TransformsUBO", 0);

		m_gizmo_shader->bindUniformBlock("TransformsUBO", 0);
		m_gizmo_UI_shader->bindUniformBlock("TransformsUBO", 0);

		//m_skybox_shader->bindUniformBlock("TransformsUBO", 2);
		m_grid_shader->bindUniformBlock("TransformsUBO", 2);

		m_drawing_match_shader->bindUniformBlock("TransformsUBO", 0);		// ------------- drawing match shader test ------------
		m_tile_view_drawing_shader->bindUniformBlock("TransformsUBO", 0);	// ------------- tile-view's drawing ------------
		// ----- ----- ----- ----- ----- -----

		// -------------- render init --------------
		glLineWidth(1.5f);
		setToDefaultConfig();

		//quad for post processing
		//vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		Vector<float> quadVertices = {
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
		glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		// ----- ----- ----- ----- ----- ----- -----

		use_clip_box->glInit(ui_id_num);
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
			m_comp_id_program->use();
			break;
		}
		case DEFAULT_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			m_test_shader->use();
			m_test_shader->setFloat("shading_factor", 1.f);
			break;
		}
		case OFFLINE_SHADING: {
			auto& color = m_bg_color_off;
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_test_shader->use();
			m_test_shader->setFloat("shading_factor", 1.f);
			break;
		}
		case TRANSPARENCY_SHADING: {
			glEnable(GL_BLEND); //启用混合（可以使用透明物体）
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 按比例颜色混合
			glBlendEquation(GL_FUNC_ADD);//设置运算符 默认 源 和 target数值相加

			//transformUBO.update(0, 64, glm::value_ptr(m_modelview));
			//transformUBO.update(64, 64, glm::value_ptr(m_projection * m_view * m_model));
			//transformUBO.update(128, 48, glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(m_model)))));
			//transformUBO.update(176, 16, glm::value_ptr(m_clip_plane));
			//transformUBO.update(192, 64, glm::value_ptr(m_init_model));
			//transformUBO.update(256, 96, m_clip_box.data());
			//transformUBO.update(352, 16, glm::value_ptr(m_drawing_match_plane));
			//transformUBO.update(368, 4, &m_TileView);

			////ifcRenderUBO.update(0, 4, &m_alpha);
			//ifcRenderUBO.update(4, 4, &m_compId);
			//ifcRenderUBO.update(8, 4, &m_hoverCompId);
			//ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));

			m_test_shader->use();
			m_test_shader->setFloat("shading_factor", 1.f);
			break;
		}
		case BOUNDINGBOX_SHADING: {
			m_select_bbx_shader->use();
			m_select_bbx_shader->setMat4("storeyOffset_mat", storeyOffset_mat);
			break;
		}
		case EDGE_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			m_test_shader->use();
			m_test_shader->setFloat("shading_factor", .7f);
			break;
		}
		case CHOSEN_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			m_chosen_shader->use();
			break;
		}
		case CHOSEN_TRANS_SHADING: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			m_chosen_shader->use();
			break;
		}
		case COLLISION_RENDER: {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			m_collision_shader->use();
			m_collision_shader->setVec2("u_resolution", glm::vec2(1600.f, 900.f));
			m_collision_shader->setFloat("alpha", 0.02f);
			break;
		}
		default:break;
		}
		SharedPtr<GLVertexBuffer> vb = it->second;

		vb->run_draw_func(local_render_id);
		glDisable(GL_BLEND);
	}

	void GLRender::renderClipBox(const bool hidden) {
		if (!hidden) {
			auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model * use_clip_box->toMat()));
			m_clip_plane_shader->use();
			m_clip_plane_shader->setInt("ui_id", last_clp_face_key);
			//std::cout << clp_face_id << std::endl;
			m_clip_plane_shader->setVec3("this_color", glm::vec3(0.f, 1.f, 1.f));
			use_clip_box->drawBox(true);
			setToDefaultConfig();
		}
	}

	void GLRender::renderClipBox() {
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model * use_clip_box->toMat()));
		m_clip_plane_shader->use();
		m_clip_plane_shader->setInt("ui_id", -1);
		m_clip_plane_shader->setVec3("this_color", glm::vec3(1.f, 0.f, 0.f));
		glDisable(DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_FALSE);
		glLineWidth(7.f);
		use_clip_box->drawBox(false);
		glDepthMask(GL_TRUE);
		glLineWidth(1.5f);
		setToDefaultConfig();
	}

	void GLRender::renderClipBoxInUIlayer(const bool hidden) {
		if (!hidden) {
			auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model * use_clip_box->toMat()));
			m_clip_plane_UI_shader->use();
			use_clip_box->drawBoxInUILayer();
		}
	}

	void GLRender::renderSkyBox(const glm::mat4& porjmat) {
		auto& color = m_bg_color;
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_skybox_shader->use();
		m_skybox_shader->setMat4("view_matrix", porjmat * glm::mat4(glm::mat3(m_view)));
		skyBox->drawSkyBox();
		setToDefaultConfig();
	}

	void GLRender::renderText(glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height)
	{
		const wchar_t text[] = L"Ifc-Render-Engine-For-Tianha-Corp";
		const wchar_t text2[] = L"中文测试字样";
		m_text_shader->use();
		m_text_shader->setVec3("textColor", color);
		m_text_shader->setVec2("offset", glm::vec2(position));
		m_text_shader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast <float>(window_height))); // ortho正交投影
		glDisable(DEPTH_TEST);
		glDepthFunc(GL_ALWAYS); // always pass depth test
		glDepthMask(GL_FALSE); // forbit import from depth test
		//textdata.render_text(text, glm::vec3(position), scale);
		//texturefont.drawText(text, 1.f);

		m_text_shader->setVec2("offset", glm::vec2(position) + glm::vec2(100.f, 100.f));
		//texturefont.drawText(text2, .5f);
		/*static bool lockk = false;
		if (!lockk) {
			texturefont.init_bmp();
			lockk = true;
		}*/
		glDepthMask(GL_TRUE);

		setToDefaultConfig();
	}
	void GLRender::renderText(const std::string& text, const glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height)
	{
		m_text_shader->use();
		m_text_shader->setVec3("textColor", color);
		m_text_shader->setVec2("offset", glm::vec2(0));
		m_text_shader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(window_width), 0.0f, static_cast <float>(window_height))); // ortho正交投影
		glDisable(DEPTH_TEST);
		glDepthFunc(GL_ALWAYS); // always pass depth test
		glDepthMask(GL_FALSE); // forbit import from depth test
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		textdata.render_text(text, glm::vec3(position), scale);
		
		glDepthMask(GL_TRUE);
		setToDefaultConfig();
	}


	void GLRender::renderGridText(Vector<std::wstring>& texts, Vector<float>& text_data, bool& text_reset) {
		//texturefont.drawText3Ds(m_text3d_shader, texts, text_data, m_projection, m_modelview);
		texturefont.drawText3D(m_text3d_shader, texts, text_data, m_projection, m_modelview, text_reset);
		setToDefaultConfig();
	}

	void GLRender::renderGridLine(Vector<float>& grid_line , int width, int height, bool& grid_line_reset)
	{
		static uint32_t grid_vao;
		static uint32_t grid_line_size = 0;

		if (grid_line_reset) {
			grid_line_reset = false;
			grid_line_size = grid_line.size();
			Vector<float> tmp(grid_line_size * 14 / 12);
			for (int i = 0, j = 0; i < grid_line_size; i += 12,j += 14) {
				tmp[j] = grid_line[i];
				tmp[j + 1] = grid_line[i + 1];
				tmp[j + 2] = grid_line[i + 2];
					 
				tmp[j + 3] = grid_line[i + 6];
				tmp[j + 4] = grid_line[i + 7];
				tmp[j + 5] = grid_line[i + 8];
					 
				tmp[j + 6] = grid_line[i + 11];
					 
				tmp[j + 7] = grid_line[i + 3];
				tmp[j + 8] = grid_line[i + 4];
				tmp[j + 9] = grid_line[i + 5];
					 
				tmp[j + 10] = grid_line[i + 6];
				tmp[j + 11] = grid_line[i + 7];
				tmp[j + 12] = grid_line[i + 8];
					 
				tmp[j + 13] = grid_line[i + 11];
			}
			uint32_t grid_vbo;
			glGenVertexArrays(1, &grid_vao);
			glGenBuffers(1, &grid_vbo);
			glBindVertexArray(grid_vao);
			glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
			glBufferData(GL_ARRAY_BUFFER, tmp.size() * sizeof(float), tmp.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			Vector<float>().swap(grid_line);
		}
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
		m_grid_shader->use();
		m_grid_shader->setVec2("u_resolution", glm::vec2(width, height));
		glBindVertexArray(grid_vao);
		glDrawArrays(GL_LINES, 0, grid_line_size / 3);
		setToDefaultConfig();
	}

	void GLRender::renderAxis(IFCModel& ifc_model, const glm::vec3& pick_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos)
	{
		//trans_click_center = glm::translate(trans_click_center, pick_center - world_pos);
		//model = trans_click_center * model;
		myaxis.update_model_mat_info(ifc_model.getModelMatrix(), ifc_model.curcenter, ifc_model.getScaleFactor(),
			pick_center, view_pos, init_view_pos);
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * myaxis.axis_model));
		m_axis_shader->use();
		myaxis.drawAxis();
		setToDefaultConfig();
	}
	void GLRender::renderViewCube(const glm::mat4& rotate_matrix, const glm::vec2 window_size)
	{
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		glm::mat4 tempmatrix = gizmo.private_transform(window_size) * rotate_matrix;
		transformMVPUBO.update(0, 64, glm::value_ptr(tempmatrix));
		m_gizmo_shader->use();
		m_gizmo_shader->setInt("hover_id", last_hovered_face_key);
		gizmo.drawGizmo();
		setToDefaultConfig();
	}

	void GLRender::renderGizmoInUIlayer(const glm::mat4& rotate_matrix, const glm::vec2 window_size) {
		glClearColor(-1, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepthf(1.0);
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		glm::mat4 tempmatrix = gizmo.private_transform(window_size) * rotate_matrix;
		transformMVPUBO.update(0, 64, glm::value_ptr(tempmatrix));
		m_gizmo_UI_shader->use();
		gizmo.drawGizmoInUiLayer();
	}
	
	unsigned int  GLRender::loadCubemap(Vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	// ------------- drawing match shading -------------
	void GLRender::renderDrawing(IFCModel& ifc_model, float k) {
		static bool first = true;
		static uint32_t drawing_vao;
		static uint32_t drawingTex;
		static float drawing_width, drawing_height;
		float ratio = 0.01;			// based on the ratio of drawing.  //e.g. 1:100
		if (first)
		{
			float quadVertices[] = {
				// positions		// texCoords
				-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
				-1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
				 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,

				-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
				 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
				 1.0f, 0.0f, -1.0f,  1.0f, 1.0f
			};
			uint32_t drawing_vbo;
			glGenVertexArrays(1, &drawing_vao);
			glGenBuffers(1, &drawing_vbo);
			glBindVertexArray(drawing_vao);
			glBindBuffer(GL_ARRAY_BUFFER, drawing_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

			glGenTextures(1, &drawingTex);
			glBindTexture(GL_TEXTURE_2D, drawingTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			unsigned char* data = stbi_load("resources\\textures\\scrpit.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				drawing_width = width * ratio;
				drawing_height = height * ratio;
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

			glBindTexture(GL_TEXTURE_2D, 0);

			first = false;
		}
		glm::mat4 scale(1.0f);
		glm::mat4 y_translate(1.0f);
		y_translate = glm::translate(y_translate, glm::vec3(m_drawing_match_plane));
		scale = glm::scale(scale, glm::vec3(drawing_width * k, 1.0, drawing_height * k));
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model * y_translate * scale));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_drawing_match_shader->use();
		m_drawing_match_shader->setInt("Drawing", 0);

		glBindVertexArray(drawing_vao);
		glBindTexture(GL_TEXTURE_2D, drawingTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// ------------- tile-view's drawing -------------
	void GLRender::renderTileViewDrawing(IFCModel& ifc_model) {
		static uint32_t floorNum;		// the count of floor
		static bool first = true;
		static uint32_t tile_vao;
		static uint32_t tileTex[100];
		static float tex_width[100], tex_height[100];
		float ratio = 0.01;			// based on the ratio of drawing.  //e.g. 1:100
		if (first)
		{
			floorNum = ifc_model.tile_matrix.size();
			float quadVertices[] = {
				// positions		// texCoords
				-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
				-1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
				 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,

				-1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
				 1.0f, 0.0f,  1.0f,  1.0f, 0.0f,
				 1.0f, 0.0f, -1.0f,  1.0f, 1.0f
			};
			uint32_t drawing_vbo;
			glGenVertexArrays(1, &tile_vao);
			glGenBuffers(1, &drawing_vbo);
			glBindVertexArray(tile_vao);
			glBindBuffer(GL_ARRAY_BUFFER, drawing_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

			glGenTextures(100, tileTex);
			for (int i = 0; i < floorNum; i++) {
				glBindTexture(GL_TEXTURE_2D, tileTex[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				int width, height, nrChannels;
				unsigned char* data = stbi_load("resources\\textures\\scrpit.png", &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
					tex_width[i] = width * ratio;
					tex_height[i] = height * ratio;
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			first = false;
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBindVertexArray(tile_vao);
		auto& transformMVPUBO = *m_uniform_buffer_map.transformsUBO;
		m_tile_view_drawing_shader->use();
		m_tile_view_drawing_shader->setInt("Drawing", 0);

		for (int i = 0; i < floorNum; i++) {
			glm::mat4 scale(1.0f);
			glm::mat4 y_translate(1.0f);
			float k = 2.f;
			float ratio = 1.f;
			scale = glm::scale(scale, glm::vec3(tex_width[i] * k, 1.0, tex_height[i] * k * ratio));
			y_translate = glm::translate(y_translate, glm::vec3(m_tile_view_lowest_y_plane[i]));	// translate to the current floor's minY
			transformMVPUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model * ifc_model.tile_matrix[i] * y_translate * scale));
			glBindTexture(GL_TEXTURE_2D, tileTex[i]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLRender::AerialViewRender(RenderWindow& w) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, w.getAerialColorTexId());
		if (w.getDepthNormalTexId() != -1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, w.getDepthNormalTexId());
		}
		m_offscreen_program->use();
		m_offscreen_program->setInt("screenTexture", 0);
		m_offscreen_program->setInt("depthNormalTexture", 1);
		m_offscreen_program->setFloat("scale", .25f);
		m_offscreen_program->setInt("this_flag", 0);
		//m_offscreen_program->setFloat("mod", 1.f);

		glm::vec2 win_size = w.getWindowSize();
		glm::vec2 win_texel_size = glm::vec2(1.0 / win_size.x, 1.0 / win_size.y);
		m_offscreen_program->setVec2("screenTexTexelSize", win_texel_size);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(off_vao);
		//glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		setToDefaultConfig();
	}

	void GLRender::postRender(RenderWindow& w)
	{
		glActiveTexture(GL_TEXTURE0); // texture 0 // 在绑定之前激活相应的纹理单元
		glBindTexture(GL_TEXTURE_2D, w.getColorTexId());// 获取颜色纹理序号 并绑定
		if (w.getDepthNormalTexId() != -1) {
			glActiveTexture(GL_TEXTURE1); // texture 1 // 在绑定之前激活相应的纹理单元
			glBindTexture(GL_TEXTURE_2D, w.getDepthNormalTexId());// 获取深度纹理序号 并绑定
		}
		m_offscreen_program->use(); // 激活着色器程序 //进行渲染
		m_offscreen_program->setInt("screenTexture", 0);
		m_offscreen_program->setInt("depthNormalTexture", 1);
		m_offscreen_program->setFloat("scale", 1.f);
		m_offscreen_program->setInt("this_flag", 1);

		glm::vec2 win_size = w.getWindowSize(); //获取存储屏幕大小
		glm::vec2 win_texel_size = glm::vec2(1.0 / win_size.x, 1.0 / win_size.y); //获取纹理到屏幕缩放比例
		m_offscreen_program->setVec2("screenTexTexelSize", win_texel_size);//以uniform形式传给vertex shader

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(off_vao);
		glClear(GL_COLOR_BUFFER_BIT); // 清空颜色缓冲
		glDrawArrays(GL_TRIANGLES, 0, 6); // 渲染两个三角形
		setToDefaultConfig();
	}

	void GLRender::setToDefaultConfig()
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

	void GLRender::reset_render() {

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

	uint32_t GLRender::addModel(SharedPtr<GLVertexBuffer> vertex_buffer, int id)
	{
		//uint32_t id = 1;// util::get_next_globalid();
		//m_vertex_buffer_map.clear();
		m_vertex_buffer_map.insert(make_pair(id, vertex_buffer));
		return id;
	}
	void GLRender::deleteModel(uint32_t id) {
		m_vertex_buffer_map.erase(id);
	}
	void GLRender::clear_model() {
		m_vertex_buffer_map.clear();
		std::cout << "Render clear.\n";
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

	void GLRender::CollisionGeomUpdate(uint32_t render_id, const Vector<uint32_t>& collid_ebo) {
		m_vertex_buffer_map[render_id]->uploadCollisionElementBuffer(collid_ebo);
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
	void GLRender::setCameraPos(const glm::vec3& m_pos) {
		m_camera_pos = m_pos;
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

	void GLRender::setClippingBox(const bool hidden) {
		m_clip_box = getClippingBoxVectors(hidden);
	}

	void GLRender::updateOpenDrawingMatch(bool _flag) {
		// TODO open Drawing match
		if (_flag)
			m_drawing_match_plane = drawing_plane.to_vec4();
		else
			m_drawing_match_plane = hidden_drawing_plane;
	}

	void GLRender::TileView(bool show)
	{
		if (show) {
			m_TileView = 1;
		}
		else {
			m_TileView = 0;
		}
	}
	void GLRender::TileViewMatrix(SharedPtr<IFCModel> ifcModel) {

		for (int i = 0; i < ifcModel->bbxs_each_floor.size(); i++) {
			glm::mat4 model(1.0f);
			glm::vec3 bbx_center = ifcModel->get_bbx_center(ifcModel->bbxs_each_floor[i]);
			model = glm::translate(model, bbx_center);
			float delta_y = bbx_center[1] - ifcModel->bbxs_each_floor[i][1];
			model = glm::translate(model, glm::vec3(0., -delta_y, 0.));
			m_tile_view_lowest_y_plane.push_back(model * glm::vec4(0.f, 0.f, 0.f, 1.f));
		}
	}

	void GLRender::setStoreyMat(glm::mat4 matrix) {
		storeyOffset_mat = matrix;
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
	void GLRender::upload_mat4s_to_gpu(const Vector<glm::mat4>& offsets_mats)
	{
		m_uniform_buffer_map.StoreyOffsetTransformUBO->update(0, 6400, offsets_mats.data());			// matrix
		//m_uniform_buffer_map.StoreyOffsetTransformUBO->update(6400, 400, floorIndex.data());			// real floor index to sort floor
	}

	void GLRender::ubo_datasa_updates() {

		auto& transformUBO = *m_uniform_buffer_map.transformsUBO;
		auto& ifcRenderUBO = *m_uniform_buffer_map.ifcRenderUBO;

		transformUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
		transformUBO.update(64, 64, glm::value_ptr(m_init_model));
		transformUBO.update(128, 16, glm::value_ptr(m_clip_plane));
		transformUBO.update(144, 96, m_clip_box.data());
		transformUBO.update(240, 16, glm::value_ptr(m_drawing_match_plane));
		transformUBO.update(256, 4, &m_TileView);

		ifcRenderUBO.update(0, 4, &m_hoverCompId);
		ifcRenderUBO.update(16, 12, glm::value_ptr(m_camera_front));
		ifcRenderUBO.update(32, 12, glm::value_ptr(m_camera_pos));
	}

	void GLRender::transformUBO_refresh() {
		auto& transformUBO = *m_uniform_buffer_map.transformsUBO;
		transformUBO.update(0, 64, glm::value_ptr(m_projection * m_view * m_model));
	}
} 
