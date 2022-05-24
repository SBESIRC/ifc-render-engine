#include "render_ui.h"
#include<functional>
#include<GLFW/glfw3.h>
#include<iostream>
namespace ifcre {
	void RenderUI::onWindowResize(int width, int height)
	{
		m_editCamera->setAspect((float)width / (float)height);
	}
	void RenderUI::onMouseButton(int button, int action, int mods)
	{
		auto& camera = *m_editCamera;
		auto& status = m_mouseStatus;
		double click_x, click_y;
		int32_t tx, ty;
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			switch (action) {
			case GLFW_PRESS: {
				glfwGetCursorPos(m_io.window, &click_x, &click_y);
				tx = click_x;
				//ty = m_io.m_surfaceIO->getHeight() - click_y - 1;
				ty = click_y;
				float depth = m_render->getDepthValue(tx, ty);
				printf("depth: %f\n", depth);
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
	void RenderUI::registerInput()
	{
		auto& surface_io = *(m_io.m_surfaceIO);
		surface_io.registerWindowResizeCallback(std::bind(&RenderUI::onWindowResize, this, std::placeholders::_1, std::placeholders::_2));
		surface_io.registerMouseButtonCallback(std::bind(&RenderUI::onMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		//surface_io.registerCursorPosCallback();
		//surface_io.registerScrollCallback();
	}
}