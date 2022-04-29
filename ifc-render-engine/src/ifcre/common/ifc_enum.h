#pragma once
#ifndef IFCRE_ENUM_H_
#define IFCRE_ENUM_H_

namespace ifcre {
	enum RenderTypeEnum {
		NORMAL_DEPTH_WRITE = 0x00,
		DEFAULT_SHADING = 0x01,
		//EDGE_SHADING = 0x02,
		TRANSPARENCY_SHADING = 0x03,
		COMP_ID_WRITE = 0x04,
		BOUNDINGBOX_SHADING = 0x05
	};
	enum RenderPartEnum {
		ALL = 0x01,
		ALL_WEG = 0x02,
		NO_TRANS = 0x03,
		TRANS = 0x04,
		BBX_LINE = 0x05
	};
}

#endif