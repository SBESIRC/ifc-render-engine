#pragma once
#ifndef IFCRE_PARSER_H_
#define IFCRE_PARSER_H_

#include "OBJ_Loader.h.h"

#include "../common/std_types.h"
#include "model.h"

namespace ifcre {
	
	class IFCParser {
		// TODO
    public:
        static SharedPtr<IFCModel> load(String file) {
            glm::vec3 pMin, pMax;
            Vector<uint32_t> g_indices;
            Vector<Real> g_vertices;
            Vector<Real> g_normals;

			std::ifstream is(file.c_str(), std::ios::binary);

			//vertices
			size_t s;
			is.read((char*)&s, sizeof(size_t));
			g_vertices.resize(s);
			Real x_min, x_max, y_min, y_max, z_min, z_max;
			x_min = y_min = z_min = FLT_MAX;
			x_max = y_max = z_max = -FLT_MAX;
			for (int i = 0; i < s; i++) {
				is.read((char*)&g_vertices[i], sizeof(Real));
				switch (i % 3)
				{
				case 0:
				{
					x_min = std::min(x_min, g_vertices[i]);
					x_max = std::max(x_max, g_vertices[i]);
					break;
				}
				case 1: {
					y_min = std::min(y_min, g_vertices[i]);
					y_max = std::max(y_max, g_vertices[i]);
					break;
				}
				default: {
					z_min = std::min(z_min, g_vertices[i]);
					z_max = std::max(z_max, g_vertices[i]);
					break;
				}
				}
			}
			pMax = glm::vec3(x_max, y_max, z_max);
			pMin = glm::vec3(x_min, y_min, z_min);
			//normals
			is.read((char*)&s, sizeof(size_t));
			g_normals.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&g_normals[i], sizeof(Real));
			}

			//global_indices
			is.read((char*)&s, sizeof(size_t));
			g_indices.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&g_indices[i], sizeof(unsigned int));
				g_indices[i]--;
			}
			auto ret = make_shared<IFCModel>(g_indices, g_vertices, g_normals);
			ret->setBBX(pMin, pMax);
			ret->getVerAttrib();

			//indices by components
			is.read((char*)&s, sizeof(size_t));
			//ret->components.resize(s);
			size_t tmps;
			for (int i = 0; i < s; i++) {
				is.read((char*)&tmps, sizeof(size_t));
				Vector<uint32_t> tmp(tmps);
				for (int j = 0; j < tmps; j++) {
					is.read((char*)&tmp[j], sizeof(unsigned int));
					tmp[j]--;
				}
				ret->components.emplace_back(ComponentModel(tmp, tmps));
			}

			//materials' paragraph
			Vector<MtlData> mat_vec;
			is.read((char*)&s, sizeof(size_t));
			std::cout << s << std::endl;
			mat_vec.resize(s);
			for (int i = 0; i < s; i++) {
				is.read((char*)&mat_vec[i].data[0], sizeof(float) * 7);
				is.read((char*)&mat_vec[i].data[7], sizeof(int));
			}
			ret->setMaterialData(mat_vec);
			return ret;
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