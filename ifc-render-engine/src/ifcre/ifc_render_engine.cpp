#include "ifc_render_engine.h"

namespace ifcre {
	SharedPtr<IFCRenderEngine> ifcre;

	void IFCRenderEngine::initialize(Map<String, String> &configs)
	{
		if (m_init) {
			return;
		}
		
		int width = atoi(configs["width"].c_str());
		int height = atoi(configs["height"].c_str());
		
		m_glrender = std::make_shared<GLRender>();
		m_render_window = std::make_shared<RenderWindow>("IFC Render", width, height);

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

			render();

			m_window.swapBuffer();
		}
	}

	SharedPtr<RenderEngine> IFCRenderEngine::getSingleton()
	{
		if (ifcre.get() == nullptr) {
			ifcre = std::make_shared<IFCRenderEngine>();
		}
		return ifcre;
	}

// private:
	void IFCRenderEngine::render()
	{
		auto& m_render = *m_glrender;
		auto& m_window = *m_render_window;
		GLColor clearValue = { 0.2f, 0.3f, 0.3f, 1.0f };
		//m_window.bind();


		m_render.clearFrameBuffer((GLClearEnum)(CLEAR_COLOR | CLEAR_DEPTH),  &clearValue);

		//m_window.unbind();

	}
// ----- ----- ----- ----- ----- ----- ----- ----- 
}