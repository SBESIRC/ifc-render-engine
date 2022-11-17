﻿#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"
#include "ifcrender/render_ui.h"
#include "grid.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "Character.h"

//#define ONLY_DEPTH_NROMAL_RES
#define TEST_COMP_ID_RES

namespace ifcre {
	extern IFCVulkanRender;
	SharedPtr<IFCRenderEngine> ifcre;

	void IFCRenderEngine::setConfig(String key, String value) {
		m_cache_configs[key] = value;
	}
	void IFCRenderEngine::clear_model_data() {
		Vector<uint32_t>().swap(_g_indices);
		Vector<Real>().swap(_g_vertices);
		Vector<Real>().swap(_g_normals);
		Vector<Vector<uint32_t>>().swap(_c_indices);
		Vector<Real>().swap(_face_mat);
		Vector<uint32_t>().swap(_edge_indices);
		//Vector<uint32_t>().swap(_comp_types);
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
			Vector<uint32_t>().swap(_tmp_c_indices);
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

	//void IFCRenderEngine::set_comp_types(int val) {
	//	_comp_types.emplace_back(val);
	//}

	void IFCRenderEngine::set_grid_data(int val) {
		if (val == 0) { // 0代表清空轴网数据
			vector<float>().swap(grid_lines);
			vector<float>().swap(grid_circles);
			grid_text.clear();
			grid_line_reset = true;
			grid_text_reset = true;
			vector<float>().swap(grid_text_data);
		}
		else if (val == 1) { // 1代表结束传输
			//ifc_test_model->generate_circleLines(grid_lines, grid_circles);
			//m_render_window->to_show_grid = true;
		}
		else if (val == 2) { // 2代表隐藏轴网显示
			m_render_window->to_show_grid = false;
		}
		else if (val == 3) { // 3代表显示轴网
			m_render_window->to_show_grid = true;
		}
	}
	void IFCRenderEngine::set_grid_lines(float val) {
		grid_lines.emplace_back(val);
	}
	void IFCRenderEngine::set_grid_circles(float val) {
		grid_circles.emplace_back(val);
	}
	void IFCRenderEngine::set_grid_text(String val) {
		wstring wstr(val.begin(), val.end());
		grid_text.emplace_back(wstr);
	}
	void IFCRenderEngine::set_grid_text_data(float val) {
		grid_text_data.emplace_back(val);
	}



	void IFCRenderEngine::set_collide_command(int val) {
		if (val == 0) { // 开始传输A、B数据前先进行清空
			collide_idsA.clear();
			collide_idsB.clear();
			collide_idsC.clear();
		}
		else if (val == 1) { // A、B传输完成，进行碰撞检测
			collider->bufferData(_g_indices, _g_vertices, _c_indices); // 此处会清空原始数据
			collider->setTotalIds(collide_idsC);
			collider->setRespetcIds(collide_idsA, collide_idsB);
			vector<uint32_t>().swap(collide_twin);
			collide_twin = collider->getIndexArr();
			collide_size = collide_twin.size();
		}
	}
	void IFCRenderEngine::set_collide_idsA(int val) {
		collide_idsA.insert((uint32_t)val);
		collide_idsC.insert((uint32_t)val);
	}
	void IFCRenderEngine::set_collide_idsB(int val) {
		collide_idsB.insert((uint32_t)val);
		collide_idsC.insert((uint32_t)val);
	}
	int IFCRenderEngine::get_collide_ids_size()	{
		return collide_size;
	}
	void IFCRenderEngine::get_collide_ids(int* arr) {
		for (size_t i = 0; i < collide_twin.size(); i++) {
			arr[i] = (int)collide_twin[i];
		}
	}

	void IFCRenderEngine::init(GLFWwindow* wndPtr)
	{
		auto& configs = m_cache_configs;

		if (!m_init) { //初次打开窗口
			// 获取config数据
			width = atoi(configs["width"].c_str());
			height = atoi(configs["height"].c_str());
			// use_transparency = configs["use_transparency"] == "true";
			String graphics_api = configs["render_api"];
			if (graphics_api == "vulkan") {
				m_render_api = VULKAN_RENDER_API;
			}

			//glfw初始化、创建窗口、提示多重采样、监控用户事件、垂直同步、创建帧缓冲
			if (m_render_api == OPENGL_RENDER_API) {
				m_render_window = make_shared<RenderWindow>("IFC Render", width, height, true, false, wndPtr);
				m_glrender = make_shared<GLRender>();
				m_glrender->bind_ui_to_window(*m_render_window);
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

		String model_file = configs["file"];
		if (model_file == "nil") {
			ifc_test_model = make_shared<IFCModel>(_g_indices, _g_vertices, _g_normals, _c_indices, _face_mat, _edge_indices);//, _comp_types);
			collider = make_shared<Collider>();
		}
		else {
			if (try_ifc) {
				ifc_test_model = IFCParser::load(model_file);
				_g_indices = ifc_test_model->g_indices;
				_g_vertices = ifc_test_model->g_vertices;
				_c_indices = ifc_test_model->c_indices;
			}
			else {
				test_model = DefaultParser::load(model_file);
			}
		}

		mousemove = make_shared<bool>(true);///////////////////////////////////////////
		if (configs["reset_view_pos"].size() > 0 || m_camera == nullptr) {
			//获得整个模型的模型矩阵、以及缩放系数
			util::get_model_matrix_byBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax(), ifc_test_model->bbx_model_mat, scale_factor);
			ifc_test_model->setModelMatrix(ifc_test_model->bbx_model_mat);
			ifc_test_model->setScaleFactor(scale_factor);
		}
		else { // 固定模型矩阵（观察位置）
			ifc_test_model->setModelMatrix(ifc_m_matrix);
			ifc_test_model->setScaleFactor(scale_factor); // for remember axis
		}
		ifc_test_model->PrecomputingCubeDirection(); // 将模型设置为glm::vec3(0.f, 0.f, 0.f)
		if (m_render_api == OPENGL_RENDER_API) {
			//generateIFCMidfile("resources\\models\\ifc_midfile\\newIFC.ifc", 0.01);

			if (configs["reset_view_pos"].size() > 0 || m_camera == nullptr) { // 固定相机视角方向
				m_camera = make_shared<GLCamera>(m_view_pos);
				m_camera->PrecomputingCubeDireciton(m_view_pos); // 为相机预设6个位置
				m_render_window->setCamera(m_camera);
			}

			m_render_window->setIfcModel(ifc_test_model);/////////////////
			m_glrender->getClipBox()->setBasePos(ifc_test_model->getpMin(), ifc_test_model->getpMax());///////////////////
			m_glrender->getClipBox()->bind_the_world_coordination(ifc_test_model->bbx_model_mat);///////////////////////
			m_glrender->bind_ui_to_clipbox();///////////////////////
			m_glrender->drawing_plane.get_center(ifc_test_model->getModelCenter());/////////////

			// now, m_glrender should store 100 glm::mat4 as the offset_matrix of storeys to send to GPU memory
			// and these glm::mat4s should be generated by ifc_test_model cause it's related to the ifc data
			// so the code should be like:
			Vector<glm::mat4> offsets_temp_mats(100, glm::mat4(1.0f));
			m_glrender->upload_mat4s_to_gpu(ifc_test_model->tile_offsets_mats(), ifc_test_model->floorIndex());
			// add a rendered model
			SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
			SharedPtr<GLVertexBuffer> select_bbx_vb = make_shared<GLVertexBuffer>();
			if (try_ifc) {
				model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->g_indices);			//上传数据vbo & ebo
				// position 3
				model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 12, (void*)0);//位置
				// normal 3
				model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 12, (void*)(3 * sizeof(Real)));//法向量
				// color 4
				model_vb->vertexAttribDesc(2, 4, sizeof(Real) * 12, (void*)(6 * sizeof(Real)));//颜色
				// comp id 1
				model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 12, (void*)(10 * sizeof(Real)));//所在物件的索引
				// storey id 1
				model_vb->vertexAttribDesc(4, 1, sizeof(Real) * 12, (void*)(11 * sizeof(Real)));

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

	void IFCRenderEngine::offscreenRending(const int index) {
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;

		m_window.offScreenRender();
		dataIntegration();
		m_render.setViewMatrix(m_camera->getPrecomputedViewMatrix(index));
		m_render.setModelMatrix(ifc_test_model->bbx_model_mat);
		m_render.setInitModelMatrix(ifc_test_model->bbx_model_mat);
		m_render.setMirrorModelMatrix(glm::mat4(1.f));
		m_render.setModelViewMatrix(m_camera->getPrecomputedViewMatrix(index) * ifc_test_model->bbx_model_mat);
		m_render.setProjectionMatrix(m_window.getOrthoProjMatrix());
		m_render.setClippingBox(true);
		m_render.render(ifc_test_model->render_id, OFFLINE_SHADING, ALL);
		m_render.renderClipBox();

		m_window.endOffScreenRender();
	}

	void IFCRenderEngine::run()
	{
		if (!m_init) {
			//std::cout << "IFC Engine has to 'initialize' !!!" << std::endl;
			m_init = true;
			switch (m_render_api) {
				case OPENGL_RENDER_API: {
					auto& m_window = *m_render_window;
					offscreenRending();
					while (!m_window.isClose()) {
						//sleep 1 ms to reduce cpu time
						std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

						m_window.processInput();
						offscreenRending();
						//test dynamic ebo of components
						changeGeom();

						drawFrame();
						if (!m_window.getHidden()) {
							m_glrender->AerialViewRender(m_window);
						}
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

// private:
	void IFCRenderEngine::drawFrame()
	{
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;
		//m_render.enableTest(DEPTH_TEST);
		//m_render.depthFunc(LESS_FUNC);
		GLColor clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };

		glm::fvec2 mouse_move_vec(0.f);

		// -------------- ifc model transform by mouse ---------------
		

		if (cube_change_log) {
			ifc_test_model->TranslateToCubeDirection(cube_num); // 设置模型位置
			m_camera->RotateToCubeDirection(cube_num); // 设置相机数据
			cube_change_log = false;
		}
		if (m_window.getClickCompId() >= 0 && m_window.trigger) {
			auto bound_vecs = ifc_test_model->generate_bbxs_bound_by_vec({ m_window.chosen_list });
			zoombyBBX(glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]), glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]));
		}

		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
		{
			m_window.startRenderToWindow();  // 切换到当前frame buffer
			dataIntegration();
#pragma region COMP THINGS

#ifdef TEST_COMP_ID_RES
			m_window.switchRenderCompId();
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, COMP_ID_WRITE, DYNAMIC_ALL); // 高光显示鼠标掠过的物件
			key = m_window.getClickCompId();
			m_render.setCompId(key);//m_render.setCompId(m_window.getClickCompId());

			//todo add gizmo's id here
			m_window.switchRenderUI();
			m_render.renderGizmoInUIlayer(m_camera->getCubeRotateMatrix(), m_window.getWindowSize());
			m_render.renderClipBoxInUIlayer(m_window.getHidden());
			ui_key = m_window.getClickedUIId();
			if (ui_key > -1 && ui_key < 26 && m_render_window->m_mouse_status.single_click) {
				cube_change_log = true;
				cube_num = ui_key;
				m_render_window->m_mouse_status.single_click = false;
			}
			else if (ui_key > 25) {
				ui_key -= 26;
				//int xsig = (mouse_move_vec.x > 0) ? 1 : ((mouse_move_vec.x < 0) ? -1 : 0);
				//int ysig = (mouse_move_vec.y > 0) ? 1 : ((mouse_move_vec.y < 0) ? -1 : 0);
				//int finalsig = xsig + ysig;
				//if (finalsig) {
				//	//std::cout << clp_face_key * 2 + (finalsig > 0 ? 1 : 0) << std::endl;
				//	m_glrender->getClipBox()->updateBox(clp_face_key * 2 + (finalsig > 0 ? 1 : 0), mouse_move_vec.length());
				//}
				m_render.last_clp_face_key = ui_key + 26;
			}

			glm::vec3 clicked_coord = m_window.getClickedWorldCoord();
			//std::cout << clicked_coord.x << "\t" << clicked_coord.y << "\t" << clicked_coord.z << "\t\n";

			m_glrender->getClipBox()->bind_the_world_coordination(ifc_test_model->getModelMatrix());

			m_render.last_hovered_face_key = m_window.getClpBoxFaceId();

			/*if (clp_face_key > -1 && clp_face_key < 26) {
				m_render.last_hovered_face_key = clp_face_key;
			}*/
#endif

#pragma endregion

#pragma region mouse work

			if (*mousemove && !m_window.rotatelock) {
				/*if (m_window.getHidden()) {

				}*/
				if (m_window.isMouseHorizontalRot()) {
					m_camera->rotateByScreenX(clicked_coord, m_window.getMouseHorizontalVel());
				}
				if (m_window.isMouseVerticalRot()) {

					m_camera->rotateByScreenY(clicked_coord, m_window.getMouseVerticalVel());
				}
				if (m_window.isRightMouseClicked()) {
					if (m_window.isMouseMove() && m_last_rmclick) {
						glm::vec3 hover = m_window.getVirtualHoverWorldCoord();
						glm::vec3 step = hover - m_last_hover_pos;
						ifc_test_model->translate(step);
						//wrong way here
						//m_camera->translateByHoverDiv(step);
						//printvec3(step);
					}
					m_last_hover_pos = clicked_coord;
					m_last_rmclick = true;
				}
				else {
					m_last_rmclick = false;
				}
			}
			else {
				mouse_move_vec.x = m_window.getMouseHorizontalVel();
				mouse_move_vec.y = m_window.getMouseVerticalVel();
			}

#pragma endregion

#ifndef ONLY_DEPTH_NROMAL_RES
			// 1. render scene
			m_window.switchRenderColor();

			m_render.render(ifc_test_model->render_id, DEFAULT_SHADING, DYNAMIC_NO_TRANS);
			//m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING, DYNAMIC_NO_TRANS);

			//2. render chosen scene, no transparency// 渲染选中的不透明构件
			m_render.render(ifc_test_model->render_id, CHOSEN_SHADING, CHOSEN_NO_TRANS);

			//3. render transparency scene// 渲染透明的构件
			m_render.setAlpha(trans_alpha);
			m_render.render(ifc_test_model->render_id, TRANSPARENCY_SHADING, DYNAMIC_TRANS);

			//4. render chosen scene, transparency// 渲染选中的透明构件
			m_render.render(ifc_test_model->render_id, CHOSEN_TRANS_SHADING, CHOSEN_TRANS);

			//5. render edges (maybe
			m_render.render(ifc_test_model->render_id, EDGE_SHADING, /*EDGE_LINE*/DYNAMIC_EDGE_LINE);

			//6. render collision geometry
			//m_render.render(ifc_test_model->render_id, COLLISION_RENDER, COLLISION);

			//7. render bounding box
			if (m_window.getClickCompId() >= 0) {
				auto bound_vecs = ifc_test_model->generate_bbxs_bound_by_vec({ m_window.chosen_list });
				auto chosenbbx = ifc_test_model->generate_bbxs_by_vec2(bound_vecs);

				if (!m_window.chosen_list.empty()) {
					uint32_t floor_id = ifc_test_model->this_comp_belongs_to_which_storey[*m_window.chosen_list.begin()];
					m_render.setStoreyMat(ifc_test_model->tile_matrix[floor_id]);
				}

				m_render.ModelVertexUpdate(select_bbx_id, chosenbbx);

				m_render.render(select_bbx_id, BOUNDINGBOX_SHADING, BBX_LINE);
			}

			m_render.ui_update(mousemove, m_window.getHidden() && !m_window.getShowDrawing(), global_alpha, trans_alpha);

#endif
			//8. render sup things
			// render sky box
			//m_render.renderSkybox(m_camera->getViewMatrix(), m_window.getProjMatrix());

			// -------------- render grid ---------------
			if (m_render_window->to_show_grid) {
				m_render.renderGridLine(grid_lines, width, height, grid_line_reset);
				m_render.renderGridText(grid_text, grid_text_data, grid_text_reset);
			}
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			
			// -------------- render axis, not normal render procedure ---------------
			m_render.renderAxis(*ifc_test_model
				, clicked_coord
				, m_camera->getViewPos()
				, m_view_pos);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			// ----------------------------- render text -----------------------------
			if (m_window.getShowText()) {
				auto sxaswd = m_render.get_pixel_pos_in_screen(glm::vec4(158.f, 0.7f, 20.f, 1.f), m_window.get_width(), m_window.get_height());
				m_render.renderText(sxaswd, 1.f, glm::vec3(1.f, 0.5f, 0.f), m_window.get_width(), m_window.get_height());
			}

			// -------------- render clipping plane, not normal render procedure ---------------
			m_render.renderClipBox(m_window.getHidden());
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----


			// ------------- drawing match shading ----------------------------------
			if (m_window.getShowDrawing())
				m_render.renderDrawing(*ifc_test_model);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----


			//--------------- gizmo rendering ----------------------------------------
			m_render.renderGizmo(m_camera->getCubeRotateMatrix(), m_window.getWindowSize());
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			m_render.simpleui->render();
			m_window.endRenderToWindow();
		}
		// post render
		m_render.postRender(m_window); // 后处理，清空缓冲
	}
// ----- ----- ----- ----- ----- ----- ----- ----- 


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

