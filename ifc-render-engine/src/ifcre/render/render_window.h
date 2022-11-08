#pragma once
#ifndef IFCRE_RENDER_WINDOW_H_
#define IFCRE_RENDER_WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>
#include <set>
#include <chrono>
#include "../common/std_types.h"
#include "gl/gl_render_texture.h"
#include "gl_camera.h"
#include "../common/ifc_util.h"
#include "../resource/model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ifcre {
	struct DrawingMatchPlane {
	public:
		glm::vec3 normal;
		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 base_pos;
		glm::mat4 cur_model_mat;
		glm::vec3 model_center;
		DrawingMatchPlane() {}
		~DrawingMatchPlane() {}
		DrawingMatchPlane(glm::vec4 p) {
			normal = p;
			base_pos = glm::vec3(0.);
			front = glm::vec3(0., 0., -1.);
			right = glm::normalize(glm::cross(front, normal));
		}

		void operator+=(glm::vec4 p)
		{
			normal += glm::vec3(p);
		}

		void bind_the_world_coordination(glm::mat4 model_mat) {
			cur_model_mat = model_mat;
		}

		void get_center(glm::vec3 center)
		{
			model_center = center;
		}
		glm::vec4 to_vec4()
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, model_center);
			return model * glm::vec4(normal, 1.0);
		}
	};

	class RenderWindow {
	public:
		RenderWindow(const char* title, int32_t w, int32_t h, bool aa = true, bool vsync = false , GLFWwindow* wndPtr = NULL);
		~RenderWindow();

		void processInput();

		bool isClose();

		bool swapBuffer();
		void pollEvents();

		void offScreenRender();
		void endOffScreenRender();

		void startRenderToWindow();
		void endRenderToWindow();

		void switchRenderCompId();
		void switchRenderUI();
		void switchRenderBack();
		void switchRenderDepthNormal();
		void switchRenderColor();

		void recreateFramebuffer(int w, int h);

		void readPixels();
		bool  SaveImage(const char* imgpath, const int width, const int height);
		uint32_t getFBOId();
		uint32_t getColorTexId();
		uint32_t getDepthNormalTexId();
		uint32_t getAerialColorTexId() { return m_arerial_fb.m_AerialView_rt->getTexId(); }
		int getClickCompId();
		int getHoverCompId();
		int getClickedUIId();
		int getClpBoxFaceId();
		glm::vec2 getWindowSize();
		glm::mat4 getProjMatrix(bool _isperspective);
		glm::mat4 getPerspectiveProjMatrix();
		glm::mat4 getOrthoProjMatrix() { m_projection = ortho_projection; isperspective = false; return ortho_projection; }
		bool getHidden() { return hidden; }
		Vector<glm::vec4> getClippingBoxVectors(bool _hidden);
		void setCamera(SharedPtr<GLCamera> camera);
		void setIfcModel(SharedPtr<IFCModel> ifcModel);
		int get_width() { return m_width; }
		int get_height() { return m_height; }
		bool getShowDrawing() { return showDrawing; }	// drawing match shader
		glm::vec4 getDrawingPlane(bool _show);

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

		String get_glsl_verison() const {
			return glsl_version;
		}
		GLFWwindow* get_glfw_window() const {
			return m_window;
		}


		// ---------- 
		//int geomframe = 0;
		bool geom_changed = true;
		bool chosen_changed_w = false;
		bool chosen_changed_x = false;
		void setDefaultStatus();
		bool rotatelock = false;
		bool trigger = false;
		bool to_show_grid = true;

		DrawingMatchPlane drawing_plane = glm::vec4(0.f, 0.f, 0.f, 1.f);
	private:

		void createFramebuffer(int w, int h);
		// 不同的framebuffer对应不同的fbo_id
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
			SharedPtr<GLRenderTexture> m_AerialView_rt;
		} m_arerial_fb;

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_comp_id_rt;
		}m_comp_fb;

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_ui_id_rt;
		}m_ui_fb;

		// current render texture using by this render window
		GLRenderTexture* m_cur_rt;

		// refer to camera of ifc engine
		SharedPtr<GLCamera> m_camera;

		//refer to ifc model
		SharedPtr<IFCModel> ifc_model;

		uint32_t m_cur_fbo;

		// memory management by GLFW
		GLFWwindow* m_window;
		String glsl_version;

		int32_t m_width, m_height;
		glm::mat4 perspective_projection;
		//for a little screen
		int32_t m_miniwidth, m_miniheight;
		glm::mat4 ortho_projection;

		glm::mat4 m_projection;//the projection used currently
		bool isperspective = true;

		public:
		struct {
			int32_t horizontal_move = 0, vertical_move = 0;
			int32_t last_mouse_x, last_mouse_y;
			bool lbtn_down = false, rbtn_down = false, single_click = false, double_click = false;
			glm::vec3 click_world_center = glm::vec3(0, 0, 0);
			glm::vec3 hover_world_center = glm::vec3(0, 0, 0);
			glm::vec3 click_world_color = glm::vec3(0, 0, 0);
			int32_t click_x = 0, click_y = 0;

			// 1: inside of model
			// -1: outside of model
			int32_t click_init_mask = 0;
			Real click_z = 1.0;
			int click_comp_id = -1;
			int hover_comp_id = -1;

			float chosen_ui_id = -1.f;
			int clpbox_face_id = -1;
		}m_mouse_status;
		private:
		struct {
			Real mouse_hori_vel = 0.015;
			Real mouse_vert_vel = 0.01;

			bool anti_aliasing = true;
		}m_option;

		// -------- render time --------
		double m_last_time = 0, m_delta_time = 0;
		// ----- ----- ----- ----- -----

		const Real m_znear = 0.1f, m_zfar = 1000.0f;
		const Real fov = 45.0f;

		bool key_frame_stopper = true;

		bool hidden = true;

		bool multichoose = false;

		// the option of whether to open the drawing match
		bool showDrawing = false;
		bool openDrawingMatch = false;
		glm::vec4 hidden_drawing_plane = glm::vec4(0.f, 10000.f, 0.f, 1.f);

	public:
		//------ chosen list
		std::set<uint32_t> chosen_list;

	private:
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

		// --------------------- event helper ----------------------
	private:
		void _setClickedWorldCoords(double clicked_x, double clicked_y, double clicked_z);
		float _getClickedDepthValue(double clicked_x, double clicked_y);
		void _setClickedWorldColors(double click_x, double click_y, bool hover_mode, bool is_comp);
		// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

	};

}
#endif
