#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_

#include "../common/std_types.h"
#include <cstdio>
#include <fstream>
#include <iostream>

namespace ifcre {

	union MtlMetaData {
		Real f;
		int i;
	};

	struct MtlData {
		MtlMetaData data[7];
	};

	class ComponentModel {
	public:
		uint32_t render_id;//different component may has different render style
		glm::vec3 pMin, pMax;//bounding box
		ComponentModel(Vector<int> _indices, size_t s/*, uint32_t _render_id) :render_id(_render_id*/) {
			this->_indices.resize(s);
			for (int i = 0; i < s; i++) {
				this->_indices[i] = _indices[i];
			}
		}
		Vector<uint32_t> getInices() {
			return this->_indices;
		}
	private:
		Vector<uint32_t> _indices;
	};

	class IFCModel {
	public:
		uint32_t render_id;//seems a render_id combine with an array of vertex?
		Vector<ComponentModel> components;
		Vector<Real> ver_attrib;
		Vector<uint32_t> g_indices;
		IFCModel(Vector<uint32_t> ids, Vector<Real> vers, Vector<Real> norms) :g_indices(ids), g_vertices(vers), g_normals(norms) {}
		IFCModel(const String filename) {
			std::ifstream is(filename.c_str(), std::ios::binary);

			//vertices
			size_t s;
			is.read((char*)&s, sizeof(Real));
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
				Vector<int> tmpvc(tmps);
				for (int j = 0; j < tmps; j++) {
					is.read((char*)&tmpvc[j], sizeof(unsigned int));
					tmpvc[j]--;
				}
				components.emplace_back(ComponentModel(tmpvc, tmps));
			}

			//material datas

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
		}
		Vector<Real> getVerAttrib() {
			size_t s = g_vertices.size();
			ver_attrib.resize(2 * s);
			int offset = 0;
			for (int i = 0; i < s; i+=3) {
				ver_attrib[offset + i] = g_vertices[i];
				ver_attrib[offset + i + 1] = g_vertices[i + 1];
				ver_attrib[offset + i + 2] = g_vertices[i + 2];
				ver_attrib[offset + i + 3] = g_normals[i];
				ver_attrib[offset + i + 4] = g_normals[i + 1];
				ver_attrib[offset + i + 5] = g_normals[i + 2];
				offset += 3;
			}
			return ver_attrib;
		}
	private:
		glm::vec3 pMin, pMax;
		
		Vector<Real> g_vertices;
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