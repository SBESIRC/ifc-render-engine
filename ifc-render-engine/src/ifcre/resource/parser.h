#pragma once
#ifndef IFCRE_PARSER_H_
#define IFCRE_PARSER_H_

#include "OBJ_Loader.h"
//#include "../collider.h"
#include "../common/std_types.h"
#include "model.h"

namespace ifcsaver {
	void save_string_into_binary(const string& source, ofstream& os) {
		size_t s = sizeof(char) * source.length();
		os.write((const char*)&s, sizeof(size_t));
		os.write(source.c_str(), s);
	}

	string read_string_from_binary(ifstream& is) {
		size_t s;
		char str[10000];
		is.read((char*)&s, sizeof(size_t));
		is.read(str, s);
		string ret(str, s / sizeof(char));
		return ret;
	}

	template <typename T>
	void save_meta_into_binary(const T& source, ofstream& os) {
		os.write((const char*)&source, sizeof(T));
	}
	template <typename T>
	T read_meta_from_binary(ifstream& is) {
		T ret;
		is.read((char*)&ret, sizeof(T));
		return ret;
	}

	template <typename T>
	void save_vector_into_binary(const vector<T>& v, ofstream& os) {
		size_t s = v.size();
		os.write((const char*)&s, sizeof(size_t));
		os.write((const char*)&v[0], s * sizeof(T));
	}

	template <typename T>
	vector<T> read_vector_from_binary(ifstream& is) {
		size_t s;
		is.read((char*)&s, sizeof(size_t));
		vector<T> ret(s);
		is.read((char*)&ret[0], sizeof(T) * s);
		return ret;
	}

	void save_properties_into_binary(const unordered_map<string, string>& source, ofstream& os) {
		size_t s = source.size();
		os.write((const char*)&s, sizeof(size_t));
		for (auto& pset : source) {
			save_string_into_binary(pset.first, os);
			save_string_into_binary(pset.second, os);
		}
	}

	unordered_map<string, string> read_properties_from_binary(ifstream& is) {
		unordered_map<string, string> ret;
		size_t s = read_meta_from_binary<size_t>(is);
		for (size_t i = 0; i < s; i++) {
			string first = read_string_from_binary(is);
			ret[first] = read_string_from_binary(is);
		}
		return ret;
	}

	void save_datas4Component_into_binary(const Datas4Component& source, ofstream& os) {
		save_meta_into_binary<unsigned int>(source.count_id, os);
		save_meta_into_binary<unsigned int>(source.edge_index_start, os);
		save_meta_into_binary<unsigned int>(source.edge_index_end, os);
		save_meta_into_binary<unsigned int>(source.mat_index_start, os);
		save_meta_into_binary<unsigned int>(source.mat_index_end, os);

		save_string_into_binary(source.name, os);
		save_string_into_binary(source.description, os);
		save_string_into_binary(source.guid, os);
		save_string_into_binary(source.type, os);

		save_meta_into_binary<size_t>(source.propertySet.size(), os);
		for (auto& pset : source.propertySet) {
			save_properties_into_binary(pset.propertySet, os);
		}
	}

	Datas4Component read_datas4Component_from_binary(ifstream& is) {
		Datas4Component ret;
		ret.count_id = read_meta_from_binary<unsigned int>(is);
		ret.edge_index_start = read_meta_from_binary<unsigned int>(is);
		ret.edge_index_end = read_meta_from_binary<unsigned int>(is);
		ret.mat_index_start = read_meta_from_binary<unsigned int>(is);
		ret.mat_index_end = read_meta_from_binary<unsigned int>(is);

		ret.name = read_string_from_binary(is);
		ret.description = read_string_from_binary(is);
		ret.guid = read_string_from_binary(is);
		ret.type = read_string_from_binary(is);

		ret.propertySet.resize(read_meta_from_binary<size_t>(is));
		for (size_t i = 0; i < ret.propertySet.size(); i++) {
			ret.propertySet[i].propertySet = read_properties_from_binary(is);
		}
		return ret;
	}

	void save_data2OpenGL_into_binary(const Datas2OpenGL& source, string filename) {
		ofstream os(filename.c_str(), ios::binary);
		save_vector_into_binary<unsigned int>(source.vert_indices, os);
		save_vector_into_binary<unsigned int>(source.edge_indices, os);
		size_t s = source.search_m.size();
		save_meta_into_binary<size_t>(s, os);
		for (size_t i = 0; i < s; i++) {
			save_vector_into_binary<unsigned int>(source.search_m[i], os);
		}
		save_vector_into_binary<real_t>(source.verts, os);
		save_vector_into_binary<real_t>(source.vert_normals2, os);
		save_vector_into_binary<Material_new>(source.face_mat, os);

		s = source.componentDatas.size();
		save_meta_into_binary<size_t>(s, os);
		for (size_t i = 0; i < s; i++) {
			save_datas4Component_into_binary(source.componentDatas[i], os);
		}
		os.close();
	}

	Datas2OpenGL read_datas2OpenGL_from_binary(ifstream& is) {
		Datas2OpenGL ret;
		ret.vert_indices = read_vector_from_binary<unsigned int>(is);
		ret.edge_indices = read_vector_from_binary<unsigned int>(is);
		ret.search_m.resize(read_meta_from_binary<size_t>(is));
		for (size_t i = 0; i < ret.search_m.size(); i++) {
			ret.search_m[i] = read_vector_from_binary<unsigned int>(is);
		}
		ret.verts = read_vector_from_binary<real_t>(is);
		ret.vert_normals2 = read_vector_from_binary<real_t>(is); // 读取法向量
		ret.face_mat = read_vector_from_binary<Material_new>(is); // 读取每个面上的信息

		ret.componentDatas.resize(read_meta_from_binary<size_t>(is));
		for (size_t i = 0; i < ret.componentDatas.size(); i++) {
			ret.componentDatas[i] = read_datas4Component_from_binary(is);
		}
		return ret;
	}
#ifdef _DEBUG
//this bbx generation fuction is for collision detection
vector<real_t> generate_bbx_for_collision(const Datas2OpenGL& datas) {
	size_t comp_size = datas.search_m.size();
	vector<real_t> ret(6 * comp_size);
	for (size_t i = 0; i < comp_size; i++) {
		real_t x_min, x_max, y_min, y_max, z_min, z_max;
		x_min = y_min = z_min = numeric_limits<real_t>::max();
		x_max = y_max = z_max = numeric_limits<real_t>::lowest();
		for (size_t j = 0; j < datas.search_m[i].size(); j++) {
			//x
			x_min = min(x_min, datas.verts[datas.search_m[i][j] * 3]);
			x_max = max(x_max, datas.verts[datas.search_m[i][j] * 3]);
			//y
			y_min = min(y_min, datas.verts[datas.search_m[i][j] * 3 + 1]);
			y_max = max(y_max, datas.verts[datas.search_m[i][j] * 3 + 1]);
			//z
			z_min = min(z_min, datas.verts[datas.search_m[i][j] * 3 + 2]);
			z_max = max(z_max, datas.verts[datas.search_m[i][j] * 3 + 2]);
		}
		ret[6 * i] = x_min, ret[6 * i + 1] = y_min, ret[6 * i + 2] = z_min;
		ret[6 * i + 3] = x_max, ret[6 * i + 4] = y_max, ret[6 * i + 5] = z_max;
	}
	return ret;
}
#endif
}
namespace ifcre {
	
	class IFCParser {
		// TODO
    public:
	   static SharedPtr<IFCModel> load(String file) {
		   if (endsWith(file, ".midfile")) {
			   ifstream is(file.c_str(), std::ios::binary);
			   Datas2OpenGL ge = ifcsaver::read_datas2OpenGL_from_binary(is);
			   is.close();
			   return make_shared<IFCModel>(ge);
		   }
		   else {
#if _DEBUG
			   file += ".midfile";
			   ifstream is(file.c_str(), std::ios::binary);
			   Datas2OpenGL ge = ifcsaver::read_datas2OpenGL_from_binary(is);
			   is.close();
			   return make_shared<IFCModel>(ge);
#else
			   Datas2OpenGL ge = generateIFCMidfile(file);
			   ifcsaver::save_data2OpenGL_into_binary(ge, file + ".midfile");
			   return make_shared<IFCModel>(ge);
#endif
		   }
	   }

//	   static SharedPtr<IFCModel> load(String file) {
//#ifdef _DEBUG
//		   file += ".midfile";
//		   ifstream is(file.c_str(), std::ios::binary);
//		   Datas2OpenGL ge = ifcsaver::read_datas2OpenGL_from_binary(is);
//		   is.close();
//#else
//		   Datas2OpenGL ge = generateIFCMidfile(file);
//		   ifcsaver::save_data2OpenGL_into_binary(ge, file + ".midfile");
//#endif
//		   auto ret = make_shared<IFCModel>(ge);
//		   //Collider collider;
//		   //collider.bufferData(&ge);
//		   //collider.addFilter([](const Datas4Component& hcg) {return true; });
//		   //collider.addCondition([](const Datas4Component& hcg1, const Datas4Component& hcg2) {return hcg1.type != hcg2.type; });
//		   //ret->collision_pairs = collider.getIndexArr();
//		   return ret;
//	   }

		static bool endsWith(const string s, const string sub) {
			return s.rfind(sub) == (s.length() - sub.length());
		}
	};

	class DefaultParser {
	public:
		static SharedPtr<DefaultModel> load(String file) {
			objl::Loader loader;
			loader.LoadFile(file);

            std::vector<unsigned int> indices;
            std::vector<float> vertices;
            // Go through each loaded mesh and out its contents
            for (int i = 0; i < loader.LoadedMeshes.size(); i++) {
                // Copy one of the loaded meshes to be our current mesh
                objl::Mesh curMesh = loader.LoadedMeshes[i];

                for (int j = 0; j < curMesh.Vertices.size(); j++) {
                    vertices.push_back(curMesh.Vertices[j].Position.X);
                    vertices.push_back(curMesh.Vertices[j].Position.Y);
                    vertices.push_back(curMesh.Vertices[j].Position.Z);

                    vertices.push_back(curMesh.Vertices[j].Normal.X);
                    vertices.push_back(curMesh.Vertices[j].Normal.Y);
                    vertices.push_back(curMesh.Vertices[j].Normal.Z);
                }


                indices = curMesh.Indices;
            }

            return make_shared<DefaultModel>(indices, vertices);
		}
	};
}
#endif