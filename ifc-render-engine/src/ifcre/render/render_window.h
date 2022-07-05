#pragma once
#ifndef IFCRE_RENDER_WINDOW_H_
#define IFCRE_RENDER_WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/std_types.h"
#include "gl/gl_render_texture.h"
#include "gl_camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace ifcre {

	struct ClipPlane {
		glm::vec3 normal;
		glm::vec3 front;
		glm::vec3 right;
		Real moveSpeed = .01f;
		Real rotateSpeed = .001f;
		glm::vec3 base_pos; 
		ClipPlane() {}
		~ClipPlane() {}
		ClipPlane(glm::vec4 p) {
			normal = p;
			base_pos = glm::vec3(0.);
			front = glm::vec3(0., 0., -1.);
			right = glm::normalize(glm::cross(front, normal));//1 0 0
			
		}
		glm::vec4 out_as_vec4() {
			return glm::vec4(normal, glm::dot(normal,base_pos));
		}
		void rotateByNormal(float angleA) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleA, normal);
			front = rot * glm::vec4(front, 1.f);
			right = rot * glm::vec4(right, 1.f);
		}

		void rotateByFront(float angleB) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleB, front);
			normal = rot * glm::vec4(normal, 1.f);
			right = rot * glm::vec4(right, 1.f);
		}

		void rotateByRight(float angleC) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleC, right);
			normal = rot * glm::vec4(normal, 1.f);
			front = rot * glm::vec4(front, 1.f);
		}
		glm::mat4 toMat() {
			//glm::mat4 ret(1.0f);
			//ret = glm::lookAt(base_pos, base_pos + front, normal);
			//ret = glm::lookAt(front, glm::vec3(0.),normal);
			glm::vec4 world_x = glm::vec4(-glm::cross(normal, front), 0.),
				world_z = glm::vec4(-front, 0.),
				world_y = glm::vec4(normal, 0.);
			glm::vec4 add(base_pos, 1.);
			//glm::vec4 add(0., 0., 0., 1.);
			glm::mat4 basis(world_x, world_y, world_z, add);
			//basis = glm::translate(basis, base_pos);
			return basis;
		}

	private:
		void _updateVectors() {
			normal = glm::normalize(glm::cross(right, front));
		}
	};

	struct ClipBox :public ClipPlane {
		Real length, width, height;
		ClipBox() {}
		~ClipBox() {}
		ClipBox(glm::vec3 pos, glm::vec3 up, glm::vec3 _right, Real len, Real wid, Real hei) :length(len), width(wid), height(hei)
			/*,base_pos(pos), normal(up), right(_right)*/
		{
			base_pos = pos;
			normal = up;
			right = _right;
			front = glm::cross(up, right);
		}
		Vector<glm::vec4> out_as_vec4s() {
			return {
				glm::vec4(-normal,glm::dot(-normal,base_pos + normal * height / 2.f)),
				glm::vec4(-right,glm::dot(-right,base_pos + right * width / 2.f)),
				glm::vec4(-front,glm::dot(-front,base_pos + front * length / 2.f)),
				glm::vec4(normal,glm::dot(normal,base_pos - normal * height / 2.f)),
				glm::vec4(right,glm::dot(right,base_pos - right * width / 2.f)),
				glm::vec4(front,glm::dot(front,base_pos - front * length / 2.f))
			};
		}
		glm::mat4 toMat() {
			glm::vec4 world_x = glm::vec4(-glm::cross(normal, front), 0.),
				world_z = glm::vec4(-front, 0.),
				world_y = glm::vec4(normal, 0.);
			glm::vec4 add(base_pos, 1.);
			glm::mat4 basis(world_x, world_y, world_z, add);
			glm::mat4 ret(1.0f);
			ret = glm::scale(ret, glm::vec3(width / 2., height / 2., length / 2.));

			return basis * ret;
		}
	};

	class RenderWindow {
	public:
		RenderWindow(const char* title, int32_t w, int32_t h, bool aa = true, bool vsync = false , GLFWwindow* wndPtr = NULL);
		~RenderWindow();
		bool Init(const char* title, int32_t w, int32_t h, bool aa = true, bool vsync = false,GLFWwindow* wndPtr = NULL);
		void processInput();

		bool isClose();

		bool swapBuffer();
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
		bool getHidden() { return hidden; }
		glm::vec3 getClippingPlanePos() { return use_clip_plane.base_pos; }
		ClipPlane getClippingPlane();
		Vector<glm::vec4> getClippingBoxVectors();
		ClipBox getClipBox() { return use_clip_box; }
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
		int geomframe = 0;
		bool geomchanged = true;

		void setDefaultStatus();
		// ---------- 

	private:

		void createFramebuffer(int w, int h);

		struct {
			uint32_t fbo_id;
			SharedPtr<GLRenderTexture> m_default_rt;
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

			// 1: inside of model
			// -1: outside of model
			int32_t click_init_mask = 0;
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

		bool key_frame_stopper = true;

		bool hidden = true;
		ClipPlane hidden_clip_plane = glm::vec4(0.f, 0.f, 0.f, -1.f);
		ClipPlane use_clip_plane = glm::vec4(0.f, 1.f, 0.f, 2.f);

		ClipBox use_clip_box = ClipBox(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f), 1.f, 1.f, 1.f);

		const Vector<glm::vec4> hidden_box_vector = Vector<glm::vec4>(6, glm::vec4(0.f, 0.f, 0.f, -1.f));

	private:
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
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