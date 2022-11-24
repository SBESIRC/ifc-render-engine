#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_

//#define _HAS_STD_BYTE 0
#include "../common/std_types.h"
#include "../common/ifc_util.h"
#include "../mesh_simplier.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Ifc2OpenGLDatas.h>
#include <random>// just used for test dynamic geom
//#define _USE_MATH_DEFINES

#define M_PI       3.14159265358979323846   // pi

//#include <math.h>

extern"C" Datas2OpenGL generateIFCMidfile(const std::string filename, const float tolerance = 0.01);

namespace ifcre {

	enum CompState {
		DUMP = 0x00,
		VIS = 0x01,
		CHOSEN = 0x02
	};

	union MtlMetaData {
		Real f;
		int i;
	};

	struct MtlData {
		MtlMetaData data[8];
		// 3 for kd(float, float, float), 3 for ks(float, float, float)
		// 1 for alpha(float), and 1 for ns(int)
	};
	struct MaterialData {
		glm::vec3 kd;		// 漫反射rgb
		glm::vec3 ks;		// 高光项rgb(specular)
		float alpha;		// 透明度
		int ns;				// cos的指数(specularity)32/18
		MaterialData() {}
		MaterialData(glm::vec4 a, glm::vec4 b, float c, int d) :kd(a), ks(b), alpha(c), ns(d) {}
	};

	class IFCModel {
	public:
		// generate IFCModel through datas2OpenGL, basically used currently
		IFCModel(const struct Datas2OpenGL& datas, bool edge_generated = false) :g_indices(datas.vert_indices), g_vertices(datas.verts),
			g_normals(datas.vert_normals2), c_indices(datas.search_m), edge_indices(datas.edge_indices),
			this_comp_belongs_to_which_storey(datas.this_comp_belongs_to_which_storey),
				storeys_comp_id(datas.storeys_component_id) {
				clock_t start, end;
			//start = clock();
			Vector<CompState>(c_indices.size(), VIS).swap(comp_states);
			
			size_t facs = datas.face_mat.size(); // 获取面的数量
			Vector<MaterialData>(facs).swap(material_data);
			for (size_t i = 0; i < facs; i++) {
				material_data[i].kd = glm::vec3(datas.face_mat[i].ka_r, datas.face_mat[i].ka_g, datas.face_mat[i].ka_b);
				material_data[i].ks = glm::vec3(datas.face_mat[i].ks_r, datas.face_mat[i].ks_g, datas.face_mat[i].ks_b);
				material_data[i].ns = datas.face_mat[i].ns;
				material_data[i].alpha = datas.face_mat[i].a;
			}
			getVerColor();					// 生成顶点颜色数组
			generateCompIds();				// 生成顶点到其包含物件的映射
			generateStoreyIds();
			generate_bbxs_by_comps();		// 生成各个物件的bbx
			generate_bbxs_each_floor();
			cal_tile_matrix();
			getVerAttrib();					// 生成顶点属性数组
			divide_model_by_alpha();		// 根据透明度将顶点分为两组
			generate_edges_by_msMeshes(edge_generated);	// 生成边
			//end = clock();
			//std::cout << (double)(end - start) / CLOCKS_PER_SEC << "s used for oepnGL data generating\n";
		}
		// generate IFCModel using myownfile.dll directly, not used anymore
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
			generateStoreyIds();
			generateCompIds();
			generate_bbxs_by_comps();
			generate_bbxs_each_floor();
			cal_tile_matrix();
			getVerAttrib();
			divide_model_by_alpha();
		}

