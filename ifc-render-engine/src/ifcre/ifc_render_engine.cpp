#include <Windows.h>
#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"
#include "ifcrender/render_ui.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "Character.h"
#include "common/logger.h"
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

	void IFCRenderEngine::set_grid_data(int val) {
		if (val == 0) { // 0代表清空轴网数据
			m_DataIsReady = false;
			vector<float>().swap(grid_lines);
			vector<float>().swap(grid_circles);
			Vector<Wstring>().swap(grid_text);
			grid_line_reset = true;
			grid_text_reset = true;
			vector<float>().swap(grid_text_data);
		}
		else if (val == 1) { // 1代表结束传输
			//ifc_test_model->generate_circleLines(grid_lines, grid_circles);
			//m_render_window->to_show_grid = true;
			m_DataIsReady = true;
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
		std::wstring wstr(val.begin(), val.end());
		grid_text.emplace_back(wstr);
	}
	void IFCRenderEngine::set_grid_text_data(float val) {
		grid_text_data.emplace_back(val);
	}

	void IFCRenderEngine::init(GLFWwindow* wndPtr)
	{
		m_DataIsReady = false;

		// 获取config数据
		int width = atoi(m_cache_configs["width"].c_str());
		int height = atoi(m_cache_configs["height"].c_str());

		//glfw初始化、创建窗口、提示多重采样、监控用户事件、垂直同步、创建帧缓冲
		if (m_render_window == nullptr)
		{
			m_render_window = make_shared<RenderWindow>("IFC Render", width, height, true, false, wndPtr);
		}
		m_glrender = make_shared<GLRender>();
		m_glrender->clear_model();
		m_render_window->setDefaultStatus();

		// 加载模型数据
		String model_file = m_cache_configs["file"];
		if (model_file == "nil") {
			ifc_model = make_shared<IFCModel>(_g_indices, _g_vertices, _g_normals, _c_indices, _face_mat, _edge_indices);
		}
		else {
			ifc_model = IFCParser::load(m_cache_configs["file"]);
		}
		
		mousemove = make_shared<bool>(true);///////////////////////////////////////////
		if (m_cache_configs["reset_view_pos"].size() > 0 || m_camera == nullptr) {
			//获得整个模型的模型矩阵、以及缩放系数
			util::get_model_matrix_byBBX(ifc_model->getpMin(), ifc_model->getpMax(), ifc_model->bbx_model_mat, scale_factor);
			ifc_model->setModelMatrix(ifc_model->bbx_model_mat);
			ifc_model->setScaleFactor(scale_factor);
		}
		else { // 固定模型矩阵（观察位置）
			ifc_model->setModelMatrix(ifc_m_matrix);
			ifc_model->setScaleFactor(scale_factor); // for remember axis
		}
		ifc_model->PrecomputingCubeDirection(); // 将模型设置为glm::vec3(0.f, 0.f, 0.f)

		if (m_cache_configs["reset_view_pos"].size() > 0 || m_camera == nullptr) { // 固定相机视角方向
			m_camera = make_shared<GLCamera>(m_view_pos);
			m_camera->PrecomputingCubeDireciton(m_view_pos); // 为相机预设6个位置
		}
		m_render_window->setCamera(m_camera);

		m_render_window->setIfcModel(ifc_model);/////////////////

		UploadOriginalData();

		m_DataIsReady = true;
		
	}

	void IFCRenderEngine::UploadOriginalData()
	{
		m_glrender->getClipBox()->setBasePos(ifc_model->getpMin(), ifc_model->getpMax());///////////////////
		m_glrender->getClipBox()->bind_the_world_coordination(ifc_model->bbx_model_mat);///////////////////////
																							 //m_glrender->bind_ui_to_clipbox();///////////////////////
		m_glrender->drawing_plane.get_center(ifc_model->getModelCenter());/////////////
																			   //m_glrender->drawing_plane.get_lowest_y(ifc_test_model->bbxs_each_floor);

																			   // now, m_glrender should store 100 glm::mat4 as the offset_matrix of storeys to send to GPU memory
																			   // and these glm::mat4s should be generated by ifc_test_model cause it's related to the ifc data
																			   // so the code should be like:
		Vector<glm::mat4> offsets_temp_mats(100, glm::mat4(1.0f));
		m_glrender->upload_mat4s_to_gpu(offsets_temp_mats);

		m_glrender->TileViewMatrix(ifc_model);

		// add a rendered model

	 	auto model_vb = make_shared<GLVertexBuffer>();
		auto select_bbx_vb = make_shared<GLVertexBuffer>();
		model_vb->upload(ifc_model->ver_attrib, ifc_model->g_indices);			//上传数据vbo & ebo
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

		//hey bro 8 look here, its where your job should be!
		//collision_list = { ifc_test_model->collider_inde_hard[ifc_test_model->collider_ind_count * 2],ifc_test_model->collider_inde_hard[ifc_test_model->collider_ind_count * 2 + 1] };
		//ifc_test_model->collider_ind_count = (ifc_test_model->collider_ind_count + 1) % (ifc_test_model->collider_inde_hard.size() / 2);
		//model_vb->uploadCollisionElementBuffer(ifc_test_model->generate_ebo_from_component_ids(collision_list));
		m_glrender->clear_model();
		ifc_model->render_id = m_glrender->addModel(model_vb, 1);

		//bounding box needs a vertexBuffer as well
		select_bbx_vb->uploadBBXData(ifc_model->generate_bbxs_by_vec({ 0 }), ifc_model->bbx_drawing_order);
		select_bbx_vb->vertexAttribDesc(0, 3, sizeof(Real) * 3, (void*)0);
		select_bbx_id = m_glrender->addModel(select_bbx_vb, 2);
	}

	void IFCRenderEngine::offscreenRending(const int index) {
		m_render_window->offScreenRender();

		dataIntegration();
		m_glrender->setViewMatrix(m_camera->getPrecomputedViewMatrix(index));
		m_glrender->setModelMatrix(ifc_model->bbx_model_mat);
		m_glrender->setInitModelMatrix(ifc_model->bbx_model_mat);
		m_glrender->setMirrorModelMatrix(glm::mat4(1.f));
		m_glrender->setModelViewMatrix(m_camera->getPrecomputedViewMatrix(index) * ifc_model->bbx_model_mat);
		m_glrender->setProjectionMatrix(m_render_window->getOrthoProjMatrix());
		m_glrender->setClippingBox(true);

		m_glrender->ubo_datasa_updates();

		m_glrender->render(ifc_model->render_id, RenderTypeEnum::OFFLINE_SHADING, RenderPartEnum::ALL);
		m_glrender->renderClipBox();

		m_render_window->endOffScreenRender();
	}

	void IFCRenderEngine::zoom_into(Vector<Real> bound_vecs, glm::vec3& clicked_coord) {
		glm::mat4 model_mat;
		Real scaler = 0;
		glm::vec3 pmin = glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]);
		glm::vec3 pmax = glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]);
		util::get_model_matrix_byBBX(pmin, pmax, model_mat, scaler);
		if (/*m_render_window->getShowTileView()*/tileViewButton) {
			glm::mat4 trans = ifc_model->tile_matrix[ifc_model->this_comp_belongs_to_which_storey[*m_render_window->chosen_list.begin()]];
			ifc_model->setModelMatrix(model_mat * util::inverse_mat4(trans));
		}
		else
			ifc_model->setModelMatrix(model_mat);
		ifc_model->setScaleFactor(scaler);
		ifc_model->curcenter = (pmin + pmax) / 2.f;
		m_camera->set_pos((m_render_window->_isperspectivecurrent ? -15.f : -100.f) * m_camera->getViewForward() / scaler / 4.f);
		//flag_between_zoom_reset = true;
	}

	void IFCRenderEngine::run()
	{
		if (m_DoesRenderAlreadyRunning)
			return;

		m_DoesRenderAlreadyRunning = true;

		switch (m_render_api)
		{
			case OPENGL_RENDER_API: {

				offscreenRending();
				while (!m_render_window->isClose())
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
					m_render_window->processInput();

					if (!m_DataIsReady)
					{
						m_render_window->pollEvents();
						continue;
					}

					offscreenRending();
					//update dynamic data
					updateDynamicEboData();

					drawFrame();

					//if (/*!m_render_window->getHidden()*/clipboxButton) {
					//	m_glrender->AerialViewRender(m_window);
					//}

					if (!m_render_window->swapBuffer())
						break;

					m_render_window->pollEvents();
				}
				break;
			}
			case VULKAN_RENDER_API: {
				while (true) {
					// TODO tick

					if (!m_ifcRender->render(m_vulkanScene)) {
						break;
					}
				}
				break;
			}
		}
	}

	void IFCRenderEngine::drawFrame()
	{
		//m_glrender->enableTest(DEPTH_TEST);
		//m_glrender->depthFunc(LESS_FUNC);

		glm::fvec2 mouse_move_vec(0.f);

#pragma region transform by mouse
		// -------------- ifc model transform by mouse ---------------

		glm::vec3 clicked_coord = m_render_window->getClickedWorldCoord();
		//reset_coord(clicked_coord);
		if (cube_change_log) {
			ifc_model->TranslateToCubeDirection(cube_num); // 设置模型位置
			m_camera->RotateToCubeDirection(cube_num); // 设置相机数据
			cube_change_log = false;
		}
		/*if (m_render_window->getClickCompId() >= 0 && m_render_window->trigger) {
			auto bound_vecs = ifc_test_model->generate_bbxs_bound_by_vec({ m_render_window->chosen_list });
			zoombyBBX(glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]), glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]));
		}*/
		if (m_render_window->getClickCompId() >= 0 && m_render_window->trigger) {
			m_render_window->trigger = false;
			auto bound_vecs = ifc_model->generate_bbxs_bound_by_vec({ m_render_window->chosen_list });
			zoom_into(bound_vecs, clicked_coord);
		}
		if (m_render_window->collidertrig) {
			m_render_window->collidertrig = !m_render_window->collidertrig;
			//showcolid = true;
			collision_list = { ifc_model->collider_inde_hard[ifc_model->collider_ind_count * 2],ifc_model->collider_inde_hard[ifc_model->collider_ind_count * 2 + 1] };
			ifc_model->collider_ind_count = (ifc_model->collider_ind_count + 1) % (ifc_model->collider_inde_hard.size() / 2);
			//m_render_window->chosen_changed = true;
			auto bound_vecs = ifc_model->generate_bbxs_bound_by_vec(collision_list);
			zoom_into(bound_vecs, clicked_coord);

			glm::vec3 pmin = glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]);
			glm::vec3 pmax = glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]);
			/*auto tempp = ifc_test_model->getModelMatrix() * glm::vec4(pmin, 1.0f);
			tempp /= tempp.w;
			pmin = tempp;

			tempp = ifc_test_model->getModelMatrix() * glm::vec4(pmax, 1.0f);
			tempp /= tempp.w;
			pmax = tempp;*/
			clipboxButton = true;
			collider_trans_flag = true;
			m_glrender->getClipBox()->setBox(pmin, pmax);
		}

