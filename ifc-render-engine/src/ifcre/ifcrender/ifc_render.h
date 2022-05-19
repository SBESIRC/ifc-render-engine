#pragma once
#ifndef _IFCRE_IFC_RENDER_H_
#define _IFCRE_IFC_RENDER_H_

#include "scene.h"
#include "surface_io.h"
#include "vk/vulkan_manager.h"
namespace ifcre {
	class IFCRender {
	public:
		virtual void initialize(int32_t w, int32_t h) = 0;
		virtual bool render(Scene &scene) = 0;
		virtual SurfaceIO* getSurfaceIO() = 0;
	};

	class IFCVulkanRender : public IFCRender {
	public:
		virtual void initialize(int32_t w, int32_t h) override;
		virtual bool render(Scene& scene) override;
		virtual SurfaceIO* getSurfaceIO() override { return m_surfaceIO.get(); }

	private:
		VulkanManager m_vkManager;
		std::shared_ptr<SurfaceIO> m_surfaceIO;

		bool m_init = false;
		
	};
};
#endif