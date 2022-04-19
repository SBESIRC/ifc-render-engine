#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"

//#define ONLY_DEPTH_NROMAL_RES
//#define TEST_COMP_ID_RES

namespace ifcre {
	SharedPtr<IFCRenderEngine> ifcre;

	void IFCRenderEngine::initialize(Map<String, String> &configs)
	{
		if (m_init) {
			return;
		}
		
		int width = atoi(configs["width"].c_str());
		int height = atoi(configs["height"].c_str());
		String model_file = configs["file"];
		try_ifc = configs["model_type"] == "ifc";
		use_transparency = configs["use_transparency"] == "true";
		if (try_ifc) {
			ifc_test_model = IFCParser::load(model_file);
		}
		else {
			test_model = DefaultParser::load(model_file);
		}
		//generateIFCMidfile("resources\\models\\ifc_midfile\\newIFC.ifc", 0.01);
		m_render_window = make_shared<RenderWindow>("IFC Render", width, height, true);
		m_glrender = make_shared<GLRender>();
		
		//SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
		//model_vb->upload(test_model->vertices, test_model->indices);
		//model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
		//model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
		//test_model->render_id = m_glrender->addModel(model_vb);

		m_camera = make_shared<GLCamera>(m_view_pos);
		m_render_window->setCamera(m_camera);
		// ifc_test_model->m_model = m_camera->getModelMatrixByBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax());
		Real scale_factor = 0;
		glm::mat4 ifc_model_matrix;
		util::get_model_matrix_byBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax(), ifc_model_matrix, scale_factor);
		ifc_test_model->setModelMatrix(ifc_model_matrix);
		ifc_test_model->setScaleFactor(scale_factor);

		// add a rendered model
		SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
		SharedPtr<GLVertexBuffer> no_trans_model_vb = make_shared<GLVertexBuffer>();
		SharedPtr<GLVertexBuffer> trans_model_vb = make_shared<GLVertexBuffer>();
		if (try_ifc) {
			model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->g_indices);
			model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 10, (void*)0);
			model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 10, (void*)(3 * sizeof(Real)));
			model_vb->vertexAttribDesc(2, 3, sizeof(Real) * 10, (void*)(6 * sizeof(Real)));
			model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 10, (void*)(9 * sizeof(Real)));
			
			if (use_transparency) {
				/*no_trans_model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->no_trans_ind);
				no_trans_model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 10, (void*)0);
				no_trans_model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 10, (void*)(3 * sizeof(Real)));
				no_trans_model_vb->vertexAttribDesc(2, 3, sizeof(Real) * 10, (void*)(6 * sizeof(Real)));
				no_trans_model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 10, (void*)(9 * sizeof(Real)));
				no_transparency_id = m_glrender->addModel(no_trans_model_vb);

				trans_model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->trans_ind);
				trans_model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 10, (void*)0);
				trans_model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 10, (void*)(3 * sizeof(Real)));
				trans_model_vb->vertexAttribDesc(2, 3, sizeof(Real) * 10, (void*)(6 * sizeof(Real)));
				trans_model_vb->vertexAttribDesc(3, 1, sizeof(Real) * 10, (void*)(9 * sizeof(Real)));
				transparency_id = m_glrender->addModel(trans_model_vb);*/
				model_vb->uploadNoTransElements(ifc_test_model->no_trans_ind);
				model_vb->uploadTransElements(ifc_test_model->trans_ind);
			}
			model_vb->uploadElementBufferOnly(ifc_test_model->c_indices);
			ifc_test_model->render_id = m_glrender->addModel(model_vb);
		}
		else
		{
			model_vb->upload(test_model->vertices, test_model->indices);
			model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
			model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
			test_model->render_id = m_glrender->addModel(model_vb);
		}
		m_init = true;
	}

	void IFCRenderEngine::run()
	{
		if (!m_init) {
			std::cout << "IFC Engine has to 'initialize' !!!" << std::endl;
			return;
		}

		auto& m_window = *m_render_window;
		while (!m_window.isClose()) {
			m_window.pollEvents();
			m_window.processInput();

			drawFrame();

			m_window.swapBuffer();
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
		auto model_matrix = ifc_test_model->getModelMatrix();

		// -------------- ifc model transform by mouse ---------------
		glm::vec3 clicked_coord = m_window.getClickedWorldCoord();
		if (m_window.isMouseHorizontalRot()) {
			float angle = m_window.getMouseHorizontalVel();
			ifc_test_model->rotateInLocalSpace(clicked_coord, angle);
		}
		if (m_window.isMouseVerticalRot()) {
			float angle = m_window.getMouseVerticalVel();
			ifc_test_model->rotateInWorldSpace(clicked_coord, angle);
		}

		if (m_window.isMouseMove()) {
			glm::vec3 hover = m_window.getHoverWorldCoord();
			glm::vec3 step = hover - m_last_hover_pos;
			ifc_test_model->translate(step);

			m_last_hover_pos = hover;
		}
		else {
			m_last_hover_pos = clicked_coord;
		}

		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

		{
			m_window.startRenderToWindow();
			glm::mat4 view = m_camera->getViewMatrix();
			m_render.setViewMatrix(view);
			m_render.setModelMatrix(model_matrix);
			m_render.setModelViewMatrix(view * model_matrix);
			m_render.setProjectionMatrix(m_window.getProjMatrix());
			m_render.setAlpha(1.0);

#ifdef TEST_COMP_ID_RES
			//m_window.switchRenderCompId();
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, COMP_ID_WRITE);
			//m_render.render(transparency_id, COMP_ID_WRITE);
#endif

			//// 0. prev: render normal and depth tex of the scene
#ifndef ONLY_DEPTH_NROMAL_RES
			m_window.switchRenderDepthNormal();
#endif
#ifndef TEST_COMP_ID_RES
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, NORMAL_DEPTH_WRITE);
#endif

#ifndef ONLY_DEPTH_NROMAL_RES
			//// 1. render scene
			m_window.switchRenderColor();
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING, NO_TRANS);

			////2. render transparency scene
			m_render.setAlpha(0.5);
			use_transparency ? m_render.render(ifc_test_model->render_id, TRANSPARENCY_SHADING, TRANS) : void();
			//m_window.readPixels();
#endif

			m_window.endRenderToWindow();
		}
		// post render: render edge
		m_render.postRender(m_window);

		// -------------- render axis, not normal render procedure ---------------
		m_render.renderAxis(*ifc_test_model
			, clicked_coord
			, m_camera->getViewPos()
			, m_view_pos);
		// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}