#pragma once
#ifndef IFCRE_OBJECT_H_
#define IFCRE_OBJECT_H_


#include "../common/std_types.h"
#include "../common/ifc_util.h"
#include "../mesh_simplier.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Ifc2OpenGLDatas.h>
#include <random>// just used for test dynamic geom

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
		IFCModel(const struct Datas2OpenGL& datas) :g_indices(datas.vert_indices), g_vertices(datas.verts), g_normals(datas.vert_normals2), c_indices(datas.search_m), edge_indices(datas.edge_indices) {
			clock_t start, end;
			start = clock();
			Vector<CompState>().swap(comp_states);
			comp_states.resize(c_indices.size(), VIS);
			
			size_t facs = datas.face_mat.size(); // 获取面的数量

			material_data.resize(facs);
			for (size_t i = 0; i < facs; i++) {
				material_data[i].kd = glm::vec3(datas.face_mat[i].ka_r, datas.face_mat[i].ka_g, datas.face_mat[i].ka_b);
				material_data[i].ks = glm::vec3(datas.face_mat[i].ks_r, datas.face_mat[i].ks_g, datas.face_mat[i].ks_b);
				material_data[i].ns = datas.face_mat[i].ns;
				material_data[i].alpha = datas.face_mat[i].a;
			}
			getVerColor();					// 生成顶点颜色数组
			generateCompIds();				// 生成顶点到其包含物件的映射
			generate_bbxs_by_comps();		// 生成各个物件的bbx
			getVerAttrib();					// 生成顶点属性数组
			divide_model_by_alpha();		// 根据透明度将顶点分为两组
			generate_edges_by_msMeshes();	// 生成边
			end = clock();
			std::cout << (double)(end - start) / CLOCKS_PER_SEC << "s used for oepnGL data generating\n";
		}

		IFCModel(Vector<uint32_t> _g_indices, Vector<Real> _g_vertices, Vector<Real> _g_normals, Vector<Vector<uint32_t>> _c_indices, Vector<float> _face_mat, Vector<uint32_t> _edge_indices) :
			g_indices(_g_indices), g_vertices(_g_vertices), g_normals(_g_normals), c_indices(_c_indices), edge_indices(_edge_indices)  {
			clock_t start, end;
			start = clock();
			Vector<CompState>().swap(comp_states);
			comp_states.resize(c_indices.size(), VIS);

			size_t faces = _face_mat.size() / 8;// 获取面的数量
			material_data.resize(faces);
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
				generate_edges_by_msMeshes();	// 生成边
			}
			end = clock();
			std::cout << (double)(end - start) / CLOCKS_PER_SEC << "s used for oepnGL data generating\n";
		}

		Vector<uint32_t> generate_edges_by_msMeshes() {
			Vector<Vector<uint32_t>>().swap(c_edge_indices);
			Vector<uint32_t>().swap(edge_indices);
			mesh_simplier::build_ms_vertices(g_vertices, g_normals); // 存储顶点的位置和法向量
			Vector<mesh_simplier::Mesh> meshes = mesh_simplier::generateMeshes(c_indices); // 根据c_indices 生成对应的 构件到对应要显示的edges的映射
			for (mesh_simplier::Mesh mes : meshes) {
#ifdef PAIRREP
				edge_indices.insert(edge_indices.end(), mes.edge_indexp.begin(), mes.edge_indexp.end());
				c_edge_indices.emplace_back(mes.edge_indexp);
#else
				new_edge_index.insert(new_edge_index.end(), mes.edge_index.begin(), mes.edge_index.end());
#endif
			}
			cur_edge_ind = edge_indices;
			return edge_indices;
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
		/*union tempa{
			Real f;
			int i;
		};*/
		// organize the datas of IfcModel into glVertexAttributes, for sending to GPU
		Vector<Real> getVerAttrib() {
			size_t s = g_vertices.size(); //xyzxyzxyz...
			Vector<Real>().swap(ver_attrib);
			ver_attrib.resize(s / 3 * 10);//no!
			int offset = 0;
			for (int i = 0; i < s; i += 3) {
				ver_attrib[offset + i] = g_vertices[i];								// 位置.x
				ver_attrib[offset + i + 1] = g_vertices[i + 1];						// 位置.y
				ver_attrib[offset + i + 2] = g_vertices[i + 2];						// 位置.z
				ver_attrib[offset + i + 3] = g_normals[i];							// 法向量.x
				ver_attrib[offset + i + 4] = g_normals[i + 1];						// 法向量.y
				ver_attrib[offset + i + 5] = g_normals[i + 2];						// 法向量.z
				ver_attrib[offset + i + 6] = g_kd_color[i];							// 颜色.x
				ver_attrib[offset + i + 7] = g_kd_color[i + 1];						// 颜色.y
				ver_attrib[offset + i + 8] = g_kd_color[i + 2];						// 颜色.z
				ver_attrib[offset + i + 9] = util::int_as_float(comp_ids[i / 3]);//todo: transform by bit // 所在物件的索引
				offset += 7;
			}
			return ver_attrib;
		}

		// just add compid attribute for each vertex on which component it is
		void generateCompIds() { // comp_ids: 顶点索引找到对应的物件索引
			Vector<uint32_t>().swap(comp_ids);
			comp_ids.resize(g_vertices.size() / 3);
			int j = 0;
			for (int i = 0; i < c_indices.size(); i++) {
				auto ix = c_indices[i];
				int s = ix.size();
				for (int j = 0; j < s; j++) {
					comp_ids[ix[j]] = i;
				}
			}
		}

		// add color attribute for each vertex
		Vector<Real>& getVerColor() {
			Vector<Real>().swap(g_kd_color);
			g_kd_color.resize(g_vertices.size());
			for (int i = 0; i < c_indices.size(); i++) {
				for (int j = 0; j < c_indices[i].size(); j++) {
					g_kd_color[3 * c_indices[i][j]] = material_data[i].kd.x;
					g_kd_color[3 * c_indices[i][j] + 1] = material_data[i].kd.y;
					g_kd_color[3 * c_indices[i][j] + 2] = material_data[i].kd.z;
				}
			}
			return g_kd_color;
		}
		//divide components into 2 vectors by their transparence
		void divide_model_by_alpha() {
			Vector<uint32_t> transparency_ind;
			Vector<uint32_t> no_transparency_ind;
			trans_c_indices_set.clear();
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
			Vector<Real>().swap(comps_bbx);
			comps_bbx.resize(cindicessize * 6);		// 每个物件对应6个float值
			Real a_min[3], a_max[3];
			int bbx_offset = 0;
			for (auto& ix : c_indices) {
				size_t s = ix.size();
				a_min[0] = a_min[1] = a_min[2] = numeric_limits<real_t>::max();
				a_max[0] = a_max[1] = a_max[2] = numeric_limits<real_t>::lowest();
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
			uint32_t c_indices_size = c_indices.size();
			for (auto& id : comp_indices) {
				if (id > c_indices_size || comps_bbx.size() == 0) {
					continue;
				}
				for (int i = 0; i < 3; i++) {
					ret[i] = std::min(ret[i], comps_bbx[6 * id + i]);
				}
				for (int i = 3; i < 6; i++) {
					ret[i] = std::max(ret[i], comps_bbx[6 * id + i]);
				}
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

		void PrecomputingCubeDirection() {
			m_cube_direction_transform.clear();
			m_cube_direction_transform.resize(6, glm::vec3(0.f, 0.f, 0.f));
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
			m_model = model;
			m_init_model = m_model;
			mirror_model = glm::mat4(1.f);
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

		Vector<CompState> comp_states;					// 记录每个comp的状态：隐藏、显示、高亮

		Vector<uint32_t> cur_chosen_trans_ind;			// 当前要高亮(多选)的透明顶点的索引
		Vector<uint32_t> cur_chosen_no_trans_ind;		// 当前要高亮(多选)的不透明顶点的索引

		Vector<uint32_t> cur_c_indices;					// 当前要显示的物件的索引
		Vector<uint32_t> cur_vis_trans_ind;				// 当前要显示的透明顶点的索引	
		Vector<uint32_t> cur_vis_no_trans_ind;			// 当前要显示的不透明顶点的索引	
		Vector<uint32_t> cur_edge_ind;					// 当前要显示的物件包含的边的索引

		unordered_set<uint32_t> trans_c_indices_set;	// 透明物体的索引, 用来快速分类，一次建立，多次查询

		Vector<uint32_t> bbx_drawing_order = { 0,1,5,4,0,2,6,4,5,7,3,1,3,2,6,7 }; // 按此定点顺序绘制bbx长方体框

	private:
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

		Vector<bool> is_trans;
		Vector<glm::vec3> m_cube_direction_transform;
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