#pragma region mouse work

		if (*mousemove && !m_render_window->rotatelock) {

			if (m_render_window->isMouseHorizontalRot()) {
				m_camera->rotateByScreenX(clicked_coord, m_render_window->getMouseHorizontalVel());
			}
			if (m_render_window->isMouseVerticalRot()) {

				m_camera->rotateByScreenY(clicked_coord, m_render_window->getMouseVerticalVel());
			}
		}
		else {
			mouse_move_vec.x = m_render_window->getMouseHorizontalVel();
			mouse_move_vec.y = m_render_window->getMouseVerticalVel();
		}
		if (m_render_window->isRightMouseClicked()) {
#ifdef test_cmr
			glm::vec3 hover = m_render_window->getVirtualHoverViewCordCoord();
#else
			glm::vec3 hover = m_render_window->getVirtualHoverWorldCoord();
#endif // test_cmr

			if (m_render_window->isMouseMove() && m_last_rmclick) {
				glm::vec3 step = hover - m_last_hover_pos;
#ifdef test_cmr
				//wrong way here
				m_camera->translateByHoverDiv(step);
#else
				ifc_model->translate(step);
#endif // test_cmr
			}

			m_last_rmclick = true;
		}
		else {
			m_last_rmclick = false;
		}
#ifdef test_cmr
		m_last_hover_pos = m_render_window->getClickedViewCordCoord();
