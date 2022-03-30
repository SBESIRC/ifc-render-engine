#pragma once
#ifndef IFCRE_UTIL_H_
#define IFCRE_UTIL_H_

#include "std_types.h"
#include <fstream>
#include <sstream>

namespace ifcre {
namespace util {
	uint32_t get_next_globalid() {
		static uint32_t global_id = 1;
		return global_id++;
	}

	String read_file(String filename) {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(filename);
		StringStream stream;
		stream << file.rdbuf();
		file.close();
		return stream.str();
	}
}
}

#endif