#pragma once

#ifndef CLIPS_H_
#define CLIPS_H_
#include <GLFW/glfw3.h>
#include <iostream>
#include "./common/ifc_util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ifcre {

	struct ClipPlane {
	public:
		glm::vec3 normal;
		glm::vec3 front;
		glm::vec3 right;
		Real moveSpeed = .01f;
		Real rotateSpeed = .001f;
		glm::vec3 base_pos;
		ClipPlane() {}
		~ClipPlane() {}
		ClipPlane(glm::vec4 p) {
			normal = p;
			base_pos = glm::vec3(0.);
			front = glm::vec3(0., 0., -1.);
			right = glm::normalize(glm::cross(front, normal));//1 0 0

		}
		glm::vec4 out_as_vec4() {
			return glm::vec4(normal, glm::dot(normal, base_pos));
		}
		void rotateByNormal(float angleA) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleA, normal);
			front = rot * glm::vec4(front, 1.f);
			right = rot * glm::vec4(right, 1.f);
		}

		void rotateByFront(float angleB) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleB, front);
			normal = rot * glm::vec4(normal, 1.f);
			right = rot * glm::vec4(right, 1.f);
		}

		void rotateByRight(float angleC) {
			glm::mat4 rot(1.0f);
			rot = glm::rotate(rot, angleC, right);
			normal = rot * glm::vec4(normal, 1.f);
			front = rot * glm::vec4(front, 1.f);
		}
		glm::mat4 toMat() {
			//glm::mat4 ret(1.0f);
			//ret = glm::lookAt(base_pos, base_pos + front, normal);
			//ret = glm::lookAt(front, glm::vec3(0.),normal);
			glm::vec4 world_x = glm::vec4(-glm::cross(normal, front), 0.),
				world_z = glm::vec4(-front, 0.),
				world_y = glm::vec4(normal, 0.);
			glm::vec4 add(base_pos, 1.);
			//glm::vec4 add(0., 0., 0., 1.);
			glm::mat4 basis(world_x, world_y, world_z, add);
			//basis = glm::translate(basis, base_pos);
			return basis;
		}

	protected:
		void _updateVectors() {
			normal = glm::normalize(glm::cross(right, front));
		}
	};
	enum CLIPBOXUPDATE {
		back_inc = 0x00,
		back_dec = 0x01,
		left_inc = 0x02,
		left_dec = 0x03,
		front_inc = 0x04,
		front_dec = 0x05,
		right_inc = 0x06,
		right_dec = 0x07,
		up_inc = 0x08,
		up_dec = 0x09,
		down_inc = 0x0a,
		down_dec = 0x0b
	};
	struct ClipBox :public ClipPlane {
		uint32_t edge_vao, plane_vao;
		uint32_t edge_vbo, edge_ebo, plane_vbo, plane_ebo;
		Real length = 0, width = 0, height = 0;
		glm::vec3 pos_after_model_matrix;
		glm::mat4 cur_model_mat;
		Vector<uint32_t> cube_element_buffer_object = {
			//back			 //left			   //front
			0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
			//right				   //up					   //down
			12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };
		Vector<uint32_t> cube_element_edge_buffer_object = { 0,1,1,2,2,3,3,0, 4,5,5,6,6,7,7,4, 8,9,9,10,10,11,11,8,
			12,13,13,14,14,15,15,12, 16,17,17,18,18,19,19,16, 20,21,21,22,22,23,23,20,
			//points
			24, 25, 26, 27, 28, 29,
			//normals
			30, 31, 32, 33, 34, 35,
			36, 37, 38, 39, 40, 41
		};
		Vector<glm::vec4> face_normal = {
			//back							//left							//front
			glm::vec4(0.f,0.f,-1.f,1.f),	glm::vec4(-1.f,0.f,0.f,1.f),	glm::vec4(0.f,0.f,1.f,1.f),
			//right							//up							//down
			glm::vec4(1.f,0.f,0.f,1.f),		glm::vec4(0.f,1.f,0.f,1.f),		glm::vec4(0.f,-1.f,0.f,1.f)
		};
		ClipBox() {}
		~ClipBox() {}
		ClipBox(glm::vec3 pos, glm::vec3 up, glm::vec3 _right, Real len, Real wid, Real hei) :length(len), width(wid), height(hei)
			/*,base_pos(pos), normal(up), right(_right)*/ {
			base_pos = pos;
			normal = up;
			right = _right;
			front = glm::cross(up, right);
		}
		const void glInit(int& ui_id_num) {
			float k = .5f;
			/*Vector<float> coord_plane = {
				-k,-k, -k,
				 k,-k, -k,
				 k,-k, k,
				-k,-k, k,
				-k, k, -k,
				 k, k, -k,
				 k, k, k,
				-k, k, k
			};
			*/glGenVertexArrays(1, &edge_vao);
			glGenBuffers(1, &edge_vbo);
			glGenBuffers(1, &edge_ebo);
			/*glBindVertexArray(edge_vao);
			glBindBuffer(GL_ARRAY_BUFFER, edge_vbo);
			glBufferData(GL_ARRAY_BUFFER, coord_plane.size()*sizeof(float), coord_plane.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_element_edge_buffer_object.size() * sizeof(uint32_t), cube_element_edge_buffer_object.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);*/
			ui_id_num--;
			Vector<float>coord_plane = {
				//position		//ui_ids
				//back
				-k, -k, -k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				-k,	k,	-k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				k,	k,	-k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				k,	-k,	-k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				//left
				-k,	-k,	-k,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				-k,	-k,	k,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				-k,	k,	k,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				-k,	k,	-k,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				//front
				-k,	-k,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				k,	-k,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				k,	k,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				-k,	k,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				//right
				k,	-k,	k,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				k,	-k,	-k,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				k,	k,	-k,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				k,	k,	k,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				//up
				-k,	k,	k,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				k,	k,	k,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				k,	k,	-k,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				-k,	k,	-k,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				//down
				k,	-k,	k,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,
				-k,	-k,	k,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,
				-k,	-k,	-k,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,
				k,	-k,	-k,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,

				//midpoints
				0,  0, -k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				-k,	0,	0,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				0,	0,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				k,	0,	0,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				0,	k,	0,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				0,	-k,	0,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,

				//normals
				0,	0,	-k,		util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				0,	0,	-k * 2,	util::int_as_float(ui_id_num + 1)/*ui_id_num + 1.f*/,
				-k,	0,	0,		util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				-k * 2,	0,	0,	util::int_as_float(ui_id_num + 2)/*ui_id_num + 2.f*/,
				0,	0,	k,		util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				0,	0,	k * 2,	util::int_as_float(ui_id_num + 3)/*ui_id_num + 3.f*/,
				k,	0,	0,		util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				k * 2,	0,	0,	util::int_as_float(ui_id_num + 4)/*ui_id_num + 4.f*/,
				0,	k,	0,		util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				0,	k * 2,	0,	util::int_as_float(ui_id_num + 5)/*ui_id_num + 5.f*/,
				0,	-k,	0,		util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/,
				0,	-k * 2,	0,	util::int_as_float(ui_id_num + 6)/*ui_id_num + 6.f*/

			};
			face_normal = {
				//back							//left							//front
				glm::vec4(0.f,0.f,-k,1.f),	glm::vec4(-k,0.f,0.f,1.f),	glm::vec4(0.f,0.f,k,1.f),
				//right							//up							//down
				glm::vec4(k,0.f,0.f,1.f),		glm::vec4(0.f,k,0.f,1.f),		glm::vec4(0.f,-k,0.f,1.f)
			};
			glBindVertexArray(edge_vao);
			glBindBuffer(GL_ARRAY_BUFFER, edge_vbo);
			glBufferData(GL_ARRAY_BUFFER, coord_plane.size() * sizeof(float), coord_plane.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_element_edge_buffer_object.size() * sizeof(uint32_t), cube_element_edge_buffer_object.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenVertexArrays(1, &plane_vao);
			glGenBuffers(1, &plane_vbo);
			glGenBuffers(1, &plane_ebo);
			glBindVertexArray(plane_vao);
			glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
			glBufferData(GL_ARRAY_BUFFER, coord_plane.size() * sizeof(float), coord_plane.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer_object.size() * sizeof(uint32_t), cube_element_buffer_object.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			//ui_id_num += 6;
		}
		void bind_the_world_coordination(glm::mat4 model_mat) {
			cur_model_mat = model_mat;
		}
		void setBasePos(glm::vec3 pmin, glm::vec3 pmax) {
			base_pos = (pmin + pmax) / 2.f;
		}
		Vector<glm::vec4> out_as_vec4s() {
			//TODO: change vec4.w, and rewrite this in shader // DONE!
			pos_after_model_matrix = cur_model_mat * glm::vec4(base_pos, 1.f);
			return {
				glm::vec4(-normal,glm::dot(-normal,pos_after_model_matrix + normal * height * cur_model_mat[1][1] / 2.f)),
				glm::vec4(-right,glm::dot(-right,pos_after_model_matrix + right * width * cur_model_mat[1][1] / 2.f)),
				glm::vec4(-front,glm::dot(-front,pos_after_model_matrix + front * length * cur_model_mat[1][1] / 2.f)),
				glm::vec4(normal,glm::dot(normal,pos_after_model_matrix - normal * height * cur_model_mat[1][1] / 2.f)),
				glm::vec4(right,glm::dot(right,pos_after_model_matrix - right * width * cur_model_mat[1][1] / 2.f)),
				glm::vec4(front,glm::dot(front,pos_after_model_matrix - front * length * cur_model_mat[1][1] / 2.f))
			};
		}
		const glm::mat4 toMat() const {
			glm::vec4 world_x = glm::vec4(-glm::cross(normal, front), 0.),
				world_z = glm::vec4(-front, 0.),
				world_y = glm::vec4(normal, 0.);
			glm::vec4 add(base_pos, 1.);
			glm::mat4 basis(world_x, world_y, world_z, add);
			glm::mat4 ret(1.0f);
			ret = glm::scale(ret, glm::vec3(width, height, length));

			return basis * ret;
		}
#define upadte_rate (.5f * dis)
		void updateBox(int direct, float dis = .05f) {
			switch (direct)
			{
			case back_inc: {
				length += upadte_rate;
				base_pos += upadte_rate / 2.f * front;
				break;
			}
			case back_dec: {
				length -= upadte_rate;
				if (length < upadte_rate) {
					length = upadte_rate;
				}
				else {
					base_pos -= upadte_rate / 2.f * front;
				}
				break;
			}
			case left_inc: {
				width += upadte_rate;
				base_pos -= upadte_rate / 2.f * right;
				break;
			}
			case left_dec: {
				width -= upadte_rate;
				if (width < upadte_rate) {
					width = upadte_rate;
				}
				else {
					base_pos += upadte_rate / 2.f * right;
				}
				break;
			}
			case front_inc: {
				length += upadte_rate;
				base_pos -= upadte_rate / 2.f * front;
				break;
			}
			case front_dec: {
				length -= upadte_rate;
				if (length < upadte_rate) {
					length = upadte_rate;
				}
				else {
					base_pos += upadte_rate / 2.f * front;
				}
				break;
			}
			case right_inc: {
				width += upadte_rate;
				base_pos += upadte_rate / 2.f * right;
				break;
			}
			case right_dec: {
				width -= upadte_rate;
				if (width < upadte_rate) {
					width = upadte_rate;
				}
				else {
					base_pos -= upadte_rate / 2.f * right;
				}
				break;
			}
			case up_inc: {
				height += upadte_rate;
				base_pos += upadte_rate / 2.f * normal;
				break;
			}
			case up_dec: {
				height -= upadte_rate;
				if (height < upadte_rate) {
					height = upadte_rate;
				}
				else {
					base_pos -= upadte_rate / 2.f * normal;
				}
				break;
			}
			case down_inc: {
				height += upadte_rate;
				base_pos -= upadte_rate / 2.f * normal;
				break;
			}
			case down_dec: {
				height -= upadte_rate;
				if (height < upadte_rate) {
					height = upadte_rate;
				}
				else {
					base_pos += upadte_rate / 2.f * normal;
				}
				break;
			}
			default:
				break;
			}
		}
		const void drawBox(const bool this_flag) const {

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			glBindVertexArray(edge_vao);
			glDepthMask(GL_FALSE);
			glBindBuffer(GL_ARRAY_BUFFER, edge_vbo);/*
			glDrawElements(GL_LINES, cube_element_edge_buffer_object.size(), GL_UNSIGNED_INT, 0);*/
			glDrawElements(GL_LINES, 48, GL_UNSIGNED_INT, 0);

			if (this_flag) {
				glPointSize(20.f);
				//glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, (void*)(48 * sizeof(uint32_t)));
				//glDrawElements(GL_LINES, 12, GL_UNSIGNED_INT, (void*)(54 * sizeof(uint32_t)));
			}

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		const void drawBoxInUILayer() const {
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			glBindVertexArray(plane_vao);
			glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
			glDrawElements(GL_TRIANGLES, cube_element_buffer_object.size(), GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDisable(GL_CULL_FACE);
		}
	};

	struct DrawingMatchPlane {
	public:
		glm::vec3 normal;
		glm::vec3 front;
		glm::vec3 right;
		//sglm::vec3 base_pos;
		glm::mat4 cur_model_mat;
		glm::vec3 model_center;
		DrawingMatchPlane() {}
		~DrawingMatchPlane() {}
		DrawingMatchPlane(glm::vec4 p) {
			normal = p;
			//base_pos = glm::vec3(0.);
			front = glm::vec3(0., 0., -1.);
			right = glm::normalize(glm::cross(front, normal));
		}

		void operator+=(glm::vec4 p)
		{
			normal += glm::vec3(p);
		}

		void bind_the_world_coordination(glm::mat4 model_mat) {
			cur_model_mat = model_mat;
		}

		void get_center(glm::vec3 center)
		{
			model_center = center;
		}
		glm::vec4 to_vec4()
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, model_center);
			return model * glm::vec4(normal, 1.0);
		}
		void update_my_self(int input) {
			switch (input)
			{
			case 0:
				normal += glm::vec3(0., .05, 0.);
				break;
			case 1:
				normal -= glm::vec3(0., .05, 0.);
				break;
			default:
				break;
			}
		}
	};
}
#endif
