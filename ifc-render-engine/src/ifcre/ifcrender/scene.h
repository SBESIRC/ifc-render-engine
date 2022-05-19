#pragma once
#ifndef _IFCRE_SCENE_H_
#define _IFCRE_SCENE_H_

#include "ifc_camera.h"
#include <memory>
#include "../resource/model.h"
namespace ifcre {

	struct Scene {
		std::shared_ptr<IFCCamera> m_camera;
		IFCModel* m_ifcObject;
	};

};
#endif