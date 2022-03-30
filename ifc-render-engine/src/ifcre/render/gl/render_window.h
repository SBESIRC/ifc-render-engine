#pragma once
#ifndef IFCRE_RENDER_WINDOW_H_
#define IFCRE_RENDER_WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>
#include "../../common/std_types.h"
namespace ifcre {

	class RenderWindow {
	public:
		RenderWindow(const char* title, int w, int h, bool vsync = false);
		~RenderWindow();

		void processInput();

		bool isClose();

		void swapBuffer();
		void pollEvents();

		void bind();
		void unbind();
		void recreateFramebuffer(int w, int h);

		uint32_t getColorTexId();

		Real zoom_parameter;
	private:

		void createFramebuffer(int w, int h);

		struct {
			uint32_t fbo_id;
			uint32_t colortex_id;
			uint32_t rbo_id;
		} m_framebuffer;

        // memory management by GLFW
        GLFWwindow* m_window;

	};

}

#endif