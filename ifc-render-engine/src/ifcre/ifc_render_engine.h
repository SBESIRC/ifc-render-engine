﻿#pragma once
#ifndef IFC_RENDER_ENGINE
#define IFC_RENDER_ENGINE

#include "render/gl_render.h"
#include "render/gl_camera.h"
#include "common/std_types.h"
#include "resource/model.h"
#include <map>

#include "ifcrender/ifc_render.h"
namespace ifcre {

	// pure virtual function is to focus on 'action', not 'member type'
	class RenderEngine {
	public:
		virtual void setConfig(String key, String value) = 0;

		virtual void clear_model_data() = 0;
		virtual void set_g_indices(int val) = 0;
		virtual void set_g_vertices(float val) = 0;
		virtual void set_g_normals(float val) = 0;
		virtual void set_c_indices(int val) = 0;
		virtual void set_face_mat(float val) = 0;
		virtual void set_edge_indices(int val) = 0;
		//virtual void set_comp_types(int val) = 0;


		virtual void init(GLFWwindow*) = 0;
		//virtual void init2(GLFWwindow* ) = 0;
		//virtual void initialize(Map<String, String> &configs) = 0;
		//virtual void setFloat() = 0;
		virtual void run() = 0;
		virtual int getSelectedCompId() = 0;
		virtual int getSelectedCompIdsSize() = 0;
		virtual void getSelectedCompIds(int* arr) = 0;
		virtual void setSelectCompIds(int val) = 0;

		virtual void SetSleepTime(int val) = 0;
		virtual void SetDataReadyStatus(bool dataIsReady) = 0;
		virtual bool saveImage(const char* filePath) = 0;
		//virtual void SetClipBox() = 0;
		virtual void zoom2Home(bool resetAxis) = 0;

		virtual void set_grid_data(int val) = 0;
		virtual void set_grid_lines(float val) = 0;
		virtual void set_grid_circles(float val) = 0;
		virtual void set_grid_text(String val) = 0;
		virtual void set_grid_text_data(float val) = 0;
	};


	class IFCRenderEngine : public RenderEngine {
	public:
		//void initialize(Map<String, String> &configs);
		void setConfig(String key, String value);

		void clear_model_data();
		void set_g_indices(int val);
		void set_g_vertices(float val);
		void set_g_normals(float val);
		void set_c_indices(int val);
		void set_face_mat(float val);
		void set_edge_indices(int val);
		//void set_comp_types(int val);

		void set_grid_data(int val);
		void set_grid_lines(float val);
		void set_grid_circles(float val);
		void set_grid_text(String val);
		void set_grid_text_data(float val);

		//void init2(GLFWwindow* wndPtr);
		void init(GLFWwindow*);
		void UploadOriginalData();
		void run();
		int getSelectedCompId();
		int getSelectedCompIdsSize();
		void getSelectedCompIds(int* arr);
		void setSelectCompIds(int val);
		void SetSleepTime(int val);
		void SetDataReadyStatus(bool dataIsReady);
		bool saveImage(const char* filePath);
		//void SetClipBox();
		void zoom2Home(bool resetAxis);

		//test dynamic ebo of components, using keyboard input
		void updateDynamicEboData();
		//get data ready before draw
		void dataIntegration();
		void offscreenRending(const int index = 4);

		void zoom_into(Vector<Real> bound_vecs);
		void zoom_into_set_axis();
		bool flag_between_zoom_reset = false;
		void reset_coord(glm::vec3& clicked_coord);

	public:
		IFCRenderEngine(){}
		// not thread safety
		static SharedPtr<RenderEngine> getSingleton();
		int ui_key;
		int clp_face_key;

		SharedPtr<bool> mousemove;
	private:
		void drawFrame();
		void fps(double interval = 1.0);

	private:
		Map<String, String> m_cache_configs;
		bool m_DoesRenderAlreadyRunning = false;
		volatile bool m_DataIsReady = true;
		int to_show_states;
		Real scale_factor = 0;
		glm::mat4 ifc_m_matrix;
		uint32_t m_sleepTime = 10;
		SharedPtr<GLRender> m_glrender = nullptr;
		SharedPtr<RenderWindow> m_render_window = nullptr;
		SharedPtr<GLCamera> m_camera;

		SharedPtr<IFCModel> ifc_model;

		Vector<uint32_t> _g_indices;
		Vector<Real> _g_vertices;
		Vector<Real> _g_normals;
		Vector<uint32_t> _tmp_c_indices;
		Vector<Vector<uint32_t>> _c_indices;
		Vector<float> _face_mat;
		Vector<uint32_t> _edge_indices;

		Vector<float> grid_lines;
		Vector<float> grid_circles;
		Vector<Wstring> grid_text;
		Vector<float> grid_text_data;
		bool grid_line_reset = true;
		bool grid_text_reset = true;
	private:
		SharedPtr<IFCRender> m_ifcRender;
		Scene m_vulkanScene;

	private:
		const glm::vec3 m_view_pos = glm::vec3(0, 0, 15); // 摄像机位置 // z轴正方向出屏幕
		glm::vec3 m_last_hover_pos = glm::vec3(0);
		// right mouse click
		bool m_last_rmclick = false;
		uint32_t select_bbx_id;

		// view cube (gizmo) sets
		int cube_num = 0;
		bool cube_change_log = false;

		bool trigger = false;

		float global_alpha = 1.f;
		float m_trans_alpha = .3f;

		bool clipboxButton = false;
		bool drawingMatchButton = false;
		bool tileViewButton = false;

		bool collider_trans_flag = false;

		float script_scale_fractor = 1.f;
		bool showcolid = false;

		std::vector<uint32_t> collision_list;
		RenderAPIEnum m_render_api = RenderAPIEnum::OPENGL_RENDER_API;
	};
}

#endif
