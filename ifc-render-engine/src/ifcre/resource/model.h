#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_

#include "../common/std_types.h"

namespace ifcre {

	class IFCModel {

	};
	

	// for obj
	class DefaultModel {
	public:
		DefaultModel(Vector<uint32_t>ind, Vector<float> ver) : indices(ind), vertices(ver) {}

		
	private:
		Vector<uint32_t> indices;
		Vector<float> vertices;
	};

}

#endif