		IFCModel(Vector<uint32_t>& _g_indices, Vector<Real>& _g_vertices, Vector<Real>& _g_normals, Vector<Vector<uint32_t>>& _c_indices,
			Vector<float>& _face_mat, Vector<uint32_t>& _edge_indices) : //, Vector<uint32_t>& _comp_types) :
			g_indices(_g_indices), g_vertices(_g_vertices), g_normals(_g_normals), c_indices(_c_indices) { //, comp_types(_comp_types) {
			clock_t start, end;
			start = clock();
			Vector<CompState>(c_indices.size(), VIS).swap(comp_states);

			size_t faces = _face_mat.size() / 8;// 获取面的数量
			Vector<MaterialData>(faces).swap(material_data);
			for (size_t i = 0,j = 0 ; i < faces; ++i, j+=8) {
				material_data[i].kd = glm::vec3(_face_mat[j], _face_mat[j+1], _face_mat[j+2]);
				material_data[i].ks = glm::vec3(_face_mat[j+3], _face_mat[j+4], _face_mat[j+5]);
				material_data[i].alpha = _face_mat[j + 6];
				material_data[i].ns = _face_mat[j+7];
			}

			getVerColor();					// 生成顶点颜色数组
			generateCompIds();				// 生成顶点到其包含物件的映射
			generate_bbxs_by_comps();		// 生成各个物件的bbx
			getVerAttrib();					// 生成顶点属性数组
			divide_model_by_alpha();		// 根据透明度将顶点分为两组
			if (edge_indices.size() > 0) {
				generate_edges_by_msMeshes(false);	// 生成边
			}
			end = clock();
			std::cout << (double)(end - start) / CLOCKS_PER_SEC << "s used for oepnGL data generating\n";
		}

