#pragma once
#ifndef _IFCRE_IFC_RENDER_H_
#define _IFCRE_IFC_RENDER_H_

#include "scene.h"
#include "surface_io.h"
#include "vk/vulkan_manager.h"
namespace ifcre {
	class RenderUI;
	class IFCRender {
	public:
		virtual void initialize(int32_t w, int32_t h) = 0;
		virtual bool render(Scene &scene) = 0;
		virtual SurfaceIO* getSurfaceIO() = 0;
		virtual void updateWindow(int32_t x, int32_t y, int32_t w, int32_t h) = 0;

		virtual float getDepthValue(int32_t x, int32_t y) = 0;
	};

	class IFCVulkanRender : public IFCRender {
	public:
		virtual void initialize(int32_t w, int32_t h) override;
		virtual bool render(Scene& scene) override;
		virtual SurfaceIO* getSurfaceIO() override { return m_surfaceIO.get(); }
		virtual void updateWindow(int32_t x, int32_t y, int32_t w, int32_t h) override;
		virtual float getDepthValue(int32_t x, int32_t y) override;

	private:
		VulkanManager m_vkManager;
		std::shared_ptr<SurfaceIO> m_surfaceIO;
		std::shared_ptr<RenderUI> m_renderUI;

		bool m_init = false;
		
	};
};
#endif