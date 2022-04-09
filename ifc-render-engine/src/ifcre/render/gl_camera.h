#pragma once
#ifndef IFCRE_GL_CAMERA_H_
#define IFCRE_GL_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gl/gl_render_texture.h"
#include "../common/std_types.h"

namespace ifcre {
	class GLCamera {
	public:
		GLCamera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f))
            : m_pos(pos)
            , m_worldup(worldup)
            , m_front(glm::vec3(0.0f, 0.0f, -1.0f)) {
            reset();
            _updateCameraVectors();
        }

        glm::mat4 getViewMatrix() {
            return glm::lookAt(m_pos, m_pos + m_front, m_up);
        }

        glm::mat4 getModelMatrixByBBX(const glm::vec3 pMin,const glm::vec3 pMax) {
            //Real mm = std::max(pMax.x - pMin.x, std::max(pMax.y - pMin.y, pMax.z - pMin.z));
            Real scales = 1. / (pMax.x - pMin.x) * 15;
            glm::vec3 offset = glm::vec3(-(pMin.x + pMax.x) / 2, -(pMin.y + pMax.y) / 2, -(pMin.z + pMax.z) / 2);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(scales, scales, scales));
            //scale(scales, scales, scales);
            model = glm::translate(model, offset);
            return model;
        }

        void translateByScreenOp(float offx, float offy, float offz) {
            glm::vec3 t(0.0f);
            if (offx != 0) {
                m_pos += m_velocity.x * (offx > 0 ? m_right : -m_right);
            }
            if (offy != 0) {
                m_pos += m_velocity.y * (offy > 0 ? m_up : -m_up);
            }
            if (offz != 0) {
                m_pos += m_velocity.z * (offz > 0 ? m_front : -m_front);
            }
        }

        void reset() {
            //m_model = glm::mat4(1.0f);
        }

        void rotateByScreenX(glm::vec3& center, float angleX) {
            //m_model = glm::rotate(m_model, angle, axis);
            glm::mat4 trans(1.0f);
            glm::mat4 rot(1.0f);
            glm::mat4 inv_trans(1.0f);
            trans = glm::translate(trans, -center);
            rot = glm::rotate(rot, angleX, glm::vec3(0, 1, 0));
            inv_trans = glm::translate(inv_trans, center);
            glm::vec3 new_pos = inv_trans * rot * trans * glm::vec4(m_pos, 1.0f);
            _lookCenterX(center, new_pos);
        }

        void translate(Real x, Real y) {
            glm::vec3 trans = x * m_right + y * m_up;
            trans *= 0.005;
            m_pos += trans;
        }
        
        glm::vec3 getViewPos() {
            return m_pos;
        }

	private:
        void _updateCameraVectors()
        {
            //glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)
            // calculate the new Front vector
            //glm::vec3 front;
            //front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            //front.y = sin(glm::radians(Pitch));
            //front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            //Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            m_right = glm::normalize(glm::cross(m_front, m_worldup)); 
            m_up = glm::normalize(glm::cross(m_right, m_front));
        }

        // Rodrigues formula
        void _lookCenterX(glm::vec3& center, glm::vec3& new_pos) {
            //m_front = glm::normalize(center - m_pos);
            glm::vec3 to_center = glm::normalize(center - m_pos);
            to_center.y = 0;
            glm::vec3 v = glm::cross(to_center, m_front);
            Real s = glm::length(v), c = glm::dot(to_center, m_front);
            glm::mat3 vx = glm::mat3(
                0, v.z, -v.y,
                -v.z, 0, v.x,
                v.y, -v.x, 0
            );
            glm::mat3 R = glm::mat3(1.0) + vx + vx * vx / (1 + c);

            glm::vec3 next_to_center = glm::normalize(center - new_pos);
            next_to_center.y = 0;
            m_front = R * next_to_center;
            m_pos = new_pos;
            _updateCameraVectors();
        }

        // ------------- camera attributes----------------
        glm::vec3 m_pos;
        glm::vec3 m_front;
        // calculated by worldup 
        glm::vec3 m_up;
        glm::vec3 m_right;

        glm::vec3 m_worldup;

        const glm::vec3 m_velocity = glm::vec3(0.05f, 0.05f, 0.2f);
        //glm::mat4 m_model;
        // ----- ----- ----- ----- ----- ----- -----  -----
	};
}

#endif