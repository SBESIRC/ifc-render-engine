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
		float k = .3f;
		SceneGizmo(int id_num = 0) {
			float coord_plane[] = {
				// position     uv cordination	ids
				//back
				 k, -k, -k,		.00f, .00f,		util::int_as_float(id_num + 0),
				-k, -k, -k,		.33f, .00f,		util::int_as_float(id_num + 0),
				-k,  k, -k,		.33f, .50f,		util::int_as_float(id_num + 0),
				 k,  k, -k,		.00f, .50f,		util::int_as_float(id_num + 0),
				 //left
				 -k, -k,  k,		1.0f, .50f,		util::int_as_float(id_num + 1),
				 -k,  k,  k,		1.0f, 1.0f,		util::int_as_float(id_num + 1),
				 -k,  k, -k,		.67f, 1.0f,		util::int_as_float(id_num + 1),
				 -k, -k, -k,		.67f, .50f,		util::int_as_float(id_num + 1),
				 //front
				 -k, -k,  k,		.00f, .50f,		util::int_as_float(id_num + 2),
				  k, -k,  k,		.33f, .50f,		util::int_as_float(id_num + 2),
				  k,  k,  k,		.33f, 1.0f,		util::int_as_float(id_num + 2),
				 -k,  k,  k,		.00f, 1.0f,		util::int_as_float(id_num + 2),
				 //right
				  k, -k,  k,		.67f, .00f,		util::int_as_float(id_num + 3),
				  k, -k, -k,		1.0f, .00f,		util::int_as_float(id_num + 3),
				  k,  k, -k,		1.0f, .50f,		util::int_as_float(id_num + 3),
				  k,  k,  k,		.67f, .50f,		util::int_as_float(id_num + 3),
				  //up
				  -k,  k,  k,		.33f, .50f,		util::int_as_float(id_num + 4),
				   k,  k,  k,		.67f, .50f,		util::int_as_float(id_num + 4),
				   k,  k, -k,		.67f, 1.0f,		util::int_as_float(id_num + 4),
				  -k,  k, -k,		.33f, 1.0f,		util::int_as_float(id_num + 4),
				  //down
				   k, -k, -k,		.67f, .00f,		util::int_as_float(id_num + 5),
				  -k, -k, -k,		.33f, .00f,		util::int_as_float(id_num + 5),
				  -k, -k,  k,		.33f, .50f,		util::int_as_float(id_num + 5),
				   k, -k,  k,		.67f, .50f,		util::int_as_float(id_num + 5)
				   //// position     uv cordination	ids
				   ////back
				   // k, -k, -k,		.00f, .00f,		id_num + 0.f,
				   //-k, -k, -k,		.33f, .00f,		id_num + 0.f,
				   //-k,  k, -k,		.33f, .50f,		id_num + 0.f,
				   // k,  k, -k,		.00f, .50f,		id_num + 0.f,
				   ////left
				   //-k, -k,  k,		1.0f, .50f,		id_num + 1.f,
				   //-k,  k,  k,		1.0f, 1.0f,		id_num + 1.f,
				   //-k,  k, -k,		.67f, 1.0f,		id_num + 1.f,
				   //-k, -k, -k,		.67f, .50f,		id_num + 1.f,
				   ////front
				   //-k, -k,  k,		.00f, .50f,		id_num + 2.f,
				   // k, -k,  k,		.33f, .50f,		id_num + 2.f,
				   // k,  k,  k,		.33f, 1.0f,		id_num + 2.f,
				   //-k,  k,  k,		.00f, 1.0f,		id_num + 2.f,
				   ////right
				   // k, -k,  k,		.67f, .00f,		id_num + 3.f,
				   // k, -k, -k,		1.0f, .00f,		id_num + 3.f,
				   // k,  k, -k,		1.0f, .50f,		id_num + 3.f,
				   // k,  k,  k,		.67f, .50f,		id_num + 3.f,
				   ////up
				   //-k,  k,  k,		.33f, .50f,		id_num + 4.f,
				   // k,  k,  k,		.67f, .50f,		id_num + 4.f,
				   // k,  k, -k,		.67f, 1.0f,		id_num + 4.f,
				   //-k,  k, -k,		.33f, 1.0f,		id_num + 4.f,
				   ////down
				   // k, -k, -k,		.67f, .00f,		id_num + 5.f,
				   //-k, -k, -k,		.33f, .00f,		id_num + 5.f,
				   //-k, -k,  k,		.33f, .50f,		id_num + 5.f,
				   // k, -k,  k,		.67f, .50f,		id_num + 5.f
			};

			glGenVertexArrays(1, &gizmoVAO);
			glBindVertexArray(gizmoVAO);
			glGenBuffers(1, &gizmoVBO);
			glGenBuffers(1, &gizmoEBO);
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_plane), &coord_plane, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gizmoEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, counter_clockwise_cube_element_object_buffer.size() * sizeof(uint32_t), counter_clockwise_cube_element_object_buffer.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0); //  glVertexAttribPointer(shader位置，每段数据长，x，x，步长，起始位置)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // uv cordination
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float))); // ids

			//texture
			glGenTextures(1, &gizmo_textID); // 要生成的纹理数量和id数组
			glBindTexture(GL_TEXTURE_2D, gizmo_textID); //绑定
			// 为当前绑定的纹理对象设置环绕、过滤方式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // S(x)坐标多余部分镜像重复
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // T(y)坐标多余部分镜像重复
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 缩小线性过滤（推荐GL_NEAREST就近过滤）
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 放大线性过滤
			// 加载并生成纹理
			int width, height, nrChannels;
			unsigned char* data = stbi_load("resources\\textures\\scenegizmo.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 生成目标纹理、多级渐远级别、gl纹理存储格式、、、
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
			glActiveTexture(GL_TEXTURE0); // 只有一个纹理则默认激活
			glDisable(DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			glDepthMask(GL_FALSE);

			glBindVertexArray(gizmoVAO);
			glBindTexture(GL_TEXTURE_2D, gizmo_textID); // 绑定id到当前GL_TEXTURE_2D
			glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);

			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0); // 把纹理赋值给片段着色器的采样器

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
			glDrawElements(GL_TRIANGLES, counter_clockwise_cube_element_object_buffer.size(), GL_UNSIGNED_INT, 0); // 传入EBO

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDisable(GL_CULL_FACE);
		}

		glm::mat4 private_transform(const glm::vec2 window_size) {
			//glm::vec3 newpos = glm::vec3(1.f - k, k - 1.f, 0);
			glm::vec3 window_scale = glm::vec3(window_size.y / window_size.x, 1.f, 1.f) * .225f; // .2可以设置gizmo大小
			glm::vec3 newpos = glm::vec3(0.f, 0.f, -.5f);

			glm::mat4 ret = glm::translate(glm::scale(glm::mat4(1.f), window_scale), newpos);
			return ret;
			/*ret = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), newpos, glm::vec3(0.f, 1.f, 0.f)) * ret;
			return ret;*/
		}
	};
}