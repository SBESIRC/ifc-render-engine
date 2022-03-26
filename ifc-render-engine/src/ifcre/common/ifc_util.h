#pragma once
#ifndef IFCRE_UTIL_H_
#define IFCRE_UTIL_H_

#include "std_types.h"

namespace ifcre {
namespace util {
	uint32_t get_next_globalid() {
		static uint32_t global_id = 1;
		return global_id++;
	}
}
}

#endif