#pragma once
#ifndef _IFCRE_SCENE_H_
#define _IFCRE_SCENE_H_

#include "ifc_camera.h"
#include <memory>
#include "../resource/model.h"
namespace ifcre {

	struct Scene {
		IFCCamera* m_editCamera;
		IFCModel* m_ifcObject;
		glm::vec3 m_pickWorldPos;
		
		struct {
			int32_t hovered = -1;
			int32_t clicked = -1;
		}m_compId;
	};

};
#endif