			auto bound_vecs = ifc_test_model->generate_bbxs_bound_by_vec({ ifc_test_model->cur_c_indices });
			ifc_test_model->setpMax(glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]));
			ifc_test_model->setpMin(glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]));

			m_render.DynamicUpdate(ifc_test_model->render_id,
				ifc_test_model->generate_ebo_from_component_ids(ifc_test_model->cur_c_indices),
				ifc_test_model->cur_vis_no_trans_ind,
				ifc_test_model->cur_vis_trans_ind, ifc_test_model->cur_edge_ind);

			m_render.ChosenGeomUpdate(ifc_test_model->render_id,
				ifc_test_model->cur_chosen_no_trans_ind,
				ifc_test_model->cur_chosen_trans_ind);

			m_window.geom_changed = false;
		}
	}

	void IFCRenderEngine::dataIntegration() {
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;

		auto model_matrix = ifc_test_model->getModelMatrix();
		ifc_m_matrix = model_matrix;

		glm::mat4 view = m_camera->getViewMatrix();
		glm::vec3 camera_forwad = m_camera->getViewForward();
		glm::vec3 camera_pos = m_camera->getViewPos();
		m_render.setViewMatrix(view);
		m_render.setModelMatrix(model_matrix);
		m_render.setInitModelMatrix(model_matrix);
		m_render.setMirrorModelMatrix(ifc_test_model->getMirrorModelMatrix());
		m_render.setModelViewMatrix(view * model_matrix);
		m_render.setProjectionMatrix(m_window.getProjMatrix());
		m_render.setAlpha(global_alpha);
		m_render.setCameraDirection(camera_forwad);
		m_render.setCameraPos(camera_pos);
		m_render.setClippingPlane(m_render.getClippingPlane().out_as_vec4());
		m_render.setClippingBox(m_window.getHidden());
		m_render.updateOpenDrawingMatch(m_window.getShowDrawing());
		m_render.TileView(m_window.getShowTileView());

		m_render.setHoverCompId(m_window.getHoverCompId());
	}

	void IFCRenderEngine::setSelectCompIds(int val = 0) {
		switch (val) {
		case -1: // start
			if (m_render_window == nullptr) {
				return;
			}
			to_show_states = stoi(m_cache_configs["to_show_states"]);
			// to_show_states 0、设置显示一些物件；1、高亮选中一些物件
			if (to_show_states == 0) {
				m_render_window->geom_changed = true;
			}
			else if (to_show_states == 1) {
				m_render_window->chosen_changed_x = true;
			}
			if (to_show_states == 0) {
				Vector<CompState>(ifc_test_model->c_indices.size(), DUMP).swap(ifc_test_model->comp_states);
				Vector<uint32_t>().swap(ifc_test_model->cur_c_indices);
			}
			m_render_window->chosen_list.clear();
			break;
		case -2: // show all elements
			if (m_render_window == nullptr) {
				return;
			}
			// to_show_states 0、设置显示一些物件；1、高亮选中一些物件
			m_render_window->geom_changed = true;
			Vector<CompState>(ifc_test_model->c_indices.size(), VIS).swap(ifc_test_model->comp_states);
			ifc_test_model->cur_c_indices.clear();
			for (int i = 0; i < ifc_test_model->c_indices.size(); ++i) {
				ifc_test_model->cur_c_indices.emplace_back(i);
			}
			break;
		default:
			if (val < ifc_test_model->c_indices.size()) {
				if (0 == to_show_states) {
					ifc_test_model->comp_states[val] = VIS;
					ifc_test_model->cur_c_indices.emplace_back(val);
				}
				else if (1 == to_show_states) {
					ifc_test_model->comp_states[val] = CHOSEN;
					m_render_window->chosen_list.insert(val);
				}
			}
			break;
		}
	}

	//void IFCRenderEngine::SetClipBox() {
	//	if (m_render_window == nullptr) {
	//		return;
	//	}
	//	Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
	//	uint32_t c_indices_size = ifc_test_model->c_indices.size();
	//	for (auto& id : m_render_window->chosen_list) {
	//		if (id > c_indices_size) {
	//			continue;
	//		}
	//		for (int i = 0; i < 3; i++) {
	//			ret[i] = std::min(ret[i], ifc_test_model->comps_bbx[6 * id + i]);
	//		}
	//		for (int i = 3; i < 6; i++) {
	//			ret[i] = std::max(ret[i], ifc_test_model->comps_bbx[6 * id + i]);
	//		}
	//	}
	//	Vector<Real> ret2(24); // 一个bbx有8个顶点，每个顶点要3个float存储位置
	//	for (int i = 0; i < 8; i++) {
	//		ret2[i * 3] = i & 1 ? ret[3] : ret[0];
	//		ret2[i * 3 + 1] = i & 2 ? ret[4] : ret[1];
	//		ret2[i * 3 + 2] = i & 4 ? ret[5] : ret[2];
	//	}
	//	//glm::vec3 center;
	//	m_glrender->use_clip_box = ClipBox(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f), abs(ret[3] - ret[0]), abs(ret[4] - ret[1]), abs(ret[5] - ret[2]));
	//}

	void IFCRenderEngine::SetSleepTime(int sleepTime = 10) {
		sleep_time = sleepTime;
	}

	int IFCRenderEngine::getSelectedCompId()
	{
		return m_render_window == nullptr ? -1 : m_render_window->getClickCompId();
	}

	int IFCRenderEngine::getSelectedCompIdsSize()
	{
		return m_render_window->chosen_list.size();
	}
	void IFCRenderEngine::getSelectedCompIds(int *arr) {
		std::vector <int> v(m_render_window->chosen_list.begin(), m_render_window->chosen_list.end());
		for (size_t i = 0; i < v.size(); i++)
		{
			arr[i] = v[i];
		}
	}

	void IFCRenderEngine::zoombyBBX(glm::vec3 minvec3, glm::vec3 maxvec3) {
		m_render_window->trigger = false;
		glm::mat4 model_mat;
		Real scaler = 0;
		util::get_model_matrix_byBBX(minvec3, maxvec3, model_mat, scaler);
		//ifc_test_model->setModelMatrix(model_mat);
		if (m_render_window->getShowTileView()) {
			glm::mat4 trans = ifc_test_model->tile_matrix[ifc_test_model->this_comp_belongs_to_which_storey[*m_render_window->chosen_list.begin()]];
			ifc_test_model->setModelMatrix(model_mat * util::inverse_mat4(trans));
		}
		else {
			ifc_test_model->setModelMatrix(model_mat);
		}
		ifc_test_model->setScaleFactor(scaler);
		m_camera->set_pos((m_render_window->_isperspectivecurrent ? -15.f : -100.f) * m_camera->getViewForward() / scaler / 4.f);
	}

	void IFCRenderEngine::zoom2Home() {
		if (ifc_test_model == NULL) {
			return;
		}
		
		zoombyBBX(ifc_test_model->getpMax(), ifc_test_model->getpMin());
	}

	bool IFCRenderEngine::saveImage(const char* filePath) {
		return m_render_window->SaveImage(filePath, width, height);
	}

	SharedPtr<RenderEngine> IFCRenderEngine::getSingleton()
	{
		if (ifcre.get() == nullptr) {
			ifcre = make_shared<IFCRenderEngine>();
		}
		return ifcre;
	}
}
