#pragma once
#ifndef IFCRE_RENDER_WINDOW_H_
#define IFCRE_RENDER_WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/std_types.h"
#include "gl/gl_render_texture.h"
#include "gl_camera.h"
#include <glm/glm.hpp>
namespace ifcre {

	class RenderWindow {
	public:
		RenderWindow(const char* title, int32_t w, int32_t h, bool vsync = false);
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
		uint32_t getDepthNormalTexId();
		glm::vec2 getWindowSize();
		glm::mat4 getProjMatrix();

		void setCamera(SharedPtr<GLCamera> camera);
	private:

		void createFramebuffer(int w, int h);

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_default_rt;
			SharedPtr<GLRenderTexture> m_depth_normal_rt;
		} m_framebuffer;
		// current render texture using by this render window
		GLRenderTexture* m_cur_rt;

		// refer to camera of ifc engine
		SharedPtr<GLCamera> m_camera;
		uint32_t m_cur_fbo;

        // memory management by GLFW
        GLFWwindow* m_window;

		int32_t m_width, m_height;
		glm::mat4 m_projection;

		struct {
			int32_t last_mouse_x, last_mouse_y;
			bool lbtn_down = false, rbtn_down = false;
			glm::vec3 click_world_center;
		}m_status;
		const Real m_znear = 0.1, m_zfar = 1000.0;
		const Real fov = 45.0;
	private:
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_button_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	};

}

#endif