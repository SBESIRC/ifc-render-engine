#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"
#include "ifcrender/render_ui.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>

//#define ONLY_DEPTH_NROMAL_RES
#define TEST_COMP_ID_RES

namespace ifcre {
	extern IFCVulkanRender;
	SharedPtr<IFCRenderEngine> ifcre;

	void IFCRenderEngine::setConfig(String key, String value)
	{
		m_cache_configs[key] = value;
	}
	void IFCRenderEngine::clear_model_data() {
		_g_indices.clear();
		_g_indices.reserve(1024);
		_g_vertices.clear();
		_g_vertices.reserve(1024);
		_g_normals.clear();
		_g_normals.reserve(1024);
		_c_indices.clear();
		_c_indices.reserve(1024);
		_face_mat.clear();
		_face_mat.reserve(1024);
		_edge_indices.clear();
		_edge_indices.reserve(1024);
	}
	void IFCRenderEngine::set_g_indices(int val) {
		_g_indices.emplace_back(val);
	}
	void IFCRenderEngine::set_g_vertices(float val) {
		_g_vertices.emplace_back(val);
	}
	void IFCRenderEngine::set_g_normals(float val) {
		_g_normals.emplace_back(val);
	}
	void IFCRenderEngine::set_c_indices(int val) {
		if (val == -1) {
			if (_tmp_c_indices.size() > 0) {
				_c_indices.emplace_back(_tmp_c_indices);
				//_g_indices.insert(_g_indices.end(), _tmp_c_indices.begin(), _tmp_c_indices.end()); //this can remove set_g_indices API
			}
			_tmp_c_indices.clear();
			_tmp_c_indices.shrink_to_fit();
		}
		else {
			_tmp_c_indices.emplace_back(val);
		}
	}
	void IFCRenderEngine::set_face_mat(float val) {
		_face_mat.emplace_back(val);
	}
	void IFCRenderEngine::set_edge_indices(int val) {
		_edge_indices.emplace_back(val);
	}
	void IFCRenderEngine::init(GLFWwindow* wndPtr)
	{
		auto& configs = m_cache_configs;

		if (!m_init) { //初次打开窗口
			// 获取config数据
			int width = atoi(configs["width"].c_str());
			int height = atoi(configs["height"].c_str());
			// use_transparency = configs["use_transparency"] == "true";
			String graphics_api = configs["render_api"];
			if (graphics_api == "vulkan") {
				m_render_api = VULKAN_RENDER_API;
			}

			//RenderWindow::m_mouse_status
			//glfw初始化、创建窗口、提示多重采样、监控用户事件、垂直同步、创建帧缓冲
			if (m_render_api == OPENGL_RENDER_API) {
				m_render_window = make_shared<RenderWindow>("IFC Render", width, height, true, false, wndPtr);
				m_glrender = make_shared<GLRender>();
			}
			else if (m_render_api == VULKAN_RENDER_API) {
				m_scene.m_ifcObject = ifc_test_model.get();
				m_ifcRender = make_shared<IFCVulkanRender>();
				m_ifcRender->initialize(width, height);
			}
		}
		m_render_window->setDefaultStatus();

		// 加载模型数据
		try_ifc = configs["model_type"] == "ifc";

		if (_g_indices.size() > 0) {
			ifc_test_model = make_shared<IFCModel>(_g_indices, _g_vertices, _g_normals, _c_indices, _face_mat, _edge_indices);
		}
		else {
			String model_file = configs["file"];
			if (try_ifc) {
				ifc_test_model = IFCParser::load(model_file);
			}
			else {
				test_model = DefaultParser::load(model_file);
			}
		}

		if (configs["reset_view_pos"].size() > 0 || m_camera == nullptr) {
			//获得整个模型的模型矩阵、以及缩放系数
			scale_factor = 0;
			glm::mat4 ifc_model_matrix;
			util::get_model_matrix_byBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax(), ifc_model_matrix, scale_factor);
			ifc_test_model->setModelMatrix(ifc_model_matrix);
			ifc_test_model->setScaleFactor(scale_factor);
		}
		else { // 固定模型矩阵（观察位置）
			ifc_test_model->setModelMatrix(ifc_m_matrix);
		}
		if (m_render_api == OPENGL_RENDER_API) {
			//generateIFCMidfile("resources\\models\\ifc_midfile\\newIFC.ifc", 0.01);

			//SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
			//model_vb->upload(test_model->vertices, test_model->indices);
			//model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
			//model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
			//test_model->render_id = m_glrender->addModel(model_vb);
			if (configs["reset_view_pos"].size() > 0 || m_camera == nullptr) { // 固定相机视角方向
				m_camera = make_shared<GLCamera>(m_view_pos);
				m_render_window->setCamera(m_camera);
			}
			// ifc_test_model->m_model = m_camera->getModelMatrixByBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax());

			// add a rendered model
			SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
			SharedPtr<GLVertexBuffer> select_bbx_vb = make_shared<GLVertexBuffer>();
			if (try_ifc) {
				model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->g_indices);
				model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 10, (void*)0);						//位置
				model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 10, (void*)(3 * sizeof(Real)));		//法向量
				model_vb->vertexAttribDesc(2, 3, sizeof(Real) * 10, (void*)(6 * sizeof(Real)));		//颜色
				model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 10, (void*)(9 * sizeof(Real)));		//所在物件的索引

				if (use_transparency) {
					model_vb->uploadNoTransElements(ifc_test_model->no_trans_ind);
					model_vb->uploadTransElements(ifc_test_model->trans_ind);
				}
				model_vb->uploadElementBufferOnly(ifc_test_model->c_indices);
				model_vb->UploadElementEdge(ifc_test_model->edge_indices);

				//hey bro 8 look here, its where your job should be!
				model_vb->uploadCollisionElementBuffer(ifc_test_model->generate_ebo_from_component_ids(this->ifc_test_model->collision_pairs));

				ifc_test_model->render_id = m_glrender->addModel(model_vb);

				//bounding box needs a vertexBuffer as well
				select_bbx_vb->uploadBBXData(ifc_test_model->generate_bbxs_by_vec({ 0 }), ifc_test_model->bbx_drawing_order);
				select_bbx_vb->vertexAttribDesc(0, 3, sizeof(Real) * 3, (void*)0);
				select_bbx_id = m_glrender->addModel(select_bbx_vb);
			}
			else
			{
				model_vb->upload(test_model->vertices, test_model->indices);
				model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
				model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
				test_model->render_id = m_glrender->addModel(model_vb);
			}
		}

		sleep_time = 10;
	}

	void IFCRenderEngine::run()
	{
		if (!m_init) {
			//std::cout << "IFC Engine has to 'initialize' !!!" << std::endl;
			m_init = true;
			switch (m_render_api) {
				case OPENGL_RENDER_API: {
					auto& m_window = *m_render_window;
					while (!m_window.isClose()) {
						//sleep 1 ms to reduce cpu time
						std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

						m_window.processInput();

						//test dynamic ebo of components
						changeGeom();

						drawFrame();

						if (!m_window.swapBuffer()) {
							break;
						}
						m_window.pollEvents();
					}
					m_init = false;
					break;
				}
				case VULKAN_RENDER_API: {
					while (true) {
						// TODO tick

						if (!m_ifcRender->render(m_scene)) {
							break;
						}
					}
					break;
				}
			}
		}
	}

	void IFCRenderEngine::changeGeom() {
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;

		if (m_window.chosen_changed_w) {
			ifc_test_model->update_chosen_list(m_window.chosen_list);
			m_window.geom_changed = true;
			m_window.chosen_changed_w = false;
		}
		else if (m_window.chosen_changed_x) {
			ifc_test_model->update_chosen_list(m_window.chosen_list);
			m_window.geom_changed = true;
			m_window.chosen_changed_x = false;
		}

		if (m_window.geom_changed) {
			ifc_test_model->update_chosen_and_vis_list();
			m_render.DynamicUpdate(ifc_test_model->render_id, ifc_test_model->generate_ebo_from_component_ids(ifc_test_model->cur_c_indices), ifc_test_model->cur_vis_no_trans_ind,
				ifc_test_model->cur_vis_trans_ind, ifc_test_model->cur_edge_ind);
			m_render.ChosenGeomUpdate(ifc_test_model->render_id, ifc_test_model->cur_chosen_no_trans_ind, 
				ifc_test_model->cur_chosen_trans_ind);
			m_window.geom_changed = false;
		}
	}

	void IFCRenderEngine::setSelectCompIds(int command = 0) {
		if (m_render_window == nullptr) {
			return;
		}
		auto& m_window = *m_render_window;
		// command 0、设置显示一些物件；1、高亮选中一些物件
		if (command == 0) {
			m_window.geom_changed = true;
		}
		else if (command == 1) {
			m_window.chosen_changed_x = true;
		}
		ifc_test_model->divide_chose_geom_by_alpha(m_cache_configs["selectIds"], command, m_window.chosen_list);
	}

	void IFCRenderEngine::SetSleepTime(int sleepTime = 10) {
		sleep_time = sleepTime;
	}

	int IFCRenderEngine::getSelectedCompId()
	{
		return m_render_window == nullptr ? -1 : m_render_window->getClickCompId();
	}

	void IFCRenderEngine::getSelectedCompIds() {
		m_render_window->chosen_list;
	}

	SharedPtr<RenderEngine> IFCRenderEngine::getSingleton()
	{
		if (ifcre.get() == nullptr) {
			ifcre = make_shared<IFCRenderEngine>();
		}
		return ifcre;
	}

