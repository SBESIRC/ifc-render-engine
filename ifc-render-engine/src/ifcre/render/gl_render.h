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
#include "../SkyBox.h"

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
		void clear_model();
		void reset_render();

		// ----- ----- ----- ----- ----- ----- ----- -----

		uint32_t addModel(SharedPtr<GLVertexBuffer> vertex_buffer, int id);
		void deleteModel(uint32_t id);
		void ModelVertexUpdate(uint32_t render_id, const Vector<Real>& vertices);
#ifndef ALL_COMP_TRANS
		void DynamicUpdate(uint32_t render_id, const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& no_trans_indices, const Vector<uint32_t>& trans_indices, const Vector<uint32_t>& edge_indices);
		void ChosenGeomUpdate(uint32_t render_id, const Vector<uint32_t>& chosen_no_trans_ebo, const Vector<uint32_t>& chosen_trans_ebo);
#endif
		void DynamicUpdate(uint32_t render_id, const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& trans_indices, const Vector<uint32_t>& edge_indices);
		void ChosenGeomUpdate(uint32_t render_id, const Vector<uint32_t>& chosen_trans_ebo);
		void CollisionGeomUpdate(uint32_t render_id, const Vector<uint32_t>& collid_ebo);

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
		void setCameraPos(const glm::vec3& m_pos);
		void setClippingPlane(const glm::vec4& clip_plane);
		void updateOpenDrawingMatch(bool _flag);
		void TileView(bool show);						// tile view
		void setStoreyMat(glm::mat4 matrix);
		void setClippingBox(const bool hidden);
		glm::vec4 get_test_matrix(const glm::vec4& a) const;
		glm::vec3 get_pixel_pos_in_screen(const glm::vec4& model_pos, const int& window_width, const int& window_height) const;

		void upload_mat4s_to_gpu(const Vector<glm::mat4>& offsets_mats);
		// --------------- render ----------------------

		void render(uint32_t render_id, RenderTypeEnum type, const uint32_t local_render_id);
		void renderAxis(IFCModel& ifc_model, const glm::vec3& pick_center, const glm::vec3& view_pos, const glm::vec3& init_view_pos);
		//void renderGrid(IFCModel& ifc_model);
		void renderGridText(Vector<Wstring>& texts, Vector<float>& text_data, bool& grid_text_reset);
		void renderGridLine(Vector<float>& grid_line, int width, int height, bool& grid_line_reset);
		void renderClipBox(const bool hidden);
		void renderClipBox();
		void TileViewMatrix(SharedPtr<IFCModel> ifcModel);
		void renderSkyBox(const glm::mat4& porjmat);
		//void renderSkybox1(const glm::mat3& view_matrix, const glm::mat4& projection);
		void renderClipBoxInUIlayer(const bool hidden);
		void renderText(glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height);
		void renderText(const std::string& text, const glm::vec3& position, Real scale, const glm::vec3& color, const int& window_width, const int& window_height);
		void renderViewCube(const glm::mat4& rotate_matrix, const glm::vec2 window_size);
		void renderGizmoInUIlayer(const glm::mat4& rotate_matrix, const glm::vec2 window_size);
		unsigned int  loadCubemap(Vector<String> faces);
		void renderDrawing(IFCModel& ifc_model, float k);			// ------------- drawing match shading -------------
		void renderTileViewDrawing(IFCModel& ifc_model);	// ------------- tile-view's drawing -------------
		// for offscreen
		void AerialViewRender(RenderWindow& w);

		void postRender(RenderWindow& w);
		// ----- ----- ----- ----- ----- ----- ----- -----
		// ------------- clips -------------------------

		int last_clp_face_key = 0;
		int last_hovered_face_key = 0;
		glm::vec3 getClippingPlanePos() { return use_clip_plane.base_pos; }
		ClipPlane getClippingPlane() {
			//if (hidden)
			return hidden_clip_plane;
			//return use_clip_plane;
		}
		Vector<glm::vec4> getClippingBoxVectors(bool _hidden) {
			if (_hidden)
				return hidden_box_vector;
			return use_clip_box->out_as_vec4s();
		}
		SharedPtr<ClipBox> getClipBox() {
			return use_clip_box;
		}

		glm::vec4 getDrawingPlane(bool _show) {
			if (_show)
				return drawing_plane.to_vec4();
			return hidden_drawing_plane;         // y is an infinite big pos
		}
		// ----- ----- ----- ----- ----- ----- ----- -----
#pragma region UI changes
#define thresh 1.f
		SharedPtr<ClipBoxUI> simpleui;
		void bind_ui_to_window(RenderWindow& w) {
			simpleui = make_shared<ClipBoxUI>(w.get_glfw_window(), w.get_glsl_verison().c_str());
		}
		void ui_update(SharedPtr<bool> mousemove, bool hidden, float& global_alpha, float& trans_alpha, float& script_scale_fractor, glm::vec2 dragmove, bool lbtn) {
			int my_key = last_clp_face_key - 26;
			glm::vec2 this_face_normal = glm::vec2(0.f);
			if (lbtn && my_key >= 0 && !hidden) {

				//auto temp = m_projection * m_view * use_clip_box->toMat() * use_clip_box->face_normal[my_key];
				auto temp = m_view * use_clip_box->toMat_use_for_ui_update() * use_clip_box->face_normal[my_key];
				this_face_normal = glm::vec2(temp.x, temp.y);
				auto adjust_parameter = glm::length(temp) * 15;
				//std::cout << "adjust_parameter: " << adjust_parameter << std::endl;
				this_face_normal = glm::normalize(this_face_normal);
				//std::cout << my_key << ": " << this_face_normal.x << " " << this_face_normal.y << "\n";
				//std::cout << "drag: " << dragmove.x << " " << dragmove.y << "\n";
				float times = glm::dot(this_face_normal, dragmove) / adjust_parameter;
				if (times > 0)
					use_clip_box->updateBox(my_key * 2, times);
				else if (/*mddl < lddl*/times < 0)
					use_clip_box->updateBox(my_key * 2 + 1, -times);
			}
			simpleui->updateFrame(mousemove, hidden, my_key, this_face_normal, m_bg_color, use_clip_box->base_pos, drawing_plane.normal, global_alpha, trans_alpha, script_scale_fractor);
		}
		void alpha_update() {

		}
		void bind_ui_to_clipbox() {
			simpleui->bind_clip_box(use_clip_box);
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
		float m_alpha = 1.0;
		glm::vec3 m_camera_front;
		glm::vec3 m_camera_pos;
		int m_compId;
		int m_hoverCompId;

		glm::vec4 m_clip_plane;
		Vector<glm::vec4> m_clip_box;// m_clip_box.size() MUST be 6!

		glm::vec4 m_drawing_match_plane;	// drawing match shader test ----------
		Vector<glm::vec4> m_tile_view_lowest_y_plane;

		int m_TileView;
		glm::mat4 storeyOffset_mat;

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
		UniquePtr<GLSLProgram> m_grid_shader;
		UniquePtr<GLSLProgram> m_text3d_shader;
		UniquePtr<GLSLProgram> m_skybox_shader;

		UniquePtr<GLSLProgram> m_drawing_match_shader;	// drawing match shader test ----------
		UniquePtr<GLSLProgram> m_tile_view_drawing_shader;	// drawing match shader test ----------

		// ----- ----- ----- ----- ----- ----- ----- -----
	private:
		void setToDefaultConfig();

		// render option
	private:
		glm::vec4 m_depnor_value = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
		//glm::vec4 m_bg_color = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);
		glm::vec4 m_bg_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 m_bg_color_off = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		const int m_default_com_id = -1;
		TextData textdata = TextData("resources/fonts/default.ttf");
		//TextureFont texturefont = TextureFont("resources/fonts/msyh.ttc", 32);
		TextureFont texturefont = TextureFont("resources/fonts/Stfangso.ttf", 32);
		SceneGizmo gizmo = SceneGizmo(0);
		EngineAxis myaxis = EngineAxis();

		SharedPtr<ClipBox> use_clip_box = make_shared<ClipBox>(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f), 40.f, 40.f, 80.f);
		ClipPlane hidden_clip_plane = glm::vec4(0.f, 0.f, 0.f, -1.f);
		ClipPlane use_clip_plane = glm::vec4(0.f, 1.f, 0.f, 2.f);
		int ui_id_num = 26;
		const Vector<glm::vec4> hidden_box_vector = Vector<glm::vec4>(6, glm::vec4(0.f, 0.f, 0.f, -1.f));
	public:
		DrawingMatchPlane drawing_plane = glm::vec4(0.f, 0.f, 0.f, 1.f);

		// the option of whether to open the drawing match
		glm::vec4 hidden_drawing_plane = glm::vec4(0.f, 10000.f, 0.f, 1.f);
		SharedPtr<mySkyBox> skyBox = make_shared<mySkyBox>();
		glm::mat4 special_projmat;
		//offscreen quad
	private:
		uint32_t off_vao;
		unsigned int intermediateFBO;
		unsigned int screenTexture;
	};
};

#endif