		void generate_edges_by_msMeshes(bool edge_generated) {
			if (!edge_generated) {
				Vector<Vector<uint32_t>>().swap(c_edge_indices);
				Vector<uint32_t>().swap(edge_indices);
				mesh_simplier::build_ms_vertices(g_vertices, g_normals); // 存储顶点的位置和法向量
				Vector<mesh_simplier::Mesh> meshes = mesh_simplier::generateMeshes(c_indices); // 根据c_indices 生成对应的 构件到对应要显示的edges的映射
				for (mesh_simplier::Mesh mes : meshes) {
					edge_indices.insert(edge_indices.end(), mes.edge_indexp.begin(), mes.edge_indexp.end());
					c_edge_indices.emplace_back(mes.edge_indexp);
				}
				cur_edge_ind = edge_indices;
			}
			//for (int i = 0; i < c_edge_indices.size(); i++)
			//	ind_of_all_c_indices.emplace_back(i);
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

		// figure out the right bounding box of current IfcModel
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
		void setpMin(glm::vec3 curMin) {
			pMin = curMin;
		}
		void setpMax(glm::vec3 curMax) {
			pMax = curMax;
		}

		/*union tempa{
			Real f;
			int i;
		};*/
		// organize the datas of IfcModel into glVertexAttributes, for sending to GPU
		Vector<Real> getVerAttrib() {
			size_t s = g_vertices.size(); //xyzxyzxyz...
			Vector<Real>(s / 3 * 12).swap(ver_attrib);//no!
			int offset = 0;
			for (int i = 0; i < s; i += 3) {
				ver_attrib[offset + i] = g_vertices[i];								// 位置.x
				ver_attrib[offset + i + 1] = g_vertices[i + 1];						// 位置.y
				ver_attrib[offset + i + 2] = g_vertices[i + 2];						// 位置.z
				ver_attrib[offset + i + 3] = g_normals[i];							// 法向量.x
				ver_attrib[offset + i + 4] = g_normals[i + 1];						// 法向量.y
				ver_attrib[offset + i + 5] = g_normals[i + 2];						// 法向量.z
				uint32_t color_ind = i / 3 * 4;
				ver_attrib[offset + i + 6] = g_kd_color[color_ind];					// 颜色.r
				ver_attrib[offset + i + 7] = g_kd_color[color_ind + 1];				// 颜色.g
				ver_attrib[offset + i + 8] = g_kd_color[color_ind + 2];				// 颜色.b
				ver_attrib[offset + i + 9] = g_kd_color[color_ind + 3];				// 颜色.a
				ver_attrib[offset + i + 10] = util::int_as_float(comp_ids[i / 3]);//todo: transform by bit // 所在物件的索引
				ver_attrib[offset + i + 11] = util::int_as_float(comp_storey_ids[i / 3]);// 楼层信息
				offset += 9;
			}
			return ver_attrib;
		}

		// just add compid attribute for each vertex on which component it is
		void generateCompIds() { // comp_ids: 顶点索引找到对应的物件索引
			Vector<uint32_t>(g_vertices.size() / 3).swap(comp_ids);
			int j = 0;
			for (int i = 0; i < c_indices.size(); i++) {
				auto ix = c_indices[i];
				int s = ix.size();
				for (int j = 0; j < s; j++) {
					comp_ids[ix[j]] = i;
				}
			}
		}

		void generateStoreyIds() {
			comp_storey_ids.resize(g_vertices.size() / 3);
			int j = 0;
			for (int i = 0; i < c_indices.size(); i++)
			{
				auto ix = c_indices[i];
				int s = ix.size();
				for (int j = 0; j < s; j++) {
					comp_storey_ids[ix[j]] =/*this components storey id*/this_comp_belongs_to_which_storey[i];
				}
			}
		}

		// add color attribute for each vertex
		Vector<Real> getVerColor() {
			g_kd_color.resize(g_vertices.size() / 3 * 4);
			for (int i = 0; i < c_indices.size(); i++) {
				for (int j = 0; j < c_indices[i].size(); j++) {
					g_kd_color[4 * c_indices[i][j]] = material_data[i].kd.x;
					g_kd_color[4 * c_indices[i][j] + 1] = material_data[i].kd.y;
					g_kd_color[4 * c_indices[i][j] + 2] = material_data[i].kd.z;
					g_kd_color[4 * c_indices[i][j] + 3] = material_data[i].alpha;
				}
			}
			return g_kd_color;
		}

		// not used
		bool is_this_comp_transparency(uint32_t index) {
			return material_data[index].alpha < .99;
		}
		//divide components into 2 vectors by their transparence
		void divide_model_by_alpha() {
			Vector<uint32_t> transparency_ind;
			Vector<uint32_t> no_transparency_ind;
			Unordered_set<uint32_t>().swap(trans_c_indices_set);
			Vector<uint32_t>().swap(cur_c_indices);
			int v_count = 0;
			for (int i = 0; i < c_indices.size(); ++i) {
				if (material_data[i].alpha < 1) {
					transparency_ind.insert(transparency_ind.end(), c_indices[i].begin(), c_indices[i].end());
					trans_c_indices_set.insert(i);
				}
				else {
					no_transparency_ind.insert(no_transparency_ind.end(), c_indices[i].begin(), c_indices[i].end());
				}
				v_count += c_indices[i].size();
				cur_c_indices.emplace_back(i);
			}
			trans_ind = transparency_ind;
			no_trans_ind = no_transparency_ind;
			cur_vis_trans_ind = trans_ind;
			cur_vis_no_trans_ind = no_trans_ind;
		}

		void update_chosen_list(std::set<uint32_t>& chosen_list) {
			for (const int i : cur_c_indices) {
				if (comp_states[i] != VIS) {
					comp_states[i] = VIS;
				}
			}
			uint32_t c_indices_size = c_indices.size();
			if (c_indices_size == 1 && chosen_list.size() > 0 && (*chosen_list.begin() >= c_indices_size)) {
				chosen_list.clear();
			}
			else {
				for (auto cur_index : chosen_list) {
					if (cur_index < c_indices_size) {
						comp_states[cur_index] = CHOSEN;
					}
				}
			}
		}
		void generate_collision_list(std::vector<uint32_t>& collision_list) {
			Vector<uint32_t> ret;
			for (auto i : collision_list) {
				ret.insert(ret.end(), c_indices[i].begin(), c_indices[i].end());
			}
			collision_ebo = ret;
		}

		void update_chosen_and_vis_list() {
			Vector<uint32_t>().swap(cur_vis_trans_ind);
			Vector<uint32_t>().swap(cur_vis_no_trans_ind);
			Vector<uint32_t>().swap(cur_edge_ind);
			Vector<uint32_t>().swap(cur_chosen_trans_ind);
			Vector<uint32_t>().swap(cur_chosen_no_trans_ind);

			uint32_t edge_c_indices_size = c_edge_indices.size();

			for (const int i : cur_c_indices) {
				if (comp_states[i] == VIS) {
					if (trans_c_indices_set.find(i) != trans_c_indices_set.end()) {
						cur_vis_trans_ind.insert(cur_vis_trans_ind.end(), c_indices[i].begin(), c_indices[i].end());
					}
					else {
						cur_vis_no_trans_ind.insert(cur_vis_no_trans_ind.end(), c_indices[i].begin(), c_indices[i].end());
					}
					if (i < edge_c_indices_size) {
						cur_edge_ind.insert(cur_edge_ind.end(), c_edge_indices[i].begin(), c_edge_indices[i].end());
					}
				}
				else if (comp_states[i] == CHOSEN) {
					if (trans_c_indices_set.find(i) != trans_c_indices_set.end()) {
						cur_chosen_trans_ind.insert(cur_chosen_trans_ind.end(), c_indices[i].begin(), c_indices[i].end());
					}
					else {
						cur_chosen_no_trans_ind.insert(cur_chosen_no_trans_ind.end(), c_indices[i].begin(), c_indices[i].end());
					}
				}
			}
		}

		void generate_bbxs_by_comps() {
			size_t cindicessize = c_indices.size(); // 获取物件数量
			Vector<Real>(cindicessize * 6).swap(comps_bbx); // 每个物件对应6个float值
			Real a_min[3], a_max[3];
			int bbx_offset = 0;
			for (auto& ix : c_indices) {
				size_t s = ix.size();
				a_min[0] = a_min[1] = a_min[2] = std::numeric_limits<real_t>::max();
				a_max[0] = a_max[1] = a_max[2] = std::numeric_limits<real_t>::lowest();
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

			// 整个模型的bbx
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

		Vector<Real> generate_bbxs_bound_by_vec(const std::set<uint32_t>& comp_indices) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto& id : comp_indices) {
				for (int i = 0; i < 3; i++) {
					ret[i] = std::min(ret[i], comps_bbx[6 * id + i]);
				}
				for (int i = 3; i < 6; i++) {
					ret[i] = std::max(ret[i], comps_bbx[6 * id + i]);
				}
			}
			return ret;
		}

		Vector<Real> generate_bbxs_bound_by_vec(const std::vector<uint32_t>& comp_indices) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto& id : comp_indices) {
				for (int i = 0; i < 3; i++) {
					ret[i] = std::min(ret[i], comps_bbx[6 * id + i]);
				}
				for (int i = 3; i < 6; i++) {
					ret[i] = std::max(ret[i], comps_bbx[6 * id + i]);
				}
			}
			return ret;
		}

		Vector<Real> generate_bbxs_bound_by_vec(const std::set<uint32_t>& comp_indices, bool flag) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto& id : comp_indices) {
				glm::vec3 pos(comps_bbx[6 * id], comps_bbx[6 * id + 1], comps_bbx[6 * id + 2]);
				pos = tile_matrix[this_comp_belongs_to_which_storey[id]] * glm::vec4(pos, 1.0);
				ret[0] = std::min(ret[0], pos.x);
				ret[1] = std::min(ret[1], pos.y);
				ret[2] = std::min(ret[2], pos.z);

				pos = glm::vec3(comps_bbx[6 * id + 3], comps_bbx[6 * id + 4], comps_bbx[6 * id + 5]);
				pos = tile_matrix[this_comp_belongs_to_which_storey[id]] * glm::vec4(pos, 1.0);
				ret[3] = std::max(ret[3], pos.x);
				ret[4] = std::max(ret[4], pos.y);
				ret[5] = std::max(ret[5], pos.z);
			}
			return ret;
		}

		Vector<Real> generate_bbxs_bound_by_vec(const std::vector<uint32_t>& comp_indices, bool flag) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto& id : comp_indices) {
				glm::vec3 pos(comps_bbx[6 * id], comps_bbx[6 * id + 1], comps_bbx[6 * id + 2]);
				pos = tile_matrix[this_comp_belongs_to_which_storey[id]] * glm::vec4(pos, 1.0);
				ret[0] = std::min(ret[0], pos.x);
				ret[1] = std::min(ret[1], pos.y);
				ret[2] = std::min(ret[2], pos.z);

				pos = glm::vec3(comps_bbx[6 * id + 3], comps_bbx[6 * id + 4], comps_bbx[6 * id + 5]);
				pos = tile_matrix[this_comp_belongs_to_which_storey[id]] * glm::vec4(pos, 1.0);
				ret[3] = std::max(ret[3], pos.x);
				ret[4] = std::max(ret[4], pos.y);
				ret[5] = std::max(ret[5], pos.z);
			}
			return ret;
		}

		Vector<Real> generate_bbxs_by_vec(const std::set<uint32_t>& comp_indices) {
			Vector<Real> ret = generate_bbxs_bound_by_vec(comp_indices);
			Vector<Real> ret2(24);
			for (int i = 0; i < 8; i++) {
				ret2[i * 3] = i & 1 ? ret[3] : ret[0];
				ret2[i * 3 + 1] = i & 2 ? ret[4] : ret[1];
				ret2[i * 3 + 2] = i & 4 ? ret[5] : ret[2];
			}
			return ret2;
		}

		Vector<Real> generate_bbxs_by_vec2(const Vector<Real>& ret) {
			Vector<Real> ret2(24);
			for (int i = 0; i < 8; i++) {
				ret2[i * 3] = i & 1 ? ret[3] : ret[0];
				ret2[i * 3 + 1] = i & 2 ? ret[4] : ret[1];
				ret2[i * 3 + 2] = i & 4 ? ret[5] : ret[2];
			}
			return ret2;
		}

		/*void getxyz_of_indices(const set<uint32_t>& comp_indices, Real& center_x, Real& center_y, Real& center_z, Real& width, Real& height, Real& length) {
			Vector<Real> ret = { FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			uint32_t c_indices_size = c_indices.size();
			for (auto& id : comp_indices) {
				if (id > c_indices_size) {
					continue;
				}
				for (int i = 0; i < 3; i++) {
					ret[i] = std::min(ret[i], comps_bbx[6 * id + i]);
				}
				for (int i = 3; i < 6; i++) {
					ret[i] = std::max(ret[i], comps_bbx[6 * id + i]);
				}
			}
		}*/

		void generate_bbxs_each_floor() {								// generate each floor's bbxs and sorting
			for (int i = 0; i < storeys_comp_id.size(); i++) {
				std::set<uint32_t> temp_comp_indices;
				for (int j = 0; j < storeys_comp_id[i].size(); j++) {
					temp_comp_indices.insert(storeys_comp_id[i][j]);
				}
				Vector<Real> bbx_floor = generate_bbxs_bound_by_vec(temp_comp_indices);		// floor's component's bbx
				bbx_floor.push_back(util::int_as_float(this_comp_belongs_to_which_storey[storeys_comp_id[i][0]]));		// add the current floor number
				bbxs_each_floor.push_back(bbx_floor);
			}

			std::sort(bbxs_each_floor.begin(), bbxs_each_floor.end(), [](Vector<Real>a, Vector<Real>b) { return a[1] < b[1]; });	// Ascending by y-axis

			//Vector<int> floorIndex(100, 0);
			//for (int i = 0; i < bbxs_each_floor.size(); i++) {			// let the real floor number index to the sorted floor number
			//	int id = util::float_as_int(bbxs_each_floor[i][6]);							// the real floor number
			//	floorIndex[id] = i;
			//	bbxs_each_floor[i].pop_back();
			//}
			//realFloor2sortFloor = floorIndex;
		}

		void cal_tile_matrix() {
			float max_delta_z = 0;
			float scale = 2.0;
			for (int i = 0; i < bbxs_each_floor.size(); i++) {
				max_delta_z = std::max(max_delta_z, bbxs_each_floor[i][5] - bbxs_each_floor[i][2]);
			}
			for (int i = 0; i < bbxs_each_floor.size(); i++) {
				float delta_y = bbxs_each_floor[i][1] - bbxs_each_floor[0][1];
				glm::mat4 model(1.f);
				model = glm::translate(model, glm::vec3(0, -delta_y, -max_delta_z * scale * i));
				tile_matrix.push_back(model);
			}

			// let the tile_matrix ordered by the order of stoerys_comp_id
			Vector <glm::mat4> tile_matrix_ordered_temp(bbxs_each_floor.size(), glm::mat4(1.f));
			for (int i = 0; i < bbxs_each_floor.size(); i++) {
				int id = util::float_as_int(bbxs_each_floor[i][6]);			// read the real stoery_id
				tile_matrix_ordered_temp[id] = tile_matrix[i];
				//bbxs_each_floor[i].pop_back();
			}
			tile_matrix_ordered = tile_matrix_ordered_temp;
		}

		glm::vec3 get_bbx_center(const Vector<Real>& ret) {
			glm::vec3 pMin = glm::vec3(ret[0], ret[1], ret[2]);
			glm::vec3 pMax = glm::vec3(ret[3], ret[4], ret[5]);
			return (pMin + pMax) * 0.5f;
		}

		Vector<uint32_t> generate_ebo_from_component_ids(Vector<uint32_t>& input_comp_ids) {
			Vector<uint32_t> ret;
			for (auto i : input_comp_ids) {
				ret.insert(ret.end(), c_indices[i].begin(), c_indices[i].end());
			}
			return ret;
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
			mirror_model = t * mirror_model;
		}

		glm::vec3 getModelCenter() {
			return m_center;
		}
		glm::vec3 getCurModelCenter() {

		}

		Vector<glm::mat4> tile_offsets_mats() {
			return tile_matrix_ordered;
		}

		Vector<int> floorIndex() {
			return realFloor2sortFloor;
		}

		void PrecomputingCubeDirection() {
			Vector<glm::vec3>(26, glm::vec3(0.f, 0.f, 0.f)).swap(m_cube_direction_transform);
			/*glm::vec3 temp = pMax - pMin;
			glm::vec3 div = temp / 2.f;
			m_cube_direction_transform.push_back(-glm::vec3(div.x, 0., 0.));
			m_cube_direction_transform.push_back(glm::vec3(div.x, 0., 0.));
			m_cube_direction_transform.push_back(-glm::vec3(0., div.y, 0.));
			m_cube_direction_transform.push_back(glm::vec3(0., div.y, 0.));
			m_cube_direction_transform.push_back(-glm::vec3(0., 0., div.z));
			m_cube_direction_transform.push_back(glm::vec3(0., 0., div.z));

			for (auto& dr : m_cube_direction_transform) {
				dr /= temp.x;
			}*/
		}

		void TranslateToCubeDirection(int num) {
			util::get_model_matrix_byBBX(pMin, pMax, m_model, m_scale_factor);
			m_init_model = m_model;
			mirror_model = glm::mat4(1.f);

			translate(m_cube_direction_transform[num]); // 都是glm::vec3(0.f, 0.f, 0.f)
		}

		void setScaleFactor(Real scale) {
			m_scale_factor = scale;
		}

		Real getScaleFactor() {
			return m_scale_factor;
		}

		void setModelMatrix(const glm::mat4& model) {
			mirror_model = model * glm::inverse(bbx_model_mat);
			m_model = model;
			m_init_model = m_model;
		}
		glm::mat4 getModelMatrix() {
			return m_model;
		}
		glm::mat4 getInitModelMatrix() {
			return m_init_model;
		}
		
		glm::mat4 getMirrorModelMatrix() {
			return mirror_model;
		}

		glm::mat4 getPreComputeModelMatrix(int i) {
			return glm::translate(glm::mat4(1.f), m_cube_direction_transform[i]);
		}

		void enlarge_scale(float div, glm::vec3 targpos) {
			glm::mat4 trans_center(1.0f);
			glm::vec3 world_pos = m_model * glm::vec4(m_center, 1.f);
			trans_center = glm::translate(trans_center, world_pos);
			m_model = m_model * trans_center;

			glm::vec3 dis = targpos - world_pos;
			float len_of_dis = glm::length(dis);

			//std::cout << "targpos: " << targpos.x << " " << targpos.y << " " << targpos.z << std::endl;
			//std::cout << "world_pos: " << world_pos.x << " " << world_pos.y << " " << world_pos.z << std::endl;

			float changed = div > 0.f ? 1.25f : 0.8f;
			m_scale_factor *= changed;
			m_model = glm::scale(m_model, glm::vec3(changed, changed, changed));
			for (int i = 0; i < 3; i++)
				m_model[3][i] *= changed;

			glm::mat4 trans_click_center(1.0f);/*
			trans_click_center = glm::translate(trans_click_center, world_pos + dis * len_of_dis / 20.f);*/
			trans_click_center = glm::translate(trans_click_center, -world_pos * changed);
			m_model = trans_click_center * m_model;
		}

		void generate_circleLines(Vector<float>& grid_lines, Vector<float>& grid_circles, int per_degree = 30) {
			int circle_lines = (359 + per_degree) / per_degree;//change per_degree by radius（able to upgrade）
			int circle_pt_cnt = circle_lines * 6;
			Vector<float> oneCircle(circle_pt_cnt);
			for (int j = 0; j < grid_circles.size(); j += 12) {
				// construct each circle to lines
				Vector<float>(circle_pt_cnt).swap(oneCircle);
				float x, z;
				glm::qua<float> q = glm::qua<float>(glm::radians(glm::vec3(grid_circles[j + 3], grid_circles[j + 4], grid_circles[j + 5]))); // normal
				for (int circle_line = 0; circle_line < circle_lines; ++circle_line) {
					x = grid_circles[j + 10] * cos(circle_line * per_degree * M_PI / 180.f); // radius
					z = grid_circles[j + 10] * sin(circle_line * per_degree * M_PI / 180.f);
					glm::vec3 pos(x, 0, z);
					pos = q * pos;
					pos = pos + glm::vec3(grid_circles[j], grid_circles[j + 1], grid_circles[j + 2]); // center
					oneCircle[circle_line * 6 + 0] = pos.x; // start point
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

				for (int i = 0; i < circle_pt_cnt; i += 6) {
					grid_lines.emplace_back(oneCircle[i + 0]);
					grid_lines.emplace_back(oneCircle[i + 1]);
					grid_lines.emplace_back(oneCircle[i + 2]);
					grid_lines.emplace_back(oneCircle[i + 3]);
					grid_lines.emplace_back(oneCircle[i + 4]);
					grid_lines.emplace_back(oneCircle[i + 5]);
					// add this circle into grid_lines
					//grid_lines.insert(grid_lines.end(), oneCircle.begin(), oneCircle.end());
					grid_lines.emplace_back(grid_circles[j + 6]); // rgba
					grid_lines.emplace_back(grid_circles[j + 7]);
					grid_lines.emplace_back(grid_circles[j + 8]);
					grid_lines.emplace_back(grid_circles[j + 9]);
					grid_lines.emplace_back(grid_circles[j + 11]); // line width
					grid_lines.emplace_back(1.); // line type
				}
			}
		}

		Vector<uint32_t> collision_pairs; // collision pairs
		uint32_t render_id;// seems a render_id combine with an array of vertex?
		Vector<Real> ver_attrib;				// 每个顶点有10个属性，数量为顶点数量的十倍
		Vector<Real> comps_bbx;					// pmin, pmax // 物件对应的bbx信息，数量为物件数量的6倍

		Vector<uint32_t> g_indices;				// 顶点的索引，数量为面个数的三倍，每3个顶点一个面
		Vector<uint32_t> trans_ind;				// 原始透明顶点的索引
		Vector<uint32_t> no_trans_ind;			// 原始不透明顶点的索引
		Vector<uint32_t> edge_indices;			// ebo of edge

		Vector<Vector<uint32_t>> c_indices;		// 物件->顶点的索引，1级数量为物件的个数，2级为物件拥有顶点数
		Vector<Vector<uint32_t>> c_edge_indices;//ebos of edge, generated after generate_edges_by_msMeshes();

		Vector<Vector<uint32_t>> storeys_comp_id;//storeys_comp_id[x] stores all components' id of No.x storey
		Vector<int> this_comp_belongs_to_which_storey;//this_comp_belongs_to_which_storey[x] means No.x component belongs to No.this_comp_belongs_to_which_storey[x] storey

		Vector<CompState> comp_states;					// 记录每个comp的状态：隐藏、显示、高亮

		Vector<uint32_t> cur_chosen_trans_ind;			// 当前要高亮(多选)的透明顶点的索引
		Vector<uint32_t> cur_chosen_no_trans_ind;		// 当前要高亮(多选)的不透明顶点的索引

		Vector<uint32_t> cur_c_indices;					// 当前要显示的物件的索引
		Vector<uint32_t> cur_vis_trans_ind;				// 当前要显示的透明顶点的索引	
		Vector<uint32_t> cur_vis_no_trans_ind;			// 当前要显示的不透明顶点的索引	
		Vector<uint32_t> cur_edge_ind;					// 当前要显示的物件包含的边的索引

		Unordered_set<uint32_t> trans_c_indices_set;	// 透明物体的索引, 用来快速分类，一次建立，多次查询

		Vector<uint32_t> collision_ebo;//ebo of collision meshes, this is ready for GlUpload()

		Vector<uint32_t> bbx_drawing_order = { 0,1,5,4,0,2,6,4,5,7,3,1,3,2,6,7 }; // 按此定点顺序绘制bbx长方体框
		const Vector<uint32_t> collider_inde_hard = { 250, 422, 250, 140, 250, 535, 422, 535, 140, 535 };
		int collider_ind_count = 0;

		glm::mat4 bbx_model_mat;
		glm::vec3 curcenter;

		Vector <Vector<Real> > bbxs_each_floor;	// each element have seven member // e.g (pmin[0], pmin[1], pmin[2], pmax[0], pmax[1], pmax[2], stoery_id)
		Vector <int> realFloor2sortFloor;		// the real floor number index to the sorted floor number
		Vector <glm::mat4> tile_matrix;			// tile view matrix
		Vector <glm::mat4> tile_matrix_ordered;			// tile view matrix ordered by the order of storeys_comp_id

		//Vector<float> grid_lines; // position xyzxyz color: rgba...起点xyz 终点xyz 颜色rgba 线宽w 线型t
		//Vector<float> grid_circles; // 圆环中心xyz 圆环朝向xyz 圆环颜色rgba 圆环半径r 线宽w
	public:
		glm::mat4 m_model;						
		glm::mat4 m_init_model;					
		glm::mat4 mirror_model;					
		Real m_scale_factor;					// 整个模型的缩放系数
		glm::vec3 pMin = glm::vec3(0,0,0), pMax = glm::vec3(0,0,0);					// 整个模型的bbx信息
		glm::vec3 m_center;						// 整个模型的中心信息
		Vector<MaterialData> material_data;		// 存各个面的材质属性，每个面都有8项属性，数量为面的数量
		Vector<Real> g_vertices;				// 依次存储各个顶点位置的x、y、z信息，数量为顶点数量的三倍
		Vector<Real> g_kd_color;				// 依次存储各个顶点(漫反射项)颜色的x、y、z信息，数量为顶点数量的三倍
		Vector<Real> g_normals;					// 依次存储各个顶点法向量的x、y、z信息，数量为顶点数量的三倍
		Vector<uint> comp_ids;					// 可通过顶点索引找到对应的物件索引，数量为顶点的个数
		//Vector<uint32_t> comp_types;				// 存储构件所属类型，数量等同于构件数

		Vector<bool> is_trans;
		Vector<glm::vec3> m_cube_direction_transform;
		Vector<int> comp_storey_ids;// length = num of components; Component no.xxx's storey id is comp_storey_ids[xxx]
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
