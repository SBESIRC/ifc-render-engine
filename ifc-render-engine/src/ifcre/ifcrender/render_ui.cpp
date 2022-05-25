#include "render_ui.h"
#include<functional>
#include<GLFW/glfw3.h>
#include<iostream>
namespace ifcre {

	void RenderUI::_setClickedWorldCoords(double click_x, double click_y, double click_z)
	{
		auto& camera = *m_editCamera;
		Real w = m_io.m_surfaceIO->getWidth(), h = m_io.m_surfaceIO->getHeight();;
		// from [0, 1] to [-1, 1]
		Real y = (h - click_y - 1) / h * 2 - 1;
		Real x = click_x / w * 2 - 1;
		Real z = click_y * 2 - 1;
		m_mouseStatus.click_depth = z;
		glm::vec4 ndc(x, y, z, 1.0f);
		glm::mat4 vp_inv = glm::inverse(camera.getProjMatrix() * camera.getViewMatrix());
		glm::vec4 t = vp_inv * ndc;
		t = t / t.w;
		m_mouseStatus.click_world_center = t;
		m_mouseStatus.click_x = click_x;
		m_mouseStatus.click_y = click_y;
	}

	void RenderUI::onWindowResize(int width, int height)
	{
		m_editCamera->setAspect((float)width / (float)height);
	}
	void RenderUI::onMouseButton(int button, int action, int mods)
	{
		auto& camera = *m_editCamera;
		auto& status = m_mouseStatus;
		double click_x, click_y;

		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			switch (action) {
			case GLFW_PRESS: {
				glfwGetCursorPos(m_io.window, &click_x, &click_y);
				float click_depth = m_render->getDepthValue(click_x, click_y);
				if (click_depth != 1.0) {
					_setClickedWorldCoords(click_x, click_y, click_depth);
				}
				status.lbtn_down = true;
				break;
			}
			case GLFW_RELEASE: {
				status.lbtn_down = false;
				break;
			}
			}
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			switch (action) {
			case GLFW_PRESS: {
				glfwGetCursorPos(m_io.window, &click_x, &click_y);
				float click_depth = m_render->getDepthValue(click_x, click_y);
				if (click_depth != 1.0) {
					_setClickedWorldCoords(click_x, click_y, click_depth);
					m_mouseStatus.click_init_mask = 1;
				}
				else {
					m_mouseStatus.click_init_mask = -1;
				}
				status.rbtn_down = true;
				break;
			}
			case GLFW_RELEASE: {
				status.rbtn_down = false;
				break;
			}
			}
		}
	}

	void RenderUI::onCursorPos(double xpos, double ypos)
	{
		auto& camera = *m_editCamera;
		auto& status = m_mouseStatus;

		if (status.lbtn_down || status.rbtn_down) {
			if (status.last_mouse_x != xpos) {
				status.horizontal_move = xpos - status.last_mouse_x;

			}
			if (status.last_mouse_y != ypos) {
				status.vertical_move = ypos - status.last_mouse_y;
			}
		}
	}

	void RenderUI::registerInput()
	{
		auto& surface_io = *(m_io.m_surfaceIO);
		surface_io.registerWindowResizeCallback(std::bind(&RenderUI::onWindowResize, this, std::placeholders::_1, std::placeholders::_2));
		surface_io.registerMouseButtonCallback(std::bind(&RenderUI::onMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		surface_io.registerCursorPosCallback(std::bind(&RenderUI::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
		//surface_io.registerScrollCallback();
	}
}