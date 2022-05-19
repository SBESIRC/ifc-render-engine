#pragma once
#ifndef _IFCRE_SURFACE_IO_H_
#define _IFCRE_SURFACE_IO_H_
#include <functional>

#include <GLFW/glfw3.h>
#include "../common/ifc_enum.h"
#include "../common/std_types.h"

namespace ifcre {
	class SurfaceIO {
		typedef std::function<void(int, int)> onWindowResizeFunc;
		typedef std::function<void(double, double)> onCursorPosFunc;
		typedef std::function<void(double, double)> onScrollFunc;
		typedef std::function<void(int, int, int)> onMouseButtonFunc;
	public:
		SurfaceIO(const char* title, int32_t w, int32_t h, RenderAPIEnum api);
		~SurfaceIO();

		bool notExit() {
			if (!glfwWindowShouldClose(m_window)) {
				glfwPollEvents();
				return true;
			}
			return false;
		}

		void setWidth(int32_t w) { m_width = w; }
		void setHeight(int32_t h) { m_height = h; }
		GLFWwindow* getWindowPtr() const { return m_window; }
		
		void registerWindowResizeCallback(onWindowResizeFunc func) { m_windowResizeFuncs.push_back(func); }
		void registerCursorPosCallback(onCursorPosFunc func) { m_cursorPosFuncs.push_back(func); }
		void registerScrollCallback(onScrollFunc func) { m_scrollFuncs.push_back(func); }
		void registerMouseButtonCallback(onMouseButtonFunc func) { m_mouseButtonFuncs.push_back(func); }

		void onWindowResizeCallback(int width, int height) {
			for (auto& func : m_windowResizeFuncs) {
				func(width, height);
			}
		}
		void onCursorPosCallback(double xpos, double ypos) {
			for (auto& func : m_cursorPosFuncs) {
				func(xpos, ypos);
			}
		}
		void onScrollCallback(double xoffset, double yoffset) {
			for (auto& func : m_scrollFuncs) {
				func(xoffset, yoffset);
			}
		}
		void onMouseButtonCallback(int button, int action, int mods) {
			for (auto& func : m_mouseButtonFuncs) {
				func(button, action, mods);
			}
		}
	private:
		std::vector<onWindowResizeFunc> m_windowResizeFuncs;
		std::vector<onCursorPosFunc> m_cursorPosFuncs;
		std::vector<onScrollFunc> m_scrollFuncs;
		std::vector<onMouseButtonFunc> m_mouseButtonFuncs;

		GLFWwindow* m_window;
		int32_t m_width, m_height;
		std::string m_title;
		RenderAPIEnum m_render_api;
	};


}

#endif