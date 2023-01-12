#pragma once
#ifndef _IFCRE_SURFACE_UI_H_
#define _IFCRE_SURFACE_UI_H_

#include "surface_io.h"
#include "ifc_render.h" 
#include "ifc_camera.h"
namespace ifcre {
	class IFCRender;
	class IFCVulkanRender;
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
			m_renderAPIEnum = vk_render != nullptr ? RenderAPIEnum::VULKAN_RENDER_API : RenderAPIEnum::OPENGL_RENDER_API;
		}
		std::shared_ptr <IFCCamera> getEditCamera() { return m_editCamera; }
	public:
		glm::vec3 getClickedWorldCoord() { return m_mouseStatus.click_world_center; }
		float getMouseHorizontalVel() { return m_option.mouse_hori_vel * m_mouseStatus.horizontal_move;  }
		float getMouseVerticalVel() { return m_option.mouse_vert_vel * m_mouseStatus.vertical_move; }
		bool isMouseHorizontalRot() { return m_mouseStatus.lbtn_down && !m_mouseStatus.rbtn_down && m_mouseStatus.horizontal_move != 0; }
		bool isMouseVerticalRot() { return m_mouseStatus.lbtn_down && !m_mouseStatus.rbtn_down && m_mouseStatus.vertical_move != 0; }
		bool isRightMouseClicked() { return m_mouseStatus.rbtn_down; }
		bool isMouseMove() { return (m_mouseStatus.horizontal_move != 0 || m_mouseStatus.vertical_move != 0); }
		glm::vec3 getVirtualHoverWorldCoord() { return m_mouseStatus.hover_world_center; }

		int32_t getClickedCompId() { return m_mouseStatus.click_comp_id; }
		int32_t getHoveredCompId() { return m_mouseStatus.hover_comp_id; }

		void reset() {
			glfwSwapBuffers(m_io.window);
			double now_time = glfwGetTime();
			m_delta_time = now_time - m_last_time;
			m_last_time = now_time;
			m_hover_time += m_delta_time;

			m_mouseStatus.horizontal_move = 0;
			m_mouseStatus.vertical_move = 0;
		}
	private:
		std::shared_ptr<IFCCamera> m_editCamera;
		struct {
			bool lbtn_down = false, rbtn_down = false;
			int32_t horizontal_move = 0, vertical_move = 0;
			int32_t last_mouse_x, last_mouse_y;
			glm::vec3 click_world_center = glm::vec3(0, 0, 0);
			glm::vec3 hover_world_center = glm::vec3(0, 0, 0);
			int32_t click_x = 0, click_y = 0;
			float click_depth = 1.0;
			// 1: inside of model
			// -1: outside of model
			int32_t click_init_mask = 0;
			int32_t click_comp_id = -1;
			int32_t hover_comp_id = -1;
		}m_mouseStatus;

		struct {
			float mouse_hori_vel = 0.015;
			float mouse_vert_vel = 0.01;
		}m_option;

		// -------- render time --------
		float m_last_time = 0, m_delta_time = 0;
		float m_hover_time = 0;
		float m_move_time = 0;
		// ----- ----- ----- ----- -----
	private:
		void onWindowResize(int width, int height);
		void onMouseButton(int button, int action, int mods);
		void onCursorPos(double xpos, double ypos);
		void onScroll(double xoffset, double yoffset);
		void registerInput();
		void _setClickedWorldCoords(double click_x, double click_y, double click_z);
		struct {
			SurfaceIO* m_surfaceIO;
			GLFWwindow* window;
		}m_io;
		IFCRender* m_render;

		RenderAPIEnum m_renderAPIEnum;
	};
}

#endif
