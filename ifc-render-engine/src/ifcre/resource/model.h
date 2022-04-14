#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_

#include "../common/std_types.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern"C" void generateIFCMidfile(const std::string filename, const float tolerance = 0.01);

namespace ifcre {

	union MtlMetaData {
		Real f;
		int i;
	};

	struct MtlData {
		MtlMetaData data[8];
		// 3 for kd(float, float, float), 3 for ks(float, float, float)
		//1 for alpha(float), and 1 for ns(int)
	};
	struct MaterialData {
		glm::vec4 kd;
		glm::vec4 ks;
		float alpha;
		int ns;
		MaterialData() {}
		MaterialData(glm::vec4 a, glm::vec4 b, float c, int d) :kd(a), ks(b), alpha(c), ns(d) {}
	};

	class ComponentModel {
	public:
		uint32_t render_id;//different component may has different render style
		glm::vec3 pMin, pMax;//bounding box
		Vector<Real> bbx_vertex_array;
		Vector<uint32_t> bbx_draw_element_buffers;
		ComponentModel(Vector<uint32_t> _indices, size_t s/*, uint32_t _render_id) :render_id(_render_id*/) {
			this->_indices.resize(s);
			for (int i = 0; i < s; i++) {
				this->_indices[i] = _indices[i];
			}
		}
		Vector<uint32_t> getInices() {
			return this->_indices;
		}
		void setbbx(Vector<Real> g_vert) {
			Real x_min, x_max, y_min, y_max, z_min, z_max;
			x_min = y_min = z_min = FLT_MAX;
			x_max = y_max = z_max = -FLT_MAX;
			for (int i = 0; i < this->_indices.size(); i++) {
				x_min = std::min(x_min, g_vert[3*this->_indices[i]]);
				x_max = std::max(x_max, g_vert[3 * this->_indices[i]]);
				y_min = std::min(y_min, g_vert[3 * this->_indices[i] + 1]);
				y_max = std::max(y_max, g_vert[3 * this->_indices[i] + 1]);
				z_min = std::min(z_min, g_vert[3 * this->_indices[i] + 2]);
				z_max = std::max(z_max, g_vert[3 * this->_indices[i] + 2]);
			}
			pMax = glm::vec3(x_max, y_max, z_max);
			pMin = glm::vec3(x_min, y_min, z_min);
			generate_bbx_buffer();
		}
		void generate_bbx_buffer() {
			bbx_vertex_array.clear();
			bbx_vertex_array.resize(24);
			for (int i = 0; i < 8; i++) {
				bbx_vertex_array[3 * i] = (i & 1 ? pMax.x : pMin.x);
				bbx_vertex_array[3 * i + 1] = (i & 2 ? pMax.y : pMin.y);
				bbx_vertex_array[3 * i + 2] = (i & 4 ? pMax.z : pMin.z);
			}
			bbx_draw_element_buffers.clear();
			bbx_draw_element_buffers.resize(24);
			bbx_draw_element_buffers = { 0,1,3,2,4,5,7,6,0,1,5,4,2,3,7,6,0,2,6,4,1,3,7,5 };
		}
		
	private:
		Vector<uint32_t> _indices;
	};

	class IFCModel {
	public:
		void rotateInLocalSpace(glm::vec3& pick_center, float angle) {
			glm::mat4 rot(1.0f);
			glm::mat4 m(1.0f);
			glm::mat4 inv_model = glm::inverse(m_model);
			glm::vec3 model_pick = inv_model * glm::vec4(pick_center, 1.0f);
			rot = glm::rotate(rot, angle, glm::vec3(0, 1, 0));
			glm::vec3 trans = -model_pick;
			m_model = m_model * glm::translate(m, -trans) * rot * glm::translate(m, trans);
		}

		void rotateInWorldSpace(glm::vec3& pick_center, float angle) {
			glm::mat4 rot(1.0f);
			glm::mat4 m(1.0f);
			rot = glm::rotate(rot, angle, glm::vec3(1, 0, 0));
			glm::vec3 trans = -pick_center;
			m_model = glm::translate(m, -trans) * rot * glm::translate(m, trans) * m_model;
		}

		void translate(glm::vec3& step) {
			glm::mat4 t(1.0f);
			t = glm::translate(t, step);
			m_model = t * m_model;
		}

		glm::vec3 getModelCenter() {
			return m_center;
		}

		void setModelMatrix(const glm::mat4& model) {
			m_model = model;
		}
		glm::mat4 getModelMatrix() {
			return m_model;
		}
	private:
		glm::mat4 m_model;

	public:
		uint32_t render_id;//seems a render_id combine with an array of vertex?
		Vector<ComponentModel> components;
		Vector<Real> ver_attrib;
		Vector<uint32_t> g_indices;
		Vector<uint32_t> trans_ind;
		Vector<uint32_t> no_trans_ind;
		Vector<Vector<uint32_t>> c_indices;
		IFCModel(Vector<uint32_t> ids, Vector<Real> vers, Vector<Real> norms) :g_indices(ids), g_vertices(vers), g_normals(norms) {}
		IFCModel(const String filename) {
			std::ifstream is(filename.c_str(), std::ios::binary);
			if (!is.is_open()) {
				std::cout << filename << " opened failed.\n";
				exit(-1);
			}

			//vertices
			size_t s;
			is.read((char*)&s, sizeof(size_t));
			this->g_vertices.resize(s);
			Real x_min, x_max, y_min, y_max, z_min, z_max;
			x_min = y_min = z_min = FLT_MAX;
			x_max = y_max = z_max = -FLT_MAX;
			for (int i = 0; i < s; i++) {
				is.read((char*)&this->g_vertices[i], sizeof(Real));
				switch (i % 3)
				{
				case 0:
				{
					x_min = std::min(x_min, this->g_vertices[i]);
					x_max = std::max(x_max, this->g_vertices[i]);
					break;
				}
				case 1: {
					y_min = std::min(y_min, this->g_vertices[i]);
					y_max = std::max(y_max, this->g_vertices[i]);
					break;
				}
				default: {
					z_min = std::min(z_min, this->g_vertices[i]);
					z_max = std::max(z_max, this->g_vertices[i]);
					break;
				}
				}
			}
			this->pMax = glm::vec3(x_max, y_max, z_max);
			this->pMin = glm::vec3(x_min, y_min, z_min);
			m_center = (pMin + pMax) * 0.5f;
			//normals
			is.read((char*)&s, sizeof(size_t));
			this->g_normals.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&this->g_normals[i], sizeof(Real));
			}

			//global_indices
			is.read((char*)&s, sizeof(size_t));
			this->g_indices.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&this->g_indices[i], sizeof(unsigned int));
				this->g_indices[i]--;
			}
			//components' indices
			is.read((char*)&s, sizeof(size_t));
			size_t tmps;
			for (int i = 0; i < s; i++) {
				is.read((char*)&tmps, sizeof(size_t));
				Vector<uint32_t> tmpvc(tmps);
				for (int j = 0; j < tmps; j++) {
					is.read((char*)&tmpvc[j], sizeof(unsigned int));
					tmpvc[j]--;
				}
				components.emplace_back(ComponentModel(tmpvc, tmps));
				components[i].setbbx(g_vertices);
			}

			//material datas
			is.read((char*)&s, sizeof(size_t));
			material_data.resize(s);
			Vector<MtlData> mat_vec(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&mat_vec[i].data[0], sizeof(float) * 7);
				is.read((char*)&mat_vec[i].data[7], sizeof(int));
			}
			for (int i = 0; i < s; i++) {
				material_data[i] = MaterialData(
					glm::vec4(mat_vec[i].data[0].f, mat_vec[i].data[1].f, mat_vec[i].data[2].f, 0),
					glm::vec4(mat_vec[i].data[3].f, mat_vec[i].data[4].f, mat_vec[i].data[5].f, 0),
					mat_vec[i].data[6].f, mat_vec[i].data[7].i);
			}
			getVerColor();
			getVerAttrib();
			divide_model_by_alpha();
		}

		Vector<uint32_t> getgIndices() {
			return this->g_indices;
		}
		Vector<Real> getgVertices() {
			return this->g_vertices;
		}
		Vector<Real> getgNormals() {
			return this->g_normals;
		}

		void setBBX(glm::vec3 pa, glm::vec3 pb) {
			pMin = pa;
			pMax = pb;
			m_center = (pMin + pMax) * 0.5f;
		}
		void setMaterialData(Vector<MaterialData> _material_data) {
			material_data = _material_data;
		}
		glm::vec3 getpMin()const {
			return pMin;
		}
		glm::vec3 getpMax()const {
			return pMax;
		}
		
		Vector<Real> getVerAttrib() {
			size_t s = g_vertices.size();
			ver_attrib.resize(3 * s);
			int offset = 0;
			for (int i = 0; i < s; i += 3) {
				ver_attrib[offset + i] = g_vertices[i];
				ver_attrib[offset + i + 1] = g_vertices[i + 1];
				ver_attrib[offset + i + 2] = g_vertices[i + 2];
				ver_attrib[offset + i + 3] = g_normals[i];
				ver_attrib[offset + i + 4] = g_normals[i + 1];
				ver_attrib[offset + i + 5] = g_normals[i + 2];
				ver_attrib[offset + i + 6] = g_kd_color[i];
				ver_attrib[offset + i + 7] = g_kd_color[i + 1];
				ver_attrib[offset + i + 8] = g_kd_color[i + 2];
				offset += 6;
			}
			return ver_attrib;
		}
		Vector<Real> getVerColor() {
			Vector<Real> color(g_vertices.size());
			for (int i = 0; i < g_indices.size(); i++) {
				color[3 * g_indices[i]] = material_data[i / 3].kd.x;
				color[3 * g_indices[i] + 1] = material_data[i / 3].kd.y;
				color[3 * g_indices[i] + 2] = material_data[i / 3].kd.z;
			}
			g_kd_color = color;
			return color;
		}

		void divide_model_by_alpha() {
			Vector<uint32_t> transparency_ind;
			Vector<uint32_t> no_transparency_ind;
			for (int i = 0; i < g_indices.size(); i++) {
				if (material_data[i / 3].alpha < 1)
					transparency_ind.emplace_back(g_indices[i]);
				else
					no_transparency_ind.emplace_back(g_indices[i]);
			}
			trans_ind = transparency_ind;
			no_trans_ind = no_transparency_ind;
		}
	private:
		glm::vec3 pMin, pMax;
		glm::vec3 m_center;
		Vector<MaterialData> material_data;
		Vector<Real> g_vertices;
		Vector<Real> g_kd_color;
		Vector<Real> g_normals;
	};
	

	// for obj
	class DefaultModel {
	public:
		DefaultModel(Vector<uint32_t>ind, Vector<float> ver) : indices(ind), vertices(ver) {}
		uint32_t render_id;
		Vector<uint32_t> indices;
		Vector<float> vertices;
	};

}

#endif