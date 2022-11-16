#pragma once
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

		inline glm::mat4 inverse_mat4(glm::mat4 m)
		{
			float Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			float Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			float Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

			float Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			float Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			float Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

			float Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			float Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
			float Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

			float Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
			float Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
			float Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

			float Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
			float Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
			float Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

			float Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
			float Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
			float Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

			const glm::vec4 SignA = glm::vec4(1.0, -1.0, 1.0, -1.0);
			const glm::vec4 SignB = glm::vec4(-1.0, 1.0, -1.0, 1.0);

			glm::vec4 Fac0 = glm::vec4(Coef00, Coef00, Coef02, Coef03);
			glm::vec4 Fac1 = glm::vec4(Coef04, Coef04, Coef06, Coef07);
			glm::vec4 Fac2 = glm::vec4(Coef08, Coef08, Coef10, Coef11);
			glm::vec4 Fac3 = glm::vec4(Coef12, Coef12, Coef14, Coef15);
			glm::vec4 Fac4 = glm::vec4(Coef16, Coef16, Coef18, Coef19);
			glm::vec4 Fac5 = glm::vec4(Coef20, Coef20, Coef22, Coef23);

			glm::vec4 Vec0 = glm::vec4(m[1][0], m[0][0], m[0][0], m[0][0]);
			glm::vec4 Vec1 = glm::vec4(m[1][1], m[0][1], m[0][1], m[0][1]);
			glm::vec4 Vec2 = glm::vec4(m[1][2], m[0][2], m[0][2], m[0][2]);
			glm::vec4 Vec3 = glm::vec4(m[1][3], m[0][3], m[0][3], m[0][3]);

			glm::vec4 Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			glm::vec4 Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			glm::vec4 Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			glm::vec4 Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			glm::mat4 Inverse = glm::mat4(Inv0, Inv1, Inv2, Inv3);

			glm::vec4 Row0 = glm::vec4(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

			float Determinant = dot(m[0], Row0);

			Inverse /= Determinant;

			return Inverse;
		}
	}
}

#endif
