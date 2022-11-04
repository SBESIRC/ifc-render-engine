#pragma once

#ifndef IFCRE_GL_RENDER_H_
#define IFCRE_GL_RENDER_H_

#include <glad/glad.h>
#include <stdint.h>
#include "../common/std_types.h"
#include "render_window.h"
#include "gl/glsl_program.h"
#include "gl/gl_vertex_buffer.h"
#include "gl/gl_uniform_buffer.h"
#include "gl/gl_enum.h"
#include "gl_camera.h"
#include "../Character.h"
#include "../Gizmo.h"
#include "../Axis.h"
#include "../SimpleUI.h"

#include "../common/ifc_enum.h"
#include "../resource/model.h"

namespace ifcre {

	struct GLColor {
		Real r;
		Real g;
		Real b;
		Real a;
	};

	class GLRender {
	public:
		GLRender();

		// ----------- low-level interfaces ---------------
		void clearFrameBuffer(GLClearEnum clear, GLColor* color = nullptr, Real depth = 1.0f);

		void enableTest(GLTestEnum test);
		void disableTest(GLTestEnum test);
		void depthFunc(GLFuncEnum func);
		void enableBlend();
		void disableBlend();
		void enableMultiSample();
		void disableMultiSample();

		// ----- ----- ----- ----- ----- ----- ----- -----

		uint32_t addModel(SharedPtr<GLVertexBuffer> vertex_buffer);
		void ModelVertexUpdate(uint32_t render_id, const Vector<Real>& vertices);
		void DynamicUpdate(uint32_t render_id, const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& no_trans_indices, const Vector<uint32_t>& trans_indices, const Vector<uint32_t>& edge_indices);
		void ChosenGeomUpdate(uint32_t render_id, const Vector<uint32_t>& chosen_no_trans_ebo, const Vector<uint32_t>& chosen_trans_ebo);

		void setModelViewMatrix(const glm::mat4& mv);
		void setModelMatrix(const glm::mat4& model);
		void setInitModelMatrix(const glm::mat4& init_model);
		void setMirrorModelMatrix(const glm::mat4& mirror);
		void setViewMatrix(const glm::mat4& view);
		void setProjectionMatrix(const glm::mat4& projection);
		void setAlpha(const float& alpha);
		void setCompId(const int& comp_id);
		void setHoverCompId(const int& comp_id);
		void setCameraDirection(const glm::vec3& m_front);
		void setClippingPlane(const glm::vec4& clip_plane);
		void setClippingBox(const Vector<glm::vec4>& clip_box);
		glm::vec4 get_test_matrix(const glm::vec4& a) const;
		glm::vec3 get_pixel_pos_in_screen(const glm::vec4& model_pos, const int& window_width, const int& window_height) const;
		// --------------- render ----------------------
		void render(uint32_t render_id, RenderTypeEnum type);
		void render(uint32_t render_id, RenderTypeEnum type, const uint32_t local_render_id);
		void renderAxis(IFCModel& ifc_model, const glm::vec3& pick_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos);
		void renderGrid(IFCModel& ifc_model);
		void renderGridText(Vector<Wstring>& texts, Vector<float>& text_data, bool& grid_text_reset);
		void renderGridLine(Vector<float>& grid_line, int width, int height, bool& grid_line_reset);
		void renderClipBox(const bool hidden, const ClipBox& clip_box, int clp_face_id);
		void renderClipBox(const ClipBox& clip_box);
		void renderClipBoxInUIlayer(const bool hidden, const ClipBox& clip_box);
		void renderText(glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height);
		void renderGizmo(const glm::mat4& rotate_matrix, const glm::vec2 window_size, int last_hovered_face_key);
		void renderGizmoInUIlayer(const glm::mat4& rotate_matrix, const glm::vec2 window_size);
		void renderSkybox(const glm::mat3& view_matrix, const glm::mat4& projection);
		unsigned int  loadCubemap(Vector<String> faces);
		// for offscreen
		void AerialViewRender(RenderWindow& w);

		void postRender(uint32_t col_tex_id, uint32_t depth_normal_tex_id = -1);
		void postRender(RenderWindow& w);
		// ----- ----- ----- ----- ----- ----- ----- -----
#pragma region UI changes
		SharedPtr<SimpleUI> simpleui;
		float thisk = 0.f;
		void bind_ui_to_window(RenderWindow& w) {
			simpleui = make_shared<SimpleUI>(w.get_glfw_window(), w.get_glsl_verison().c_str());
		}
		void ui_update() {
			ImVec4 tempvec = ImVec4(m_bg_color.x, m_bg_color.y, m_bg_color.z, m_bg_color.w);
			simpleui->updateFrame(thisk, tempvec);
			m_bg_color = glm::vec4(tempvec.x, tempvec.y, tempvec.z, tempvec.w);
		}
#pragma endregion
	private:
		Map<uint32_t, SharedPtr<GLVertexBuffer>> m_vertex_buffer_map;
		GLUniformBufferMap  m_uniform_buffer_map;
	private:

		glm::mat4 m_modelview;
		glm::mat4 m_model;
		glm::mat4 m_init_model;
		glm::mat4 mirror_model;
		glm::mat4 m_view;
		glm::mat4 m_projection;
		float m_alpha;
		glm::vec3 m_camera_front;
		int m_compId;
		int m_hoverCompId;

		glm::vec4 m_clip_plane;
		Vector<glm::vec4> m_clip_box;// m_clip_box.size() MUST be 6!

		// --------------- glsl program -----------------
		UniquePtr<GLSLProgram> m_offscreen_program;
		UniquePtr<GLSLProgram> m_normal_depth_program;
		UniquePtr<GLSLProgram> m_comp_id_program;
		UniquePtr<GLSLProgram> m_test_shader;
		UniquePtr<GLSLProgram> m_chosen_shader;
		UniquePtr<GLSLProgram> m_axis_shader;
		UniquePtr<GLSLProgram> m_select_bbx_shader;
		UniquePtr<GLSLProgram> m_edge_shader;
		UniquePtr<GLSLProgram> m_clip_plane_shader;
		UniquePtr<GLSLProgram> m_clip_plane_UI_shader;
		UniquePtr<GLSLProgram> m_collision_shader;
		UniquePtr<GLSLProgram> m_gizmo_shader;
		UniquePtr<GLSLProgram> m_gizmo_UI_shader;
		UniquePtr<GLSLProgram> m_text_shader;
		UniquePtr<GLSLProgram> m_skybox_shader;
		UniquePtr<GLSLProgram> m_grid_shader;
		UniquePtr<GLSLProgram> m_text3d_shader;
		// ----- ----- ----- ----- ----- ----- ----- -----
	private:
		void _defaultConfig();

		// render option
	private:
		glm::vec4 m_depnor_value = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
		//glm::vec4 m_bg_color = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);
		glm::vec4 m_bg_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		const int m_default_com_id = -1;
		//TextData textdata = TextData("resources/fonts/default.ttf");
		//TextureFont texturefont = TextureFont("resources/fonts/msyh.ttc", 32);
		TextureFont texturefont = TextureFont("resources/fonts/Stfangso.ttf", 32);
		SceneGizmo gizmo = SceneGizmo(0);
		EngineAxis myaxis = EngineAxis();
		//offscreen quad
	private:
		uint32_t off_vao;
		unsigned int intermediateFBO;
		unsigned int screenTexture;
	};
};

#endif