#else
		m_last_hover_pos = m_render_window->getClickedWorldCoord();
#endif // test_cmr
		/*if (m_render_window->scrolltrigger) {
			m_camera->zoom( clicked_coord, m_render_window->scrollyoffset > 0 ? 1.0f : -1.0f);
			m_render_window->scrolltrigger = false;
		}*/

#pragma endregion

		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
#pragma endregion
		{
			m_render_window->startRenderToWindow();  // 切换到当前frame buffer
			dataIntegration();
			m_glrender->ubo_datasa_updates();

#pragma region COMP THINGS

			m_render_window->switchRenderCompId();
			m_glrender->render(ifc_model->render_id, COMP_ID_WRITE, DYNAMIC_ALL); // 将构件id写入framebuffer
			int selectedId = m_render_window->getClickCompId();
			m_glrender->setCompId(selectedId);

			m_render_window->switchRenderUI();
			m_glrender->renderGizmoInUIlayer(m_camera->getCubeRotateMatrix(), m_render_window->getWindowSize());
			m_glrender->renderClipBoxInUIlayer(/*m_render_window->getHidden()*/!clipboxButton);
			ui_key = m_render_window->getClickedUIId();
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
				m_glrender->last_clp_face_key = ui_key + 26;
			}

			m_glrender->getClipBox()->bind_the_world_coordination(ifc_model->getModelMatrix());

			m_glrender->last_hovered_face_key = m_render_window->getClpBoxFaceId();

#pragma endregion

#pragma region main render

			// 1. render scene
			m_render_window->switchRenderColor();

			m_glrender->renderSkyBox(m_render_window->returnPerispectiveMat());

			m_glrender->transformUBO_refresh();

			//render opaque scene// 渲染不透明构件
			m_glrender->render(ifc_model->render_id, RenderTypeEnum::DEFAULT_SHADING, RenderPartEnum::DYNAMIC_NO_TRANS);
			//m_glrender->render(ifc_model->render_id, RenderTypeEnum::DEFAULT_SHADING, RenderPartEnum::DYNAMIC_ALL);

			//render chosen scene, no transparency// 渲染选中的不透明构件
			m_glrender->render(ifc_model->render_id, RenderTypeEnum::CHOSEN_SHADING, RenderPartEnum::CHOSEN_NO_TRANS);

			//render transparency scene// 渲染透明的构件
			m_glrender->setAlpha(m_trans_alpha);
			m_glrender->render(ifc_model->render_id, RenderTypeEnum::TRANSPARENCY_SHADING, RenderPartEnum::DYNAMIC_TRANS);

			//render chosen scene, transparency// 渲染选中的透明构件
			m_glrender->render(ifc_model->render_id, RenderTypeEnum::CHOSEN_TRANS_SHADING, RenderPartEnum::CHOSEN_TRANS);

			//render edges (maybe
			m_glrender->render(ifc_model->render_id, RenderTypeEnum::EDGE_SHADING, RenderPartEnum::DYNAMIC_EDGE_LINE);

			//render collision geometry
			if (showcolid)
			{
				m_glrender->render(ifc_model->render_id, RenderTypeEnum::COLLISION_RENDER, RenderPartEnum::COLLISION);
			}

			//// ------------- edge length shading ---------------------------------- // don't use this, there are a few errors!!!
			//if (m_window.edgeIdTrigger)
			//	m_render.renderEdgeLength(*ifc_test_model, m_window.get_width(), m_window.get_height());
			//// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			// render bounding box
			if (m_render_window->getClickCompId() >= 0) {
				auto bound_vecs = !/*m_render_window->getShowTileView()*/tileViewButton ? ifc_model->generate_bbxs_bound_by_vec({ m_render_window->chosen_list })
					: ifc_model->generate_bbxs_bound_by_vec({ m_render_window->chosen_list }, true);
				auto chosenbbx = ifc_model->generate_bbxs_by_vec2(bound_vecs);

				/*if (!m_render_window->chosen_list.empty()) {
					uint32_t floor_id = ifc_test_model->this_comp_belongs_to_which_storey[*m_render_window->chosen_list.begin()];
					m_glrender->setStoreyMat(ifc_test_model->tile_matrix[floor_id]);
				}*/

				m_glrender->ModelVertexUpdate(select_bbx_id, chosenbbx);

				m_glrender->render(select_bbx_id, BOUNDINGBOX_SHADING, BBX_LINE);
			}

			//m_glrender->ui_update(mousemove, /*m_render_window->getHidden()*/!clipboxButton && /*!m_render_window->getShowDrawing()*/ !drawingMatchButton,
			//	global_alpha, trans_alpha,
			//	script_scale_fractor, m_render_window->getDragMouseMove(), m_render_window->getlbtndown()
			//);
			//m_glrender->simpleui->updateBool(clipboxButton, drawingMatchButton, tileViewButton, showcolid, m_render_window->collidertrig);

			*mousemove = !clipboxButton;
			//8. render sup things
			// render sky box
			//m_glrender->renderSkybox(m_camera->getViewMatrix(), m_render_window->getProjMatrix());

			// -------------- render grid ---------------
			if (m_render_window->to_show_grid) {
				m_glrender->renderGridLine(grid_lines, m_render_window->get_width(), m_render_window->get_height(), grid_line_reset);
				m_glrender->renderGridText(grid_text, grid_text_data, grid_text_reset);
			}
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----


			// -------------- render axis, not normal render procedure ---------------
			m_glrender->renderAxis(*ifc_model
				, clicked_coord
				, m_camera->getViewPos()
				, m_view_pos);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			// ----------------------------- render text -----------------------------
			if (m_render_window->getShowText()) {
				auto sxaswd = m_glrender->get_pixel_pos_in_screen(glm::vec4(158.f, 0.7f, 20.f, 1.f), m_render_window->get_width(), m_render_window->get_height());
				m_glrender->renderText(sxaswd, 1.f, glm::vec3(1.f, 0.5f, 0.f), m_render_window->get_width(), m_render_window->get_height());
			}

			// -------------- render clipping plane, not normal render procedure ---------------
			m_glrender->renderClipBox(/*m_render_window->getHidden()*/!clipboxButton);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----


			// ------------- drawing match shading ----------------------------------
			if (/*m_render_window->getShowDrawing()*/drawingMatchButton)
				m_glrender->renderDrawing(*ifc_model, script_scale_fractor);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			// ------------- tile view drawing shading ---------------------------------- // don't use this, there are a few errors!!!
			if (/*m_render_window->getShowTileView()*/tileViewButton)
				;
			//m_glrender->renderTileViewDrawing(*ifc_test_model);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			//---------------View Cube( gizmo ) rendering ----------------------------------------
			m_glrender->renderViewCube(m_camera->getCubeRotateMatrix(), m_render_window->getWindowSize());
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			//m_glrender->simpleui->render();

			fps(0.125);

