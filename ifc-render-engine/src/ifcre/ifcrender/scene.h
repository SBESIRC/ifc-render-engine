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
	};

};
#endif