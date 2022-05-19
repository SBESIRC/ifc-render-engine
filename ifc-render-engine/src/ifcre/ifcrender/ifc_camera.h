#pragma once
#ifndef _IFCRE_CAMERA_H_
#define _IFCRE_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../common/std_types.h"

namespace ifcre {
	class IFCCamera {
    public:
        IFCCamera(glm::vec3 pos, glm::vec3 worldup, float fov, float aspect, float near, float far)
            : m_pos(pos)
            , m_worldup(worldup)
            , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
            , m_fov(fov)
            , m_aspect(aspect)
            , m_near(near)
            , m_far(far){
            _updateCameraVectors();
        }

        glm::mat4 getViewMatrix() {
            return glm::lookAt(m_pos, m_pos + m_front, m_up);
        }

        glm::mat4 getProjMatrix() {
            glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
            projection[1][1] *= -1;
            return projection;
        }

        void zoom(glm::vec3& target, Real d) {
            glm::vec3 dir = glm::normalize(target - m_pos) * d;
            float vel = glm::length(target - m_pos) * 0.15;
            m_pos += vel * dir;
        }

        glm::vec3 getViewPos() { return m_pos; }

    private:
        void _updateCameraVectors()
        {
            // also re-calculate the Right and Up vector
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

        float m_near;
        float m_far;
        float m_fov;
        float m_aspect;
        // ----- ----- ----- ----- ----- ----- -----  -----
	};
}

#endif