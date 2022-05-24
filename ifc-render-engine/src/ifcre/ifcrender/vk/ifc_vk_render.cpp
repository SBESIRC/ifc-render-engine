#include "../ifc_render.h"
#include "../render_ui.h"
namespace ifcre {
// interface:
	void IFCVulkanRender::initialize(int32_t w, int32_t h){
		m_surfaceIO = std::make_shared<SurfaceIO>("IFC Render", w, h, VULKAN_RENDER_API);
		m_renderUI = std::make_shared<RenderUI>();
		m_renderUI->initialize(this);
		m_vkManager.initialize(m_surfaceIO->getWindowPtr());

		m_init = true;
	}

	bool IFCVulkanRender::render(Scene& scene) {
#ifdef _DEBUG
		if (!m_init)return false;
#endif
		static bool first_draw = true;
		if (first_draw) {
			IFCModelPayload payload;
			auto& m = *(scene.m_ifcObject);
			payload.vertices = &m.ver_attrib;
			payload.g_indices = &m.g_indices;
			payload.opaque_indices = &m.no_trans_ind;
			payload.transparency_indices = &m.trans_ind;
			payload.edge_indices = &m.edge_indices;
			m.render_id = m_vkManager.addIFCMesh(payload);
			first_draw = false;
		}

		bool flag = false;
		if (flag = m_surfaceIO->notExit()) {
			scene.m_editCamera = m_renderUI->getEditCamera().get();
			m_curScene = &scene;
			m_vkManager.renderFrame(scene);
		}

		return flag;
	}
	void IFCVulkanRender::updateWindow(int32_t x, int32_t y, int32_t w, int32_t h)
	{

	}
	float IFCVulkanRender::getDepthValue(int32_t x, int32_t y)
	{
		if (m_curScene == nullptr) {
			return 1.0f;
		}
		return m_vkManager.getDepthValue(*m_curScene, x, y);
	}
// ----- ----- ----- ----- ----- ----- ----- -----
}