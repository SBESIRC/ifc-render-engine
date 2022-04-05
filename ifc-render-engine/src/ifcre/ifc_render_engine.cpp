#include "ifc_render_engine.h"
#include "resource/parser.h"
#include "common/ifc_util.h"

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
		if (try_ifc) {
			ifc_test_model = IFCParser::load(model_file);
		}
		else {
			test_model = DefaultParser::load(model_file);
		}
		m_render_window = make_shared<RenderWindow>("IFC Render", width, height);
		m_glrender = make_shared<GLRender>();
		
		//SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
		//model_vb->upload(test_model->vertices, test_model->indices);
		//model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
		//model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
		//test_model->render_id = m_glrender->addModel(model_vb);

		m_camera = make_shared<GLCamera>(glm::vec3(0.0f, 0.0f, 10.0f));
		m_render_window->setCamera(m_camera);
		// ifc_test_model->m_model = m_camera->getModelMatrixByBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax());
		ifc_test_model->m_model = util::get_model_matrix_byBBX(ifc_test_model->getpMin(), ifc_test_model->getpMax());


		// add a rendered model
		SharedPtr<GLVertexBuffer> model_vb = make_shared<GLVertexBuffer>();
		if (try_ifc) {
			model_vb->upload(ifc_test_model->ver_attrib, ifc_test_model->g_indices);
			model_vb->vertexAttribDesc(0, 3, sizeof(Real) * 6, (void*)0);
			model_vb->vertexAttribDesc(1, 3, sizeof(Real) * 6, (void*)(3 * sizeof(Real)));
			ifc_test_model->render_id = m_glrender->addModel(model_vb);
		}
		else {
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
		m_render.enableTest(DEPTH_TEST);
		m_render.depthFunc(LESS_FUNC);
		GLColor clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };
		m_window.startRenderToWindow(); 
		{
			glm::mat4 view = m_camera->getViewMatrix();
			m_render.setModelViewMatrix(view * ifc_test_model->m_model);
			m_render.setProjectionMatrix(m_window.getProjMatrix());

			//// 0. prev: render normal and depth tex of the scene
			m_window.switchRenderDepthNormal();
			clearValue = { 0.5f,0.5f ,1.0f ,1.0f };
			m_render.clearFrameBuffer((GLClearEnum)(CLEAR_COLOR | CLEAR_DEPTH), &clearValue);
			//m_render.render(test_model->render_id, NORMAL_DEPTH_WRITE);
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, NORMAL_DEPTH_WRITE);

			// 1. render scene
			m_window.switchRenderColor();
			clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };
			m_render.clearFrameBuffer((GLClearEnum)(CLEAR_COLOR | CLEAR_DEPTH), &clearValue);
			m_render.render(try_ifc ? ifc_test_model->render_id : test_model->render_id, DEFAULT_SHADING);



		}
		// post render
		m_window.endRenderToWindow();
		m_render.disableTest(DEPTH_TEST);
		// render edge
		//m_render.postRender(m_window.getColorTexId(), m_window.getDepthNormalTexId());
		m_render.postRender(m_window);

	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}