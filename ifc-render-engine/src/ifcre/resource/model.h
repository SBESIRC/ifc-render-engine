#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_


#include "../common/std_types.h"
#include "../common/ifc_util.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Ifc2OpenGLDatas.h>

extern"C" Datas2OpenGL generateIFCMidfile(const std::string filename, const float tolerance = 0.01);

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
		glm::vec3 kd;
		glm::vec3 ks;
		float alpha;
		int ns;
		MaterialData() {}
		MaterialData(glm::vec4 a, glm::vec4 b, float c, int d) :kd(a), ks(b), alpha(c), ns(d) {}
	};

	class IFCModel {
	public:
		IFCModel(Vector<uint32_t> ids, Vector<Real> vers, Vector<Real> norms) :g_indices(ids), g_vertices(vers), g_normals(norms) {}
		IFCModel(const struct Datas2OpenGL& datas) :g_indices(datas.vert_indices), g_vertices(datas.verts), g_normals(datas.vert_normals2), c_indices(datas.search_m), edge_indices(datas.edge_indices) {
			clock_t start, end;
			start = clock();
			
			size_t facs = datas.face_mat.size();
			material_data.resize(facs);
			for (size_t i = 0; i < facs; i++) {
				material_data[i].kd = glm::vec3(datas.face_mat[i].ka_r, datas.face_mat[i].ka_g, datas.face_mat[i].ka_b);
				material_data[i].ks = glm::vec3(datas.face_mat[i].ks_r, datas.face_mat[i].ks_g, datas.face_mat[i].ks_b);
				material_data[i].ns = datas.face_mat[i].ns;
				material_data[i].alpha = datas.face_mat[i].a;
			}
			getVerColor();
			generateCompIds();
			generate_bbxs_by_comps();
			getVerAttrib();
			divide_model_by_alpha();
			end = clock();
			std::cout << (double)(end - start) / CLOCKS_PER_SEC << "s used for oepnGL data generating\n";
		}
		IFCModel(const String ifc_file_name) {
			const String filename = ifc_file_name;

			std::ifstream is(filename.c_str(), std::ios::binary);
			if (!is.is_open()) {
				std::cout << filename << " opened failed.\n";
				exit(-1);
			}

			//vertices
			size_t s;
			is.read((char*)&s, sizeof(size_t));
			this->g_vertices.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&this->g_vertices[i], sizeof(Real));
			}
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
			c_indices.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&tmps, sizeof(size_t));
				Vector<uint32_t> tmpvc(tmps);
				for (int j = 0; j < tmps; j++) {
					is.read((char*)&tmpvc[j], sizeof(unsigned int));
					tmpvc[j]--;
				}
				c_indices[i] = tmpvc;
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
			generateCompIds();
			generate_bbxs_by_comps();
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
		Vector<uint32_t> getCompInidces() {
			return this->comp_ids;
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
		union tempa{
			Real f;
			int i;
		};
		Vector<Real> getVerAttrib() {
			size_t s = g_vertices.size();
			ver_attrib.resize(s / 3 * 10);//no!
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
				ver_attrib[offset + i + 9] = util::in_as_float(comp_ids[i / 3]);//todo: transform by bit
				offset += 7;
			}
			return ver_attrib;
		}

		void generateCompIds() {
			comp_ids.resize(g_vertices.size()/3);
			int j = 0;
			for (int i = 0; i < c_indices.size(); i++) {
				auto ix = c_indices[i];
				int s = ix.size();
				for (int j = 0; j < s; j++) {
					comp_ids[ix[j]] = i;
				}
			}
		}

		Vector<Real> getVerColor() {
			g_kd_color.resize(g_vertices.size());
			for (int i = 0; i < g_indices.size(); i++) {
				g_kd_color[3 * g_indices[i]] = material_data[i / 3].kd.x;
				g_kd_color[3 * g_indices[i] + 1] = material_data[i / 3].kd.y;
				g_kd_color[3 * g_indices[i] + 2] = material_data[i / 3].kd.z;
			}
			return g_kd_color;
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

		void generate_bbxs_by_comps() {
			size_t cindicessize = c_indices.size();
			comps_bbx.resize(cindicessize * 6);
			Real a_min[3], a_max[3];
			int bbx_offset = 0;
			for (auto& ix : c_indices) {
				size_t s = ix.size();
				a_min[0] = a_min[1] = a_min[2] = FLT_MAX;
				a_max[0] = a_max[1] = a_max[2] = -FLT_MAX;
				for (size_t i = 0; i < s; i++) {
					for (int j = 0; j < 3; j++) {
						a_min[j] = std::min(g_vertices[3 * ix[i] + j], a_min[j]);
						a_max[j] = std::max(g_vertices[3 * ix[i] + j], a_max[j]);
					}
				}
				for (int i = 0; i < 3; i++) {
					comps_bbx[bbx_offset + i] = a_min[i];
				}
				for (int i = 0; i < 3; i++) {
					comps_bbx[bbx_offset + i + 3] = a_max[i];
				}
				bbx_offset += 6;
			}

			for (size_t i = 0; i < cindicessize; i++) {
				for (int j = 0; j < 3; j++) {
					a_min[j] = std::min(comps_bbx[6 * i + j], a_min[j]);
					a_max[j] = std::max(comps_bbx[6 * i + j + 3], a_max[j]);
				}
			}

			this->pMax = glm::vec3(a_max[0], a_max[1], a_max[2]);
			this->pMin = glm::vec3(a_min[0], a_min[1], a_min[2]);
			m_center = (pMin + pMax) * 0.5f;
		}

		Vector<Real> generate_bbxs_by_vec(const Vector<uint32_t>& comp_indices) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX};
			for (auto& id : comp_indices) {
				for (int i = 0; i < 3; i++) {
					ret[i] = std::min(ret[i], comps_bbx[6 * id + i]);
				}
				for (int i = 3; i < 6; i++) {
					ret[i] = std::max(ret[i], comps_bbx[6 * id + i]);
				}
			}
			Vector<Real> ret2(24);
			for (int i = 0; i < 8; i++) {
				ret2[i * 3] = i & 1 ? ret[3] : ret[0];
				ret2[i * 3 + 1] = i & 2 ? ret[4] : ret[1];
				ret2[i * 3 + 2] = i & 4 ? ret[5] : ret[2];
			}
			return ret2;
		}

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

		void setScaleFactor(Real scale) {
			m_scale_factor = scale;
		}

		Real getScaleFactor() {
			return m_scale_factor;
		}

		void setModelMatrix(const glm::mat4& model) {
			m_model = model;
		}
		glm::mat4 getModelMatrix() {
			return m_model;
		}

		uint32_t render_id;//seems a render_id combine with an array of vertex?
		Vector<Real> ver_attrib;
		Vector<Real> comps_bbx;//pmin, pmax
		Vector<uint32_t> g_indices;
		Vector<uint32_t> trans_ind;
		Vector<uint32_t> no_trans_ind;
		Vector<uint32_t> edge_indices;
		Vector<Vector<uint32_t>> c_indices;

		Vector<uint32_t> bbx_drawing_order = { 0,1,5,4,0,2,6,4,5,7,3,1,3,2,6,7 };

	private:
		glm::mat4 m_model;
		Real m_scale_factor;
		glm::vec3 pMin, pMax;
		glm::vec3 m_center;
		Vector<MaterialData> material_data;
		Vector<Real> g_vertices;
		Vector<Real> g_kd_color;
		Vector<Real> g_normals;
		Vector<uint> comp_ids;
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