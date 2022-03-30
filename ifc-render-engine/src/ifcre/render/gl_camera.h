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
            , m_worldup(worldup){
            reset();
            _updateCameraVectors();
        }

        glm::mat4 getModelViewMatrix()
        {
            // TODO
            glm::mat4 res = m_model * glm::lookAt(m_pos, m_pos + m_front, m_up);

            return res;
        }
        glm::mat4 setModelMatrixByBBX(const glm::vec3 pMin,const glm::vec3 pMax) {
            //Real mm = std::max(pMax.x - pMin.x, std::max(pMax.y - pMin.y, pMax.z - pMin.z));
            Real scales = 1. / (pMax.x - pMin.x);
            glm::vec3 offset = glm::vec3(-(pMin.x + pMax.x) / 2, -(pMin.y + pMax.y) / 2, -(pMin.z + pMax.z) / 2);
            m_model = glm::mat4(1.0f);
            scale(scales, scales, scales);
            m_model = glm::translate(m_model, offset);
            return m_model;
        }

        void translateByScreenOp(float offx, float offy, float offz) {
            glm::vec3 t(0.0f);
            if (offx != 0) {
                m_pos += offx > 0 ? m_right : -m_right;
            }
            if (offy != 0) {
                m_pos += offy > 0 ? m_up : -m_up;
            }
            if (offz != 0) {
                m_pos += 0.2f * (offz > 0 ? m_front : -m_front);
            }
        }

        void reset() {
            m_model = glm::mat4(1.0f);
        }

        void rotate(const glm::vec3& axis, float angle) {
            m_model = glm::rotate(m_model, angle, axis);
        }
        void raotate(Real x, Real y) {
            //not completed
            glm::mat4 rot = glm::mat4(1.);
            rot = glm::rotate(rot, glm::radians(x), m_worldup);
            rot = glm::rotate(rot, glm::radians(y), m_right);
            //m_pos = rot*glm::vec4(m_pos,1);
        }

        // TODO
        void lookCenter(glm::vec3& center) {
            m_center = center;
        }

        void scale(Real x, Real y, Real z)
        {
            m_model = glm::mat4(
                    x, 0, 0, 0,
                    0, y, 0, 0,
                    0, 0, z, 0,
                    0, 0, 0, 1) * m_model;
        }
        void translate(Real x, Real y) {
            glm::vec3 trans = x * m_right + y * m_up;
            trans *= 0.005;
            m_pos += trans;
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
            m_front = glm::vec3(0.0f, 0.0f, -1.0f);
            m_right = glm::normalize(glm::cross(m_front, m_worldup)); 
            m_up = glm::normalize(glm::cross(m_right, m_front));
        }

        // ------------- camera attributes----------------
        glm::vec3 m_pos;
        glm::vec3 m_front;
        // calculated by worldup 
        glm::vec3 m_up;
        glm::vec3 m_right;

        glm::vec3 m_worldup;
        glm::mat4 m_model;
        glm::vec3 m_center;
        // ----- ----- ----- ----- ----- ----- -----  -----
	};
}

#endif