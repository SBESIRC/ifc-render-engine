﻿#pragma once
#ifndef IFCRE_UTIL_H_
#define IFCRE_UTIL_H_

#include "std_types.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define printvec3(a) std::cout<<a.x<<" "<<a.y<<" "<<a.z<<"\n"

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

		inline void get_model_matrix_byBBX(const glm::vec3 pMin, const glm::vec3 pMax, glm::mat4& o_model, Real& o_scale_factor) {
			//Real mmdimens = std::max(pMax.x - pMin.x, std::max(pMax.y - pMin.y, pMax.z - pMin.z));
			glm::vec3 tempp = pMax - pMin;
			Real mmdimens = std::sqrt(glm::dot(tempp, tempp));
			Real scales = 15.f / (.7f * mmdimens + 20.f);
			glm::vec3 offset = glm::vec3(-(pMin.x + pMax.x) / 2, -(pMin.y + pMax.y) / 2, -(pMin.z + pMax.z) / 2);
			o_model = glm::mat4(1.0f);
			o_model = glm::scale(o_model, glm::vec3(scales, scales, scales));
			//scale(scales, scales, scales);
			o_model = glm::translate(o_model, offset);
			o_scale_factor = scales;
		}

		inline float int_as_float(int in) {
			union X {
				float a;
				int b;
			}x;
			x.b = in;
			return x.a;
		}

		inline int float_as_int(float in) {
			union X {
				int a;
				float b;
			}x;
			x.b = in;
			return x.a;
		}
	}
}

#endif
