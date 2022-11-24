//﻿#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

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

#include "common/ifc_util.h"
namespace ifcre {
	struct mySkyBox {
		GLuint skyboxVAO, skyboxVBO, skyboxEBO;
		GLuint skybox_texture_id;
		mySkyBox() {
			float k = 1.f;
			float vertex_attributes[] = {
				//position		//depth
				k,  k,  k,		1.0f,
				-k, k,  k,		1.0f,
				-k,-k,  k,		0.0f,
				k, -k,  k,		0.0f,
				k,  k, -k,		1.0f,
				-k, k, -k,		1.0f,
				-k,-k, -k,		0.0f,
				k, -k, -k,		0.0f
			};
			uint32_t vertex_indices[] = {
				0, 1, 2, 0, 2, 3,
				0, 4, 5, 0, 5, 1,
				3, 7, 4, 3, 4, 0,
				2, 6, 7, 2, 7, 3,
				1, 5, 6, 1, 6, 2,
				4, 7, 6, 4, 6, 5
			};

			glGenVertexArrays(1, &skyboxVAO);
			glBindVertexArray(skyboxVAO);
			glGenBuffers(1, &skyboxVBO);
			glGenBuffers(1, &skyboxEBO);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_attributes), &vertex_attributes, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), &vertex_indices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

		}

		void drawSkyBox() {

			//glDisable(GL_DEPTH_TEST);
			//glDepthFunc(GL_ALWAYS);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);

			glBindVertexArray(skyboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}
	};
}
#endif // !SKYBOX_H
