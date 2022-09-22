#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace ifcre {
	struct Line	{
		uint32_t stPt; // 起点
		uint32_t edPt; // 终点
		vec3 color; // 颜色
		float size; //线宽
		std::string type; // 线型
	};

	struct Circle {
		vec3 center; // 圆环中心
		vec3 color; // 圆环颜色
		float size;	// 显示粗细
		float radius; // 圆环半径
		vec3 normal; // 圆环朝向
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
		vector< GridLine>();
	};

	vector<float> g_vertices; // xyzxyz xyzxyz xyzxyz...
	vector<float> color; // xyzxyz xyzxyz xyzxyz...

}
