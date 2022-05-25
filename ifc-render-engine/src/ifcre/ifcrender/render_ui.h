#pragma once
#ifndef _IFCRE_SURFACE_UI_H_
#define _IFCRE_SURFACE_UI_H_

#include "surface_io.h"
#include "ifc_render.h"
#include "ifc_camera.h"
namespace ifcre {
	//class IFCRender;
	//class IFCVulkanRender;
	class RenderUI {
	public:
		void initialize(IFCRender* render) {
			m_render = render;
			m_io.m_surfaceIO = render->getSurfaceIO();
			m_io.window = m_io.m_surfaceIO->getWindowPtr();
			registerInput();
			IFCVulkanRender *vk_render = dynamic_cast<IFCVulkanRender*>(render);
			m_editCamera = make_shared<IFCCamera>(glm::vec3(0, 0, 10)
				, glm::vec3(0.0f, 1.0f, 0.0f)
				, 45.0f
				, (float)m_io.m_surfaceIO->getWidth() / (float)m_io.m_surfaceIO->getHeight()
				, 0.1f, 1000.0f
				, vk_render != nullptr);
		}
		std::shared_ptr <IFCCamera> getEditCamera() { return m_editCamera; }

	private:
		std::shared_ptr<IFCCamera> m_editCamera;
		struct {
			bool lbtn_down = false, rbtn_down = false;
			int32_t horizontal_move = 0, vertical_move = 0;
			int32_t last_mouse_x, last_mouse_y;
			glm::vec3 click_world_center = glm::vec3(0, 0, 0);
			int32_t click_x = 0, click_y = 0;
			float click_depth = 1.0;
			// 1: inside of model
			// -1: outside of model
			int32_t click_init_mask = 0;
		}m_mouseStatus;
	private:
		void onWindowResize(int width, int height);
		void onMouseButton(int button, int action, int mods);
		void onCursorPos(double xpos, double ypos);
		void registerInput();
		void _setClickedWorldCoords(double click_x, double click_y, double click_z);
		struct {
			SurfaceIO* m_surfaceIO;
			GLFWwindow* window;
		}m_io;
		IFCRender* m_render;
	};
}

#endif