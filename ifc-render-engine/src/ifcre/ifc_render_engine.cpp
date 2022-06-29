#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"
#include "ifcrender/render_ui.h"
#include <chrono>
#include <thread>
#include<iostream>

//#define ONLY_DEPTH_NROMAL_RES
#define TEST_COMP_ID_RES

namespace ifcre {
	extern IFCVulkanRender;
	SharedPtr<IFCRenderEngine> ifcre;

	void IFCRenderEngine::setConfig(String key, String value)
	{
		m_cache_configs[key] = value;
	}

	void IFCRenderEngine::init(GLFWwindow* wndPtr)
	{
		auto& configs = m_cache_configs;

		if (!m_init) {
			int width = atoi(configs["width"].c_str());
			int height = atoi(configs["height"].c_str());
			try_ifc = configs["model_type"] == "ifc";
			use_transparency = configs["use_transparency"] == "true";
			String graphics_api = configs["render_api"];
			if (graphics_api == "vulkan") {
				m_render_api = VULKAN_RENDER_API;
			}
			if (m_render_api == OPENGL_RENDER_API) {
				m_render_window = make_shared<RenderWindow>("IFC Render", width, height, true, false, wndPtr);
			}
			if (m_render_api == VULKAN_RENDER_API) {
				m_scene.m_ifcObject = ifc_test_model.get();
				m_ifcRender = make_shared<IFCVulkanRender>();
				m_ifcRender->initialize(width, height);
			}
			m_init = true;
		}

		m_render_window->setDefaultStatus();

		//RenderWindow::m_mouse_status
		
		String model_file = configs["file"];
		
		if (try_ifc) {
			ifc_test_model = IFCParser::load(model_file);
		}
		else {
			test_model = DefaultParser::load(model_file);
		}
		
		Real scale_factor = 0;
		glm::mat4 ifc_model_matrix;
		util::get_model_matrix_byBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax(), ifc_model_matrix, scale_factor);
		ifc_test_model->setModelMatrix(ifc_model_matrix);
		ifc_test_model->setScaleFactor(scale_factor);

		if (m_render_api == OPENGL_RENDER_API) {
			//generateIFCMidfile("resources\\models\\ifc_midfile\\newIFC.ifc", 0.01);
			
			m_glrender = make_shared<GLRender>();
	

			//SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
			//model_vb->upload(test_model->vertices, test_model->indices);
			//model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
			//model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
			//test_model->render_id = m_glrender->addModel(model_vb);

			m_camera = make_shared<GLCamera>(m_view_pos);
			m_render_window->setCamera(m_camera);
			// ifc_test_model->m_model = m_camera->getModelMatrixByBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax());

			// add a rendered model
			SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
			SharedPtr<GLVertexBuffer> select_bbx_vb = make_shared<GLVertexBuffer>();
			if (try_ifc) {
				model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->g_indices);
				model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 10, (void*)0);
				model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 10, (void*)(3 * sizeof(Real)));
				model_vb->vertexAttribDesc(2, 3, sizeof(Real) * 10, (void*)(6 * sizeof(Real)));
				model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 10, (void*)(9 * sizeof(Real)));

				if (use_transparency) {
					model_vb->uploadNoTransElements(ifc_test_model->no_trans_ind);
					model_vb->uploadTransElements(ifc_test_model->trans_ind);
				}
				model_vb->uploadElementBufferOnly(ifc_test_model->c_indices);
				model_vb->UploadElementEdge(ifc_test_model->edge_indices);
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
	}

	void IFCRenderEngine::run()
	{
		if (!m_init) {
			std::cout << "IFC Engine has to 'initialize' !!!" << std::endl;
			return;
		}

		switch (m_render_api) {
		case OPENGL_RENDER_API: {
			auto& m_window = *m_render_window;
			while (!m_window.isClose()) {
				//sleep 1 ms to reduce cpu time
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				m_window.pollEvents();
				m_window.processInput();
				drawFrame();
				m_window.swapBuffer();
			}
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

		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

		{
			m_window.startRenderToWindow();
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
#ifdef TEST_COMP_ID_RES
			m_window.switchRenderCompId();
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, COMP_ID_WRITE, ALL);
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
			m_render.setCompId(m_window.getClickCompId());
			m_render.setHoverCompId(m_window.getHoverCompId());
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING, NO_TRANS);
			//m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING, ALL);

			//2. render transparency scene
			m_render.setAlpha(0.5);
			use_transparency ? m_render.render(ifc_test_model->render_id, TRANSPARENCY_SHADING, TRANS) : void();
			//m_window.readPixels();

			//3. render edges (maybe
			m_render.render(ifc_test_model->render_id, EDGE_SHADING, EDGE_LINE);

			//4. render bounding box
			if (m_window.getClickCompId() >= 0) {
				m_render.ModelVertexUpdate(select_bbx_id, ifc_test_model->generate_bbxs_by_vec({ static_cast<uint32_t>(m_window.getClickCompId()) }));
				m_render.render(select_bbx_id, BOUNDINGBOX_SHADING, BBX_LINE); 
			}
#endif

			// -------------- render axis, not normal render procedure ---------------
			m_render.renderAxis(*ifc_test_model
				, clicked_coord
				, m_camera->getViewPos()
				, m_view_pos);
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
			// -------------- render clipping plane, not normal render procedure ---------------
			m_render.renderClipPlane(m_window.getHidden(), m_window.getClippingPlane());
			// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

			m_window.endRenderToWindow();
		}
		// post render: render edge
		m_render.postRender(m_window);
	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}