#pragma once
#include <cstdint>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;
using namespace glm;

namespace ifcre {
	struct Line	{
		uint32_t stPt; // 起点
		uint32_t edPt; // 终点
		vec3 color; // 颜色
		float size; //线宽
		std::string type; // 线型
		Line() {};
		//Line(uint32_t _stPt, uint32_t _edPt) { stPt  = };

	};

	struct Circle {
		vec3 center; // 圆环中心
		float radius; // 圆环半径
		vec3 normal; // 圆环朝向
	public:
		Circle(vec3 _center, float _radius, vec3 _normal):
			center(_center), radius(_radius), normal(_normal)
		{
			setCircleLines();
		}

		//vec3 color; // 圆环颜色
		//float size;	// 显示粗细
		//vector<pair<vec3, vec3>> circleLines(,); // 组成线的圆形
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
		string type; // 字体
		vec3 color;// 文字颜色
		float size;	// 文字大小
		glm::vec3 normal;// 文字朝向
		glm::vec3 direction; // 文字方向
		glm::vec3 center;// 文字位置
	};

	struct Label { // 标注
		//（由文字和线组成）
	};

	struct GridLine {
		vector<Line> line;
		Circle stCircle;
		Circle edCircle;
		Text text;
		//....
	};

	struct Grid {
		//vector< GridLine>();
	};

	vector<float> g_vertices; // xyzxyz xyzxyz xyzxyz...
	vector<float> color; // xyzxyz xyzxyz xyzxyz...

}
