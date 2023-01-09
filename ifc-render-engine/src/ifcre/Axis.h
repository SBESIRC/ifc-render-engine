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
#include "common/ifc_util.h"
namespace ifcre {
	struct EngineAxis {
		GLuint axis_vao, axis_vbo;
		glm::mat4 axis_model;
		EngineAxis() { // 初始化 (一般只运行一次。除非物体频繁改变)
			float coord_axis[] = {
				0.0, 0.0, 0.0,
				-1.0, 0.0, 0.0,	// x-axis
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0,	// y-axis
				0.0, 0.0, 0.0,
				0.0, 0.0, 1.0	// z-axis
			};
			glGenVertexArrays(1, &axis_vao);
			glGenBuffers(1, &axis_vbo); // 创建一个缓冲
			glBindVertexArray(axis_vao); // 绑定VAO
			glBindBuffer(GL_ARRAY_BUFFER, axis_vbo); // 设置缓冲类型
			glBufferData(GL_ARRAY_BUFFER, sizeof(coord_axis), &coord_axis, GL_STATIC_DRAW); // 把用户定义的数据复制到当前绑定缓冲(显存)
			glEnableVertexAttribArray(0); // 以顶点属性位置值作为参数，启用顶点属性
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上） 从此时绑定到GL_ARRAY_BUFFER的VBO获取数据
		}

		void update_model_mat_info(glm::mat4 init_model, glm::vec3 model_center, float scale_factor,
			glm::vec3 pick_center, glm::vec3 view_pos, glm::vec3 init_view_pos) {
			glm::mat4 trans_center(1.0f);
			trans_center = glm::translate(trans_center, model_center);
			axis_model = init_model * trans_center;
			glm::vec3 world_pos(axis_model[3][0], axis_model[3][1], axis_model[3][2]);

			/*float backscale = 15.f / scale_factor;
			float len = glm::length(view_pos - pick_center);
			backscale = (backscale - 20.f) / 7.f * len/16.f;*/
			float backscale = glm::length(view_pos - pick_center) / glm::length(init_view_pos) / scale_factor * 0.25f;
			axis_model = glm::scale(axis_model, glm::vec3(backscale, backscale, backscale));

			glm::mat4 trans_click_center(1.0f);
			trans_click_center = glm::translate(trans_click_center, pick_center - world_pos);
			axis_model = trans_click_center * axis_model;
		}

		void drawAxis() {
			glBindVertexArray(axis_vao);// 使用上面那一套VAO
			glDisable(GL_DEPTH_TEST);
			glDepthFunc(GL_ALWAYS);
			glDrawArrays(GL_LINES, 0, 6);// 使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元
		}
	};
}
