#include "ifc_render_engine.h"
#include "resource/parser.h"

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
		GLColor clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };
		m_window.bind();
		m_render.clearFrameBuffer((GLClearEnum)(CLEAR_COLOR | CLEAR_DEPTH),  &clearValue);
		
		if (try_ifc) {
			m_render.render(ifc_test_model->render_id, ifc_test_model->getBBX(), m_window.zoom_parameter);
		}
		else {
			m_render.render(test_model->render_id);
		}
		
		// post
		m_window.unbind();
		m_render.disableTest(DEPTH_TEST);
		m_render.postRender(m_window.getColorTexId());

	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}