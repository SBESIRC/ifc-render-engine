#pragma once
#ifndef IFCRE_UTIL_H_
#define IFCRE_UTIL_H_

#include "std_types.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ifcre {
namespace util {
	inline uint32_t get_next_globalid() {
		static uint32_t global_id = 1;
		return global_id++;
	}

	inline String read_file(String filename) {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(filename);
		StringStream stream;
		stream << file.rdbuf();
		file.close();
		return stream.str();
	}

	inline glm::mat4 get_model_matrix_byBBX(const glm::vec3 pMin, const glm::vec3 pMax) {
		//Real mm = std::max(pMax.x - pMin.x, std::max(pMax.y - pMin.y, pMax.z - pMin.z));
		Real scales = 1. / (pMax.x - pMin.x) * 15;
		glm::vec3 offset = glm::vec3(-(pMin.x + pMax.x) / 2, -(pMin.y + pMax.y) / 2, -(pMin.z + pMax.z) / 2);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(scales, scales, scales));
		//scale(scales, scales, scales);
		model = glm::translate(model, offset);
		return model;
	}
}
}

#endif