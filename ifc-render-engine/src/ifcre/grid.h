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
	
	class Grid {
	public:
		Grid(Vector<float>& _grid_lines, vector<float>& _grid_circles):grid_lines(_grid_lines), grid_circles(_grid_circles) {
			
		}
		vector<float> grid_lines; // position xyzxyz color: rgba...起点xyz 终点xyz 颜色rgba 线宽w 线型t
		vector<float> grid_circles; // 圆环中心xyz 圆环朝向xyz 圆环颜色rgba 圆环半径r 线宽w
		vector<Text> texts;

		void generate_circleLines(int per_degree = 10) {
			int circle_lines = (359 + per_degree) / per_degree;//change per_degree by radius（able to upgrade）
			int circle_pt_cnt = circle_lines * 6;
			vector<float> oneCircle(circle_pt_cnt);
			for (int i = 0, j = 0; j < grid_circles.size(); ++i, j += 12) {
				// construct each circle to lines
				vector<float>().swap(oneCircle);
				float x, z;
				glm::qua<float> q = glm::qua<float>(glm::radians(glm::vec3(grid_circles[j + 3], grid_circles[j + 4], grid_circles[j + 5]))); // normal
				for (int circle_line = 0; circle_line < circle_lines; ++circle_line) {
					x = grid_circles[j + 10] * cos(i * per_degree * M_PI / 180.f);
					z = grid_circles[j + 10] * sin(i * per_degree * M_PI / 180.f);
					vec3 pos(x, 0, z);
					pos = q * pos;
					pos = pos + glm::vec3(grid_circles[j], grid_circles[j + 1], grid_circles[j + 2]); // center
					oneCircle[circle_line * 6] = pos.x; // start point
					oneCircle[circle_line * 6 + 1] = pos.y;
					oneCircle[circle_line * 6 + 2] = pos.z;
					if (circle_line != 0) {
						oneCircle[circle_line * 6 + 3] = oneCircle[circle_line * 6 - 6]; // end point
						oneCircle[circle_line * 6 + 4] = oneCircle[circle_line * 6 - 5];
						oneCircle[circle_line * 6 + 5] = oneCircle[circle_line * 6 - 4];
					}
				}
				oneCircle[3] = oneCircle[circle_pt_cnt - 6];
				oneCircle[4] = oneCircle[circle_pt_cnt - 5];
				oneCircle[5] = oneCircle[circle_pt_cnt - 4];
				// add this circle into grid_lines
				grid_lines.insert(grid_lines.end(), oneCircle.begin(), oneCircle.end());
				grid_lines.emplace_back(grid_circles[j + 6]); // rgba
				grid_lines.emplace_back(grid_circles[j + 7]);
				grid_lines.emplace_back(grid_circles[j + 8]);
				grid_lines.emplace_back(grid_circles[j + 9]);
				grid_lines.emplace_back(grid_circles[j + 11]); // line width
				grid_lines.emplace_back(1.); // line type
			}
		}
	};
}
