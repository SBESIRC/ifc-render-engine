#pragma once
#ifndef IFCRE_ENUM_H_
#define IFCRE_ENUM_H_

namespace ifcre {
	enum RenderTypeEnum {
		NORMAL_DEPTH_WRITE = 0x00,
		DEFAULT_SHADING = 0x01,
		//EDGE_SHADING = 0x02,
		TRANSPARENCY_SHADING = 0x03,
		COMP_ID_WRITE = 0x04
	};
}

#endif