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
		}m_mouseStatus;
	private:
		void onWindowResize(int width, int height);
		void onMouseButton(int button, int action, int mods);
		void registerInput();
		struct {
			SurfaceIO* m_surfaceIO;
			GLFWwindow* window;
		}m_io;
		IFCRender* m_render;
	};
}

#endif