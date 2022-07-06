#pragma once
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
		virtual void init(GLFWwindow *) = 0;
		//virtual void initialize(Map<String, String> &configs) = 0;
		virtual void run() = 0;
		virtual int getSelectedCompId() = 0;
		virtual void setSelectCompIds() = 0;
	};


	class IFCRenderEngine : public RenderEngine {
	public:
		//void initialize(Map<String, String> &configs);
		void setConfig(String key, String value);
		void init(GLFWwindow *);
		void run();
		int getSelectedCompId();
		void setSelectCompIds();

		//test dynamic ebo of components, using keyboard input
		void changeGeom();

	public:
		IFCRenderEngine() : m_init(false) {}
		// not thread safety
		static SharedPtr<RenderEngine> getSingleton();

	private:
		void drawFrame();
		
	private:
		Map<String, String> m_cache_configs;
		bool m_init;
		bool try_ifc;
		int geomframe = 0;
		bool use_transparency;
		SharedPtr<GLRender> m_glrender;
		SharedPtr<RenderWindow> m_render_window;
		SharedPtr<GLCamera> m_camera;

		SharedPtr<DefaultModel> test_model;
		SharedPtr<IFCModel> ifc_test_model;

	private:
		SharedPtr<IFCRender> m_ifcRender;
		Scene m_scene;

	private:
		const glm::vec3 m_view_pos = glm::vec3(0, 0, 10);
		glm::vec3 m_last_hover_pos = glm::vec3(0);
		// right mouse click
		bool m_last_rmclick = false;
		uint32_t select_bbx_id;

		RenderAPIEnum m_render_api = OPENGL_RENDER_API;
	};
}

#endif