#include <glad/glad.h>
#include "surface_io.h"
#include <iostream>
namespace ifcre {
// -------------------------- event processing --------------------------------
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		SurfaceIO* that = (SurfaceIO*)glfwGetWindowUserPointer(window);
		that->setWidth(width);
		that->setHeight(height);
		that->onWindowResizeCallback(width, height);
	}

	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
		SurfaceIO* that = (SurfaceIO*)glfwGetWindowUserPointer(window);
		that->onCursorPosCallback(xpos, ypos);
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		SurfaceIO* that = (SurfaceIO*)glfwGetWindowUserPointer(window);
		that->onScrollCallback(xoffset, yoffset);
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		SurfaceIO* that = (SurfaceIO*)glfwGetWindowUserPointer(window);
		that->onMouseButtonCallback(button, action, mods);
	}
// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

	SurfaceIO::SurfaceIO(const char* title, int32_t w, int32_t h, RenderAPIEnum api)
		:m_title(title), m_width(w), m_height(h), m_render_api(api)
	{
		glfwInit();
		if (api == RenderAPIEnum::VULKAN_RENDER_API) {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			m_window = glfwCreateWindow(w, h, title, NULL, NULL);
		}
		else if(api == RenderAPIEnum::OPENGL_RENDER_API){
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		if (m_window == NULL) {
			std::cout << "Failed to create GLFW window\n";
			glfwTerminate();
			return; 
		}
		if (api == RenderAPIEnum::OPENGL_RENDER_API) {
			glfwMakeContextCurrent(m_window);
			// load gl functions by glad
			static bool load_gl = false;
			if (!load_gl && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				load_gl = true;
				std::cout << "Failed to initialize GLAD" << std::endl;
			}
		}

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
		glfwSetCursorPosCallback(m_window, cursor_pos_callback);
		glfwSetScrollCallback(m_window, scroll_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
	}

	SurfaceIO::~SurfaceIO() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

}
