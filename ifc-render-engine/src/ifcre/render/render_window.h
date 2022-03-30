#pragma once
#ifndef IFCRE_RENDER_WINDOW_H_
#define IFCRE_RENDER_WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/std_types.h"
#include "gl/gl_render_texture.h"
#include "gl_camera.h"
namespace ifcre {

	class RenderWindow {
	public:
		RenderWindow(const char* title, int w, int h, bool vsync = false);
		~RenderWindow();

		void processInput();

		bool isClose();

		void swapBuffer();
		void pollEvents();

		void startRenderToWindow();
		void endRenderToWindow();

		void switchRenderDepthNormal();
		void switchRenderColor();

		void recreateFramebuffer(int w, int h);

		uint32_t getColorTexId();

		void setCamera(SharedPtr<GLCamera> camera);
	private:

		void createFramebuffer(int w, int h);

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_default_rt;
			SharedPtr<GLRenderTexture> m_depth_normal_rt;
		} m_framebuffer;

		SharedPtr<GLCamera> m_camera;
		uint32_t m_cur_fbo;

        // memory management by GLFW
        GLFWwindow* m_window;
	private:
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	};

}

#endif