#pragma once
#ifndef IFCRE_PARSER_H_
#define IFCRE_PARSER_H_

#include "OBJ_Loader.h.h"

#include "../common/std_types.h"
#include "model.h"

namespace ifcre {
	
	class IFCParser {
		// TODO
	};

	class DefaultParser {
	public:
		static DefaultModel load(String file) {
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

                    vertices.push_back(curMesh.Vertices[j].TextureCoordinate.X);
                    vertices.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
                }

                // Go through every 3rd index and print the
                //	triangle that these indices represent
                for (int j = 0; j < curMesh.Indices.size(); j += 3) {
                    indices.push_back(curMesh.Indices[j]);
                    indices.push_back(curMesh.Indices[j + 1]);
                    indices.push_back(curMesh.Indices[j + 2]);
                    // file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
                }
            }

            return DefaultModel(indices, vertices);
		}
	};
}

#endif