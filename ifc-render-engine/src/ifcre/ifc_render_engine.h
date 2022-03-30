#pragma once
#ifndef IFC_RENDER_ENGINE
#define IFC_RENDER_ENGINE

#include "render/gl_render.h"
#include "render/gl_camera.h"
#include "common/std_types.h"
#include "resource/model.h"
#include <map>
namespace ifcre {

	// pure virtual function is to focus on 'action', not 'member type'
	class RenderEngine {
	public:
		virtual void initialize(Map<String, String> &configs) = 0;
		virtual void run() = 0;
	};


	class IFCRenderEngine : public RenderEngine {
	public:
		void initialize(Map<String, String> &configs);
		void run();

	public:
		IFCRenderEngine() : m_init(false) {}
		// not thread safety
		static SharedPtr<RenderEngine> getSingleton();

	private:
		void drawFrame();
	
	private:
		bool m_init;

		SharedPtr<GLRender> m_glrender;
		SharedPtr<RenderWindow> m_render_window;
		SharedPtr<GLCamera> m_camera;

		SharedPtr<DefaultModel> test_model;
	};
}

#endif