#pragma once
#include <cstdint>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ifc_render_engine.h"

using namespace std;
using namespace glm;

namespace ifcre {
	struct Line	{
		vec3 stPt; // 起点
		vec3 edPt; // 终点
		vec4 color; // 颜色
		float size; //线宽
		//std::string type; // 线型
		int type; // 线型
		Line() {};
		//Line(uint32_t _stPt, uint32_t _edPt) { stPt  = };

	};

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
		int type; // 字体
		vec4 color;// 文字颜色
		float size;	// 文字大小
		glm::vec3 normal;// 文字朝向
		glm::vec3 direction; // 文字方向
		glm::vec3 center;// 文字位置
	};

	struct Label { // 标注
		//（由文字和线组成）
	};

	struct GridLine {
		vector<Line> lines;
		vector<Circle> circles;
		vector<Text> texts;
		GridLine() {};
		GridLine(vector<Line>& _lines,vector<Circle>& _circles, vector<Text>& _texts):
			lines(_lines), circles(_circles), texts(_texts)
		{};
	};

	struct Grid {
		vector<GridLine>(GridLine());
	};

	//void set_grid_data() {
	//	//此处先进行清空数据
	//	//init datas
	//	GridLine gridLine;
	//	int grid_size = 0;
	//	int line_size = 0;
	//	int circle_size = 0;
	//	int text_size = 0;
	//	grid_size = IFCRenderEngine::set_int();
	//	while (grid_size--) {
	//		line_size = IFCRenderEngine::set_int();
	//		while (line_size--) {
	//			glm::vec3 stPt;
	//			stPt.x = IFCRenderEngine::set_float();
	//			stPt.y = IFCRenderEngine::set_float();
	//			stPt.z = IFCRenderEngine::set_float();
	//			glm::vec3 edPt;
	//			edPt.x = IFCRenderEngine::set_float();
	//			edPt.y = IFCRenderEngine::set_float();
	//			edPt.z = IFCRenderEngine::set_float();
	//			glm::vec4 color;
	//			color.r = IFCRenderEngine::set_float();
	//			color.g = IFCRenderEngine::set_float();
	//			color.b = IFCRenderEngine::set_float();
	//			color.a = IFCRenderEngine::set_float();
	//			float size;
	//			size = IFCRenderEngine::set_float();
	//			int size_type;
	//			size_type = IFCRenderEngine::set_int();
	//		}
	//		circle_size = IFCRenderEngine::set_int();
	//		while (circle_size--) {
	//			glm::vec3 center;
	//			center.x = IFCRenderEngine::set_float();
	//			center.y = IFCRenderEngine::set_float();
	//			center.z = IFCRenderEngine::set_float();
	//			float radius;
	//			radius = IFCRenderEngine::set_float();
	//			glm::vec3 normal;
	//			normal.x = IFCRenderEngine::set_float();
	//			normal.y = IFCRenderEngine::set_float();
	//			normal.z = IFCRenderEngine::set_float();
	//			glm::vec4 color;
	//			color.r = IFCRenderEngine::set_float();
	//			color.g = IFCRenderEngine::set_float();
	//			color.b = IFCRenderEngine::set_float();
	//			color.a = IFCRenderEngine::set_float();
	//			float size;
	//			size = IFCRenderEngine::set_float();
	//		}
	//		int text_size = IFCRenderEngine::set_int();
	//		while (text_size--) {
	//			int textsize;
	//			//set char
	//			int type;
	//			type = IFCRenderEngine::set_int();
	//			glm::vec4 color;
	//			color.r = IFCRenderEngine::set_float();
	//			color.g = IFCRenderEngine::set_float();
	//			color.b = IFCRenderEngine::set_float();
	//			color.a = IFCRenderEngine::set_float();
	//			float size;
	//			size = IFCRenderEngine::set_float();
	//			glm::vec3 normal;
	//			normal.x = IFCRenderEngine::set_float();
	//			normal.y = IFCRenderEngine::set_float();
	//			normal.z = IFCRenderEngine::set_float();
	//			glm::vec3 direction;
	//			direction.x = IFCRenderEngine::set_float();
	//			direction.y = IFCRenderEngine::set_float();
	//			direction.z = IFCRenderEngine::set_float();
	//			glm::vec3 center;
	//			center.x = IFCRenderEngine::set_float();
	//			center.y = IFCRenderEngine::set_float();
	//			center.z = IFCRenderEngine::set_float();
	//		}
	//	}
	//}

	vector<float> g_vertices; // xyzxyz xyzxyz xyzxyz...
	vector<float> color; // xyzxyz xyzxyz xyzxyz...

}
