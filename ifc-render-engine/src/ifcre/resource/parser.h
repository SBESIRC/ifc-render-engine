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
#ifdef _DEBUG
            auto ret = make_shared<IFCModel>(file);
#else
            auto ge = generateIFCMidfile(file);
            auto ret = make_shared<IFCModel>(ge);
#endif
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