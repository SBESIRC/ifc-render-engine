#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_

#include "../common/std_types.h"
#include <cstdio>
#include <fstream>
#include <iostream>

namespace ifcre {

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
		}
	private:
		glm::vec3 pMin, pMax;
		Vector<uint32_t> g_indices;
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