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
		scene.m_editCamera = m_renderUI->getEditCamera().get();
		
		m_curScene = &scene;

		if (flag = m_surfaceIO->notExit()) {
			auto& ui = *m_renderUI;
			auto& ifc_model = *scene.m_ifcObject;

			// user interaction
			glm::vec3 clicked_coord = ui.getClickedWorldCoord();
			scene.m_pickWorldPos = clicked_coord;
			//printf("%f %f %f\n", clicked_coord.x, clicked_coord.y, clicked_coord.z);
			if (ui.isMouseHorizontalRot()) {
				float angle = ui.getMouseHorizontalVel();
				ifc_model.rotateInLocalSpace(clicked_coord, angle);
			}
			if (ui.isMouseVerticalRot()) {
				float angle = ui.getMouseVerticalVel();
				ifc_model.rotateInWorldSpace(clicked_coord, angle);
			}

			if (ui.isRightMouseClicked()) {
				if (ui.isMouseMove() && m_last_rmclick) {
					glm::vec3 hover = ui.getVirtualHoverWorldCoord();
					glm::vec3 step = hover - m_last_hover_pos;
					ifc_model.translate(step);
				}
				m_last_hover_pos = clicked_coord;
				m_last_rmclick = true;
			}
			else {
				m_last_rmclick = false;
			}
			// ----- ----- ----- ----- ----- ----- ----- 

			m_vkManager.renderFrame(scene);

			ui.reset();
		}

		return flag;
	}
	void IFCVulkanRender::updateWindow(int32_t x, int32_t y, int32_t w, int32_t h)
	{
		// TODO
	}
	float IFCVulkanRender::getDepthValue(int32_t x, int32_t y)
	{
		auto& surface_io = *m_surfaceIO;
		if (m_curScene == nullptr 
				|| (x < 0 || y < 0 || x >= surface_io.getWidth() || y >= surface_io.getHeight()))
		{
			return 2.0f;
		}
		return m_vkManager.getDepthValue(*m_curScene, x, y);
	}
// ----- ----- ----- ----- ----- ----- ----- -----
}