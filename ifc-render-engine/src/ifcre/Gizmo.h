#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "common/ifc_util.h"
namespace ifcre {
	static std::vector<uint32_t> counter_clockwise_cube_element_object_buffer = {
		//back			 //left			   //front
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
		//right				   //up					   //down
		12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 22, 21, 20, 23, 22
	};
	struct SceneGizmo {
		GLuint gizmoVAO, gizmoVBO, gizmoEBO;
		GLuint gizmo_textID;
		SceneGizmo() {
			float k = 1.f;
			float coord_plane[] = {
				//	// position     uv cordination	ids
				//	//back
				//	 k, -k, -k,		.00f, .00f,		util::int_as_float(1),
				//	-k, -k, -k,		.33f, .00f,		util::int_as_float(1),
				//	-k,  k, -k,		.33f, .50f,		util::int_as_float(1),
				//	 k,  k, -k,		.00f, .50f,		util::int_as_float(1),
				//	//left
				//	-k, -k,  k,		1.0f, .50f,		util::int_as_float(2),
				//	-k,  k,  k,		1.0f, 1.0f,		util::int_as_float(2),
				//	-k,  k, -k,		.67f, 1.0f,		util::int_as_float(2),
				//	-k, -k, -k,		.67f, .50f,		util::int_as_float(2),
				//	//front
				//	-k, -k,  k,		.00f, .50f,		util::int_as_float(3),
				//	 k, -k,  k,		.33f, .50f,		util::int_as_float(3),
				//	 k,  k,  k,		.33f, 1.0f,		util::int_as_float(3),
				//	-k,  k,  k,		.00f, 1.0f,		util::int_as_float(3),
				//	//right
				//	 k, -k,  k,		.67f, .00f,		util::int_as_float(4),
				//	 k, -k, -k,		1.0f, .00f,		util::int_as_float(4),
				//	 k,  k, -k,		1.0f, .50f,		util::int_as_float(4),
				//	 k,  k,  k,		.67f, .50f,		util::int_as_float(4),
				//	//up
				//	-k,  k,  k,		.33f, .50f,		util::int_as_float(5),
				//	 k,  k,  k,		.67f, .50f,		util::int_as_float(5),
				//	 k,  k, -k,		.67f, 1.0f,		util::int_as_float(5),
				//	-k,  k, -k,		.33f, 1.0f,		util::int_as_float(5),
				//	//down
				//	 k, -k, -k,		.67f, .00f,		util::int_as_float(6),
				//	-k, -k, -k,		.33f, .00f,		util::int_as_float(6),
				//	-k, -k,  k,		.33f, .50f,		util::int_as_float(6),
				//	 k, -k,  k,		.67f, .50f,		util::int_as_float(6)
					// position     uv cordination	ids
					//back
					 k, -k, -k,		.00f, .00f,		0.f,
					-k, -k, -k,		.33f, .00f,		0.f,
					-k,  k, -k,		.33f, .50f,		0.f,
					 k,  k, -k,		.00f, .50f,		0.f,
					 //left
					 -k, -k,  k,		1.0f, .50f,		1.f,
					 -k,  k,  k,		1.0f, 1.0f,		1.f,
					 -k,  k, -k,		.67f, 1.0f,		1.f,
					 -k, -k, -k,		.67f, .50f,		1.f,
					 //front
					 -k, -k,  k,		.00f, .50f,		2.f,
					  k, -k,  k,		.33f, .50f,		2.f,
					  k,  k,  k,		.33f, 1.0f,		2.f,
					 -k,  k,  k,		.00f, 1.0f,		2.f,
					 //right
					  k, -k,  k,		.67f, .00f,		3.f,
					  k, -k, -k,		1.0f, .00f,		3.f,
					  k,  k, -k,		1.0f, .50f,		3.f,
					  k,  k,  k,		.67f, .50f,		3.f,
					  //up
					  -k,  k,  k,		.33f, .50f,		4.f,
					   k,  k,  k,		.67f, .50f,		4.f,
					   k,  k, -k,		.67f, 1.0f,		4.f,
					  -k,  k, -k,		.33f, 1.0f,		4.f,
					  //down
					   k, -k, -k,		.67f, .00f,		5.f,
					  -k, -k, -k,		.33f, .00f,		5.f,
					  -k, -k,  k,		.33f, .50f,		5.f,
					   k, -k,  k,		.67f, .50f,		5.f
			};

			glGenVertexArrays(1, &gizmoVAO);
			glBindVertexArray(gizmoVAO);
			glGenBuffers(1, &gizmoVBO);
			glGenBuffers(1, &gizmoEBO);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_plane), &coord_plane, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmoEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, counter_clockwise_cube_element_object_buffer.size() * sizeof(uint32_t), counter_clockwise_cube_element_object_buffer.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));

			//texture
			glGenTextures(1, &gizmo_textID);
			glBindTexture(GL_TEXTURE_2D, gizmo_textID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			unsigned char* data = stbi_load("resources\\textures\\scenegizmo.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void drawGizmo() {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glActiveTexture(GL_TEXTURE0);
			glDisable(DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			glDepthMask(GL_FALSE);

			glBindVertexArray(gizmoVAO);
			glBindTexture(GL_TEXTURE_2D, gizmo_textID);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);

			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDepthMask(GL_TRUE);
			glDisable(GL_CULL_FACE);
		}

		void drawGizmoInUiLayer() {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);

			glBindVertexArray(gizmoVAO);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDisable(GL_CULL_FACE);
		}
	};
}