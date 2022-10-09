#pragma once
#include <cstdint>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ifc_render_engine.h"

using namespace std;
using namespace glm;

namespace ifcre {
	struct Circle {
		vec3 center; // 圆环中心
		float radius; // 圆环半径
		vec3 normal; // 圆环朝向

		vec4 color; // 圆环颜色
		float size;	// 显示粗细
	public:
		Circle(vec3 _center, float _radius, vec3 _normal):
			center(_center), radius(_radius), normal(_normal)
		{
			setCircleLines();
		}
		float degree = 5.f;
		float circleLines[432] = { 0.0 };
		vec3 yUp = vec3(0, 1, 0);
		void setCircleLines() {
			float x, z;
			//glm::qua<float> q = glm::qua<float>(glm::radians(glm::vec3(45.0f, 45.0f, 90.0f)));
			glm::qua<float> q = glm::qua<float>(glm::radians(normal));
			//glm::qua<float> q = glm::qua<float>(glm::normalize(normal));
			for (int i = 0; i < 72; ++i) {
				x = radius * cos(i * 5 * M_PI / 180.f);
				z = radius * sin(i * 5 * M_PI / 180.f);
				vec3 pos(x, 0, z);
				pos = q * pos;
				pos = pos + center;
				circleLines[i * 6] = pos.x;
				circleLines[i * 6 + 1] = pos.y;
				circleLines[i * 6 + 2] = pos.z;
				if (i != 0) {
					circleLines[i * 6 + 3] = circleLines[i * 6 - 6];
					circleLines[i * 6 + 4] = circleLines[i * 6 - 5];
					circleLines[i * 6 + 5] = circleLines[i * 6 - 4];
				}
			}
			circleLines[3] = circleLines[426];
			circleLines[4] = circleLines[427];
			circleLines[5] = circleLines[428];
		}
	};

	struct Text {
		string content;// 文字内容
		int type; // 字体
		vec4 color;// 文字颜色
		float size;	// 文字大小
		glm::vec3 normal;// 文字朝向
		glm::vec3 direction; // 文字方向
		glm::vec3 center;// 文字位置
	};
	
}