// private:
	void IFCRenderEngine::drawFrame()
	{
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;
		//m_render.enableTest(DEPTH_TEST);
		//m_render.depthFunc(LESS_FUNC);
		GLColor clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };

		// -------------- ifc model transform by mouse ---------------
		glm::vec3 clicked_coord = m_window.getClickedWorldCoord();
		if (m_window.isMouseHorizontalRot()) {
			float angle = m_window.getMouseHorizontalVel();
			//ifc_test_model->rotateInLocalSpace(clicked_coord, angle);
			//todo: should change camera
			m_camera->rotateByScreenX(clicked_coord, angle);
		}
		if (m_window.isMouseVerticalRot()) {
			float angle = m_window.getMouseVerticalVel();
			//ifc_test_model->rotateInWorldSpace(clicked_coord, angle);
			//m_camera->rotateInWorldSpace(clicked_coord, angle);
			m_camera->rotateByScreenY(clicked_coord, angle);
		}

		if (m_window.isRightMouseClicked()) {
			if (m_window.isMouseMove() && m_last_rmclick) {
				glm::vec3 hover = m_window.getVirtualHoverWorldCoord();
				glm::vec3 step = hover - m_last_hover_pos;
				ifc_test_model->translate(step);
				//wrong way here
				//m_camera->translateByHoverDiv(step);

			}
			m_last_hover_pos = clicked_coord;
			m_last_rmclick = true;
		}
		else {
			m_last_rmclick = false;
		}
		auto model_matrix = ifc_test_model->getModelMatrix();
		ifc_m_matrix = model_matrix;
		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

		{
			m_window.startRenderToWindow();  // 切换到当前frame buffer
			glm::mat4 view = m_camera->getViewMatrix();
			glm::vec3 camera_forwad = m_camera->getViewForward();
			m_render.setViewMatrix(view);
			m_render.setModelMatrix(model_matrix);
			m_render.setInitModelMatrix(ifc_test_model->getInitModelMatrix());
			m_render.setMirrorModelMatrix(ifc_test_model->getMirrorModelMatrix());
			m_render.setModelViewMatrix(view * model_matrix);
			m_render.setProjectionMatrix(m_window.getProjMatrix());
			m_render.setAlpha(1.0);
			m_render.setCameraDirection(camera_forwad);
			m_render.setClippingPlane(m_window.getClippingPlane().out_as_vec4());
			m_render.setClippingBox(m_window.getClippingBoxVectors());
#ifdef TEST_COMP_ID_RES
			m_window.switchRenderCompId();
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, COMP_ID_WRITE, DYNAMIC_ALL); // 高光显示鼠标掠过的物件

			m_render.setCompId(m_window.getClickCompId());
			m_render.setHoverCompId(m_window.getHoverCompId());
			//m_window.switchRenderBack();
#endif

			//// 0. prev: render normal and depth tex of the scene
#ifndef ONLY_DEPTH_NROMAL_RES
			m_window.switchRenderDepthNormal();
#endif
#ifndef TEST_COMP_ID_RES
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, NORMAL_DEPTH_WRITE);
#endif
			//m_window.switchRenderBack();
#ifndef ONLY_DEPTH_NROMAL_RES
			// 1. render scene
			m_window.switchRenderColor();

			m_render.render(ifc_test_model->render_id, DEFAULT_SHADING, DYNAMIC_NO_TRANS);
			//m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING, DYNAMIC_NO_TRANS);

			//2. render chosen scene, no transparency
			m_render.render(ifc_test_model->render_id, CHOSEN_SHADING, CHOSEN_NO_TRANS);

			//3. render transparency scene
			m_render.setAlpha(0.3);
			m_render.render(ifc_test_model->render_id, TRANSPARENCY_SHADING, DYNAMIC_TRANS);

			//4. render chosen scene, no transparency
			m_render.render(ifc_test_model->render_id, CHOSEN_TRANS_SHADING, CHOSEN_TRANS);

			//5. render edges (maybe
			m_render.render(ifc_test_model->render_id, EDGE_SHADING, /*EDGE_LINE*/DYNAMIC_EDGE_LINE);

			//6. render collision geometry
			m_render.render(ifc_test_model->render_id, COLLISION_RENDER, COLLISION);

			//7. render bounding box
			if (m_window.getClickCompId() >= 0) {
				m_render.ModelVertexUpdate(select_bbx_id, ifc_test_model->generate_bbxs_by_vec({ m_window.chosen_list }));
				m_render.render(select_bbx_id, BOUNDINGBOX_SHADING, BBX_LINE);
			}
#endif

			// -------------- render axis, not normal render procedure ---------------
			m_render.renderAxis(*ifc_test_model
				, clicked_coord
				, m_camera->getViewPos()
				, m_view_pos);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
			
			// ----------------------------- render text -----------------------------
			//auto sxaswd = m_render.get_pixel_pos_in_screen(glm::vec4(158.f, 0.7f, 20.f, 1.f), m_window.get_width(), m_window.get_height());

			//m_render.renderText(*ifc_test_model, sxaswd, 1.f, glm::vec3(1.f, 0.5f, 0.f), m_window.get_width(), m_window.get_height());

			// -------------- render clipping plane, not normal render procedure ---------------
			m_render.renderClipBox(m_window.getHidden(), m_window.getClipBox());
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			m_window.endRenderToWindow();
		}
		// post render: render edge
		m_render.postRender(m_window); // 后处理，清空缓冲
	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}