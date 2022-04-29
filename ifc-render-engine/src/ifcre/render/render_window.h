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
		RenderWindow(const char* title, int32_t w, int32_t h, bool aa = true, bool vsync = false);
		~RenderWindow();

		void processInput();

		bool isClose();

		void swapBuffer();
		void pollEvents();

		void startRenderToWindow();
		void endRenderToWindow();

		void switchRenderCompId();
		void switchRenderBack();
		void switchRenderDepthNormal();
		void switchRenderColor();

		void recreateFramebuffer(int w, int h);

		void readPixels();

		uint32_t getFBOId();
		uint32_t getColorTexId();
		uint32_t getDepthNormalTexId();
		int getClickCompId();
		int getHoverCompId();
		glm::vec2 getWindowSize();
		glm::mat4 getProjMatrix();

		void setCamera(SharedPtr<GLCamera> camera);
		
		// --------- mouse status -----------
		glm::vec3 getClickedWorldCoord();
		glm::vec3 getVirtualHoverWorldCoord();
		float getMouseHorizontalVel();
		float getMouseVerticalVel();
		bool isMouseHorizontalRot();
		bool isMouseVerticalRot();
		bool isMouseMove();
		bool isRightMouseClicked();

		// ----- ----- ----- ----- ----- -----

		// ---------- 


	private:

		void createFramebuffer(int w, int h);

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_default_rt;
			SharedPtr<GLRenderTexture> m_comp_id_rt;
			SharedPtr<GLRenderTexture> m_depth_normal_rt;
		} m_framebuffer;
		
		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_msaa_rt;
		} m_msaa_fb;

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_comp_id_rt;
		}m_comp_fb;
		
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
			int32_t horizontal_move = 0, vertical_move = 0;
			int32_t last_mouse_x, last_mouse_y;
			bool lbtn_down = false, rbtn_down = false;
			glm::vec3 click_world_center = glm::vec3(0, 0, 0);
			glm::vec3 hover_world_center = glm::vec3(0, 0, 0);
			glm::vec3 click_world_color = glm::vec3(0, 0, 0);
			int32_t click_x = 0, click_y = 0;
			Real click_z = 1.0;
			int click_comp_id = -1;
			int hover_comp_id = -1;
		}m_mouse_status;

		struct {
			Real mouse_hori_vel = 0.015;
			Real mouse_vert_vel = 0.01;

			bool anti_aliasing = true;
		}m_option;

		// -------- render time --------
		double m_last_time = 0, m_delta_time = 0;
		// ----- ----- ----- ----- -----

		const Real m_znear = 0.1, m_zfar = 1000.0;
		const Real fov = 45.0;
	private:
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_button_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

// --------------------- event helper ----------------------
	private:
		void _setClickedWorldCoords(double clicked_x, double clicked_y, double clicked_z);
		float _getClickedDepthValue(double clicked_x, double clicked_y);
		void _setClickedWorldColors(double click_x, double click_y, bool hover_mode);
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	};

}
#endif