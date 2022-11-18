#pragma once
#ifndef IFCRE_GL_CAMERA_H_
#define IFCRE_GL_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../common/std_types.h"

namespace ifcre {

    enum class Camera_Movement {
        FORWARD,
        BACKWORD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

    class GLCamera {
    public:
        GLCamera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f))
            : m_pos(pos)
            , m_worldup(worldup)
            , m_front(glm::vec3(0.0f, 0.0f, -1.0f)) {
            reset();
            _updateCameraVectors();
        }

        glm::vec3 getViewUp() {
            return m_up;
        }

        glm::mat4 getViewMatrix() {
            //glm::mat4 ret(1.0f);
            return glm::lookAt(m_pos, m_pos + m_front, m_up); //lookAt(camera position, target, camera up) to careate a view matrix
        }

        glm::mat4 getPrecomputedViewMatrix(int i) {
            return glm::lookAt(m_precomputing_pos[i], m_precomputing_front[i], m_precomputing_up[i]);
        }

        glm::mat4 getCubeRotateMatrix(float dis = 0.f) {
            glm::vec3 v_pos = dis * glm::normalize(-m_front);
            return glm::lookAt(v_pos, v_pos + m_front, m_up);
        }

        void zoom(glm::vec3& target, Real d) {
            glm::vec3 dir = glm::normalize(target - m_pos) * d;
            float vel = glm::length(target - m_pos) * 0.15;
            m_pos += vel * dir;
        }

        void rotateInLocalSpace(glm::vec3& pick_center, float angle) {
            rotateAlongPoint(pick_center, glm::vec3(0, 1, 0), angle);
        }
        void rotateInWorldSpace(glm::vec3& pick_center, float angle) {
            rotateAlongPoint(pick_center, glm::vec3(1, 0, 0), angle);
        }
        void reset() {
            //m_model = glm::mat4(1.0f);
            m_right = glm::vec3(1.0f, 0.0f, 0.0f);
            //translating = glm::vec3(0.f, 0.f, 0.f);
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
            m_front = rot * glm::vec4(m_front, 1.0f);
            m_right = rot * glm::vec4(m_right, 1.0f);
            m_pos = new_pos;
            _updateCameraVectors();
            //_lookCenterX(center, new_pos);
        }
        void rotateByScreenY(glm::vec3& center, float angleY) {
            glm::mat4 trans(1.0f);
            glm::mat4 rot(1.0f);
            glm::mat4 inv_trans(1.0f);
            trans = glm::translate(trans, -center);
            rot = glm::rotate(rot, angleY, m_right);
            inv_trans = glm::translate(inv_trans, center);
            glm::vec3 new_pos = inv_trans * rot * trans * glm::vec4(m_pos, 1.0f);
            m_front = rot * glm::vec4(m_front, 1.0f);
            m_pos = new_pos;
            _updateCameraVectors();
            /*std::cout << "m_pos:" << m_pos.x << " " << m_pos.y << " " << m_pos.z << "\n";
            std::cout << "m_front:" << m_front.x << " " << m_front.y << " " << m_front.z << "\n";
            std::cout << "m_up:" << m_up.x << " " << m_up.y << " " << m_up.z << "\n";
            std::cout << "m_right:" << m_right.x << " " << m_right.y << " " << m_right.z << "\n";*/

        }

        // based on IfcModel->translate(glm::vec3& step)
        void translateByHoverDiv(glm::vec3& step) {
            //translating -= step;
            m_pos -= step;
            //std::cout << "m_pos:" << m_pos.x << " " << m_pos.y << " " << m_pos.z << "\n";
        }

        void RotateToCubeDirection(int num) {
            m_pos = m_precomputing_pos[num];
            m_front = m_precomputing_front[num];
            m_up = m_precomputing_up[num];
            m_right = m_precomputing_right[num];
        }

        void PrecomputingCubeDireciton(glm::vec3 view_pos) {

            //view_pos *= 1.5f;

            m_precomputing_pos.clear();
            m_precomputing_front.clear();
            m_precomputing_up.clear();
            m_precomputing_right.clear();

            m_precomputing_pos.push_back(glm::vec3(-view_pos.x, view_pos.y, -view_pos.z));//back
            m_precomputing_pos.push_back(glm::vec3(-view_pos.z, view_pos.y, view_pos.x));//left
            m_precomputing_pos.push_back(view_pos);//front
            m_precomputing_pos.push_back(glm::vec3(view_pos.z, view_pos.y, -view_pos.x));//right
            m_precomputing_pos.push_back(glm::vec3(-view_pos.x, view_pos.z, -view_pos.y));//up
            m_precomputing_pos.push_back(glm::vec3(view_pos.x, -view_pos.z, view_pos.y));//bottom

            glm::mat4 tempm(1.0f), orim(1.0f);
            float my_pi = 3.141592653f;

            tempm = glm::rotate(orim, 135.f * my_pi / 180.f, glm::vec3(-1, 0, 0));
            glm::vec4 view_pos_4 = glm::vec4(view_pos.x, view_pos.y, view_pos.z, 1.f);
            glm::vec4 front_4 = glm::vec4(0.f, 0.f, -1.f, 1.f);
            glm::vec4 up_4 = glm::vec4(0.f, 1.f, 0.f, 1.f);
            glm::vec4 right_4 = glm::vec4(1.f, 0.f, 0.f, 1.f);
            m_precomputing_pos.push_back(glm::rotate(orim, 135.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * view_pos_4);//back up
            m_precomputing_pos.push_back(glm::rotate(orim, 225.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * view_pos_4);//back right
            m_precomputing_pos.push_back(glm::rotate(orim, 225.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * view_pos_4);//back down
            m_precomputing_pos.push_back(glm::rotate(orim, 135.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * view_pos_4);//back left
            m_precomputing_pos.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(-1, -1, 0)) * view_pos_4);//left up
            m_precomputing_pos.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(1, -1, 0)) * view_pos_4);//left down
            m_precomputing_pos.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(-1, 1, 0)) * view_pos_4);//right up
            m_precomputing_pos.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(1, 1, 0)) * view_pos_4);//right down
            m_precomputing_pos.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, -1, 0)) * view_pos_4);//left front
            m_precomputing_pos.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * view_pos_4);//right front
            m_precomputing_pos.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * view_pos_4);//up front
            m_precomputing_pos.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(1, 0, 0)) * view_pos_4);//down front
            float this_angle2 = std::atanf(std::sqrtf(2.f));
            float this_angle = 1.f / this_angle2;
            m_precomputing_pos.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(-1, -1, 0)) * view_pos_4);//back left up
            m_precomputing_pos.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(-1, 1, 0)) * view_pos_4);//back right up
            m_precomputing_pos.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(1, -1, 0)) * view_pos_4);//back left down
            m_precomputing_pos.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(1, 1, 0)) * view_pos_4);//back right down
            m_precomputing_pos.push_back(glm::rotate(orim, this_angle2, glm::vec3(-1, -1, 0)) * view_pos_4);//front left up
            m_precomputing_pos.push_back(glm::rotate(orim, this_angle2, glm::vec3(-1, 1, 0)) * view_pos_4);//front right up
            m_precomputing_pos.push_back(glm::rotate(orim, this_angle2, glm::vec3(1, -1, 0)) * view_pos_4);//front left down
            m_precomputing_pos.push_back(glm::rotate(orim, this_angle2, glm::vec3(1, 1, 0)) * view_pos_4);//front right down

            m_precomputing_front.push_back(glm::vec3(0.f, 0.f, 1.f));//back
            m_precomputing_front.push_back(glm::vec3(1.f, 0.f, 0.f));//left
            m_precomputing_front.push_back(glm::vec3(0.f, 0.f, -1.f));//front
            m_precomputing_front.push_back(glm::vec3(-1.f, 0.f, 0.f));//right
            m_precomputing_front.push_back(glm::vec3(0.f, -1.f, 0.f));//up
            m_precomputing_front.push_back(glm::vec3(0.f, 1.f, 0.f));//bottom

            m_precomputing_front.push_back(glm::rotate(orim, 135.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * front_4);//back up
            m_precomputing_front.push_back(glm::rotate(orim, 225.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * front_4);//back right
            m_precomputing_front.push_back(glm::rotate(orim, 225.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * front_4);//back down
            m_precomputing_front.push_back(glm::rotate(orim, 135.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * front_4);//back left
            m_precomputing_front.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(-1, -1, 0)) * front_4);//left up
            m_precomputing_front.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(1, -1, 0)) * front_4);//left down
            m_precomputing_front.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(-1, 1, 0)) * front_4);//right up
            m_precomputing_front.push_back(glm::rotate(orim, 90.f * my_pi / 180.f, glm::vec3(1, 1, 0)) * front_4);//right down
            m_precomputing_front.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, -1, 0)) * front_4);//left front
            m_precomputing_front.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 1, 0)) * front_4);//right front
            m_precomputing_front.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * front_4);//up front
            m_precomputing_front.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(1, 0, 0)) * front_4);//down front

            m_precomputing_front.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(-1, -1, 0)) * front_4);//back left up
            m_precomputing_front.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(-1, 1, 0)) * front_4);//back right up
            m_precomputing_front.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(1, -1, 0)) * front_4);//back left down
            m_precomputing_front.push_back(glm::rotate(orim, my_pi - this_angle2, glm::vec3(1, 1, 0)) * front_4);//back right down
            m_precomputing_front.push_back(glm::rotate(orim, this_angle2, glm::vec3(-1, -1, 0)) * front_4);//front left up
            m_precomputing_front.push_back(glm::rotate(orim, this_angle2, glm::vec3(-1, 1, 0)) * front_4);//front right up
            m_precomputing_front.push_back(glm::rotate(orim, this_angle2, glm::vec3(1, -1, 0)) * front_4);//front left down
            m_precomputing_front.push_back(glm::rotate(orim, this_angle2, glm::vec3(1, 1, 0)) * front_4);//front right down

            m_precomputing_up.push_back(glm::vec3(0.f, 1.f, 0.f));//back
            m_precomputing_up.push_back(glm::vec3(0.f, 1.f, 0.f));//left
            m_precomputing_up.push_back(glm::vec3(0.f, 1.f, 0.f));//front
            m_precomputing_up.push_back(glm::vec3(0.f, 1.f, 0.f));//right
            m_precomputing_up.push_back(glm::vec3(0.f, 0.f, -1.f));//up
            m_precomputing_up.push_back(glm::vec3(0.f, 0.f, 1.f));//bottom

            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(1, 0, 0)) * up_4);//back up
            m_precomputing_up.push_back(up_4);//back left
            m_precomputing_up.push_back(glm::rotate(orim, 45 * my_pi / 180.f, glm::vec3(-1, 0, 0)) * up_4);//back down
            m_precomputing_up.push_back(up_4);//back right
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 0, -1)) * up_4);//left up
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 0, 1)) * up_4);//left down
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 0, 1)) * up_4);//right up
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(0, 0, -1)) * up_4);//right down
            m_precomputing_up.push_back(up_4);//left front
            m_precomputing_up.push_back(up_4);//right front
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(-1, 0, 0)) * up_4);//up front
            m_precomputing_up.push_back(glm::rotate(orim, 45.f * my_pi / 180.f, glm::vec3(1, 0, 0)) * up_4);//down front

            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(1, 0, -1)) * up_4);//back left up
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(1, 0, 1)) * up_4);//back right up
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(-1, 0, 1)) * up_4);//back left down
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(-1, 0, -1)) * up_4);//back right down
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(-1, 0, -1)) * up_4);//front left up
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(-1, 0, 1)) * up_4);//front right up
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(1, 0, 1)) * up_4);//front left down
            m_precomputing_up.push_back(glm::rotate(orim, this_angle, glm::vec3(1, 0, -1)) * up_4);//front right down
            /*for (int i = 0; i < 8; i++) {
                m_precomputing_up.push_back(up_4);
            }*/

            m_precomputing_right.push_back(glm::vec3(-1.f, 0.f, 0.f));//back
            m_precomputing_right.push_back(glm::vec3(0.f, 0.f, 1.f));//left
            m_precomputing_right.push_back(glm::vec3(1.f, 0.f, 0.f));//front
            m_precomputing_right.push_back(glm::vec3(0.f, 0.f, -1.f));//up
            m_precomputing_right.push_back(glm::vec3(1.f, 0.f, 0.f));//right
            m_precomputing_right.push_back(glm::vec3(1.f, 0.f, 0.f));//bottom
            for (int i = 6; i < 26; ++i) {
                m_precomputing_right.push_back(glm::normalize(glm::cross(m_precomputing_front[i], m_precomputing_up[i])));
            }

        }

        void translateByScreenOp(float offx, float offy, float offz) {
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

        const glm::vec3 getViewPos() const {
            return m_pos;
        }

        /*glm::vec3 getViewForward() {
            return m_front;
        }*/

        void set_pos(glm::vec3 pos) {
            m_pos = pos;
        }

        glm::vec3 getViewForward() {
            return glm::normalize(m_front);
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
            // m_right = glm::normalize(glm::cross(m_front, m_worldup)); 
            m_up = glm::normalize(glm::cross(m_right, m_front));
        }

        // Rodrigues formula --perhaps it doesnt need
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
            // m_front = R * next_to_center;
            m_pos = new_pos;
            _updateCameraVectors();
        }

        void do_RotateInLocalCoords(glm::vec3 local_axis, float rad) {
            glm::vec3 world_x = -glm::cross(m_up, m_front),
                world_z = -m_front,
                world_y = m_up;
            glm::mat3 basis(world_x, world_y, world_z);

            /*glm::mat3 inv_basis = glm::transpose(glm::inverse(basis));
            local_axis = inv_basis * local_axis;*/

            glm::mat4 basts(1.0f);
            glm::mat3 rot = glm::mat3(glm::rotate(basts, -rad, local_axis));
            basis = basis * rot;

            m_front = -basis[2];
            m_up = basis[1];
        }

        void rotateAlongPoint(glm::vec3& p, glm::vec3 local_axis, float rad) {
            float dist = sqrt(glm::dot(p - m_pos, p - m_pos));
            do_RotateInLocalCoords(local_axis, rad);
            m_pos = p - (m_front * dist);
        }

        // ------------- camera attributes----------------
        public:
        glm::vec3 m_pos; // 相机的位置
        glm::vec3 m_front; // 相机看向物体的方向向量
        glm::vec3 m_up; // 相机坐标的上方
        private:
        // calculated by worldup 
        glm::vec3 m_right; // 相机坐标的右方

        glm::vec3 m_worldup; // 世界y正方向


        const glm::vec3 m_velocity = glm::vec3(0.05f, 0.05f, 0.2f);
        // ----- ----- ----- ----- ----- ----- -----  -----

        Vector<glm::vec3> m_precomputing_pos;
        Vector<glm::vec3> m_precomputing_front;
        Vector<glm::vec3> m_precomputing_up;
        Vector<glm::vec3> m_precomputing_right;
    };
}

#endif
