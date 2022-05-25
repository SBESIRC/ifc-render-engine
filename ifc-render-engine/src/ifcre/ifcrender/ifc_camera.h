#pragma once
#ifndef _IFCRE_CAMERA_H_
#define _IFCRE_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../common/std_types.h"

namespace ifcre {

	class IFCCamera {
    public:
        IFCCamera(glm::vec3 pos, glm::vec3 worldup, float fov, float aspect, float near, float far, bool need_y_flip)
            : m_pos(pos)
            , m_worldup(worldup)
            , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
            , m_fov(fov)
            , m_aspect(aspect)
            , m_near(near)
            , m_far(far)
            , m_needYFlip(need_y_flip){
            _updateCameraVectors();
            _updateViewMatrix();
            _updateProjectionMatrix();
        }

        glm::mat4 getViewMatrix() {
            return m_view;
        }

        glm::mat4 getProjMatrix() {
            return m_projection;
        }

        void zoom(glm::vec3& target, Real d) {
            glm::vec3 dir = glm::normalize(target - m_pos) * d;
            float vel = glm::length(target - m_pos) * 0.15;
            m_pos += vel * dir;
            _updateViewMatrix();
        }

        glm::vec3 getViewPos() { return m_pos; }

        void setAspect(float aspect) { 
            m_aspect = aspect;
            _updateProjectionMatrix();
        }

    private:
        typedef glm::mat4(*FuncGetPerspProj)(void);
        void _updateCameraVectors()
        {
            // also re-calculate the Right and Up vector
            m_right = glm::normalize(glm::cross(m_front, m_worldup));
            m_up = glm::normalize(glm::cross(m_right, m_front));
        }

        void _updateViewMatrix() {
            m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
        }

        void _updateProjectionMatrix() {
            m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
            m_projection[1][1] *= m_needYFlip ? -1 : 1;
        }

        // ------------- camera attributes----------------
        glm::vec3 m_pos;
        glm::vec3 m_front;
        // calculated by worldup 
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_worldup;

        FuncGetPerspProj f_getPerspProj;

        float m_near;
        float m_far;
        float m_fov;
        float m_aspect;

        bool m_needYFlip;
        
        glm::mat4 m_view;
        glm::mat4 m_projection;
        // ----- ----- ----- ----- ----- ----- -----  -----
	};
}

#endif