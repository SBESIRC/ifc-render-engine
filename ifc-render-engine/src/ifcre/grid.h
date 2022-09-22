#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace ifcre {
	struct Line	{
		uint32_t stPt; // ���
		uint32_t edPt; // �յ�
		vec3 color; // ��ɫ
		float size; //�߿�
		std::string type; // ����
	};

	struct Circle {
		vec3 center; // Բ������
		vec3 color; // Բ����ɫ
		float size;	// ��ʾ��ϸ
		float radius; // Բ���뾶
		vec3 normal; // Բ������
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
		vector< GridLine>();
	};

	vector<float> g_vertices; // xyzxyz xyzxyz xyzxyz...
	vector<float> color; // xyzxyz xyzxyz xyzxyz...

}
