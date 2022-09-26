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
		uint32_t stPt; // ���
		uint32_t edPt; // �յ�
		vec3 color; // ��ɫ
		float size; //�߿�
		std::string type; // ����
		Line() {};
		//Line(uint32_t _stPt, uint32_t _edPt) { stPt  = };

	};

	struct Circle {
		vec3 center; // Բ������
		float radius; // Բ���뾶
		vec3 normal; // Բ������
	public:
		Circle(vec3 _center, float _radius, vec3 _normal):
			center(_center), radius(_radius), normal(_normal)
		{
			setCircleLines();
		}

		//vec3 color; // Բ����ɫ
		//float size;	// ��ʾ��ϸ
		//vector<pair<vec3, vec3>> circleLines(,); // ����ߵ�Բ��
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
		string content;// ��������
		string type; // ����
		vec3 color;// ������ɫ
		float size;	// ���ִ�С
		glm::vec3 normal;// ���ֳ���
		glm::vec3 direction; // ���ַ���
		glm::vec3 center;// ����λ��
	};

	struct Label { // ��ע
		//�������ֺ�����ɣ�
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