#pragma endregion

			m_render_window->endRenderToWindow();
		}
		// post render
		m_glrender->postRender(*m_render_window); // 后处理，清空缓冲
	}
	// ----- ----- ----- ----- ----- ----- ----- ----- 

	void IFCRenderEngine::updateDynamicEboData() {
		if (m_render_window->geom_changed) {
			ifc_model->update_chosen_list(m_render_window->chosen_list);
			ifc_model->update_chosen_and_vis_list();

			auto bound_vecs = ifc_model->generate_bbxs_bound_by_vec({ ifc_model->cur_c_indices });
			ifc_model->setpMax(glm::vec3(bound_vecs[0], bound_vecs[1], bound_vecs[2]));
			ifc_model->setpMin(glm::vec3(bound_vecs[3], bound_vecs[4], bound_vecs[5]));

			m_glrender->DynamicUpdate(ifc_model->render_id,
				ifc_model->generate_ebo_from_component_ids(ifc_model->cur_c_indices),
				ifc_model->cur_vis_no_trans_ind,
				ifc_model->cur_vis_trans_ind, ifc_model->cur_edge_ind);

			m_glrender->ChosenGeomUpdate(ifc_model->render_id,
				ifc_model->cur_chosen_no_trans_ind,
				ifc_model->cur_chosen_trans_ind);

			m_render_window->geom_changed = false;
		}
		if (collider_trans_flag && !collision_list.empty()) {
			ifc_model->generate_collision_list(collision_list);
			m_glrender->CollisionGeomUpdate(ifc_model->render_id, ifc_model->collision_ebo);
			collider_trans_flag = false;
		}
	}

	void IFCRenderEngine::dataIntegration() {
		auto model_matrix = ifc_model->getModelMatrix();
		ifc_m_matrix = model_matrix;

		glm::mat4 view = m_camera->getViewMatrix();
		glm::vec3 camera_forwad = m_camera->getViewForward();
		glm::vec3 camera_pos = m_camera->getViewPos();
		m_glrender->setViewMatrix(view);
		m_glrender->setModelMatrix(model_matrix);
		m_glrender->setInitModelMatrix(model_matrix);
		m_glrender->setMirrorModelMatrix(ifc_model->getMirrorModelMatrix());
		m_glrender->setModelViewMatrix(view * model_matrix);
		m_glrender->setProjectionMatrix(m_render_window->getProjMatrix());
		m_glrender->setAlpha(global_alpha);
		m_glrender->setCameraDirection(camera_forwad);
		m_glrender->setCameraPos(camera_pos);
		m_glrender->setClippingPlane(m_glrender->getClippingPlane().out_as_vec4());
		m_glrender->setClippingBox(/*m_render_window->getHidden()*/!clipboxButton);
		m_glrender->updateOpenDrawingMatch(/*m_render_window->getShowDrawing()*/drawingMatchButton);
		m_glrender->TileView(/*m_render_window->getShowTileView()*/tileViewButton);

		m_glrender->setHoverCompId(m_render_window->getHoverCompId());
	}

	void IFCRenderEngine::setSelectCompIds(int val = 0) {
		switch (val) {
		case -1: // start
			if (m_render_window == nullptr) {
				return;
			}
			to_show_states = stoi(m_cache_configs["to_show_states"]);
			// to_show_states 0、设置显示一些物件；1、高亮选中一些物件
			if (to_show_states == 0 || to_show_states == 1) {
				m_render_window->geom_changed = true;
			}
			if (to_show_states == 0) {
				Vector<CompState>(ifc_model->c_indices.size(), DUMP).swap(ifc_model->comp_states);
				Vector<uint32_t>().swap(ifc_model->cur_c_indices);
			}
			m_render_window->chosen_list.clear();
			break;
		case -2: // show all elements
			if (m_render_window == nullptr) {
				return;
			}
			// to_show_states 0、设置显示一些物件；1、高亮选中一些物件
			m_render_window->geom_changed = true;
			Vector<CompState>(ifc_model->c_indices.size(), VISIABLE).swap(ifc_model->comp_states);
			Vector<uint32_t>().swap(ifc_model->cur_c_indices);
			for (int i = 0; i < ifc_model->c_indices.size(); ++i) {
				ifc_model->cur_c_indices.emplace_back(i);
			}
			break;
		default:
			if (val < ifc_model->c_indices.size()) {
				if (0 == to_show_states) {
					ifc_model->comp_states[val] = VISIABLE;
					ifc_model->cur_c_indices.emplace_back(val);
				}
				else if (1 == to_show_states) {
					ifc_model->comp_states[val] = CHOSEN;
					m_render_window->chosen_list.insert(val);
				}
			}
			break;
		}
	}

	void IFCRenderEngine::SetSleepTime(int sleepTime = 10) {
		m_sleepTime = sleepTime;
	}
	void IFCRenderEngine::SetDataReadyStatus(bool dataIsReady) {
		m_DataIsReady = dataIsReady;
	}
	int IFCRenderEngine::getSelectedCompId()
	{
		return m_render_window == nullptr ? -1 : m_render_window->getClickCompId();
	}

	int IFCRenderEngine::getSelectedCompIdsSize()
	{
		return m_render_window->chosen_list.size();
	}
	void IFCRenderEngine::getSelectedCompIds(int* arr) {
		auto it = m_render_window->chosen_list.begin();
		int i = 0;
		for (it, i; it != m_render_window->chosen_list.end(); ++it,++i) {
			arr[i] = *it;
		}
	}

	void IFCRenderEngine::zoombyBBX(glm::vec3 minvec3, glm::vec3 maxvec3) {
		//info("use zoom");
		m_render_window->trigger = false;
		glm::mat4 model_mat;
		Real scaler = 0;
		util::get_model_matrix_byBBX(minvec3, maxvec3, model_mat, scaler);
		//ifc_test_model->setModelMatrix(model_mat);
		if (/*m_render_window->getShowTileView()*/tileViewButton) {

			if (m_render_window->chosen_list.size() > 0)
			{
				glm::mat4 trans = ifc_model->tile_matrix[ifc_model->this_comp_belongs_to_which_storey[*m_render_window->chosen_list.begin()]];
				ifc_model->setModelMatrix(model_mat * util::inverse_mat4(trans));
			}
		}
		else {
			ifc_model->setModelMatrix(model_mat);
		}
		ifc_model->setScaleFactor(scaler);
		m_camera->set_pos((m_render_window->_isperspectivecurrent ? -15.f : -100.f) * m_camera->getViewForward() / scaler / 4.f);
	}

	void IFCRenderEngine::zoom2Home() {
		//info("zoom hoom");
		if (ifc_model == NULL) {
			return;
		}

		zoombyBBX(ifc_model->getpMax(), ifc_model->getpMin());
	}

	bool IFCRenderEngine::saveImage(const char* filePath) {
		return m_render_window->SaveImage(filePath, m_render_window->get_width(), m_render_window->get_height());
	}

	SharedPtr<RenderEngine> IFCRenderEngine::getSingleton()
	{
		if (ifcre.get() == nullptr) {
			ifcre = make_shared<IFCRenderEngine>();
		}

		return ifcre;
	}

	void IFCRenderEngine::fps(double interval) {
		static double sumTime = 0.0;
		static double countFrames = 0.0;
		double deltaTime = m_render_window->getDelta_time();
		static std::string fram;
		if (sumTime <= interval) {
			++countFrames;
			sumTime += deltaTime;
		}
		else {
			fram = std::to_string(int(countFrames / interval));
			countFrames = 0.0;
			sumTime = 0.0;
		}
		m_glrender->renderText("FPS:" + fram, glm::vec3(m_render_window->get_width() - 64, 6, 0), 0.3, glm::vec3(0.33f, 0.33f, 0.33f), m_render_window->get_width(), m_render_window->get_height());
	}
}
