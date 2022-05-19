#include "../ifc_render.h"

namespace ifcre {
// interface:
	void IFCVulkanRender::initialize(int32_t w, int32_t h){
		m_surfaceIO = std::make_shared<SurfaceIO>("IFC Render", w, h, VULKAN_RENDER_API);
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
			m_vkManager.renderFrame(scene);
		}

		return flag;
	}
// ----- ----- ----- ----- ----- ----- ----- -----
}