#pragma once
#ifndef IFCRE_GLSL_PROGRAM_H_
#define IFCRE_GLSL_PROGRAM_H_

#include "../../common/std_types.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace ifcre {
    class GLSLProgram {
    public:
        GLSLProgram(const char* vertexCode, const char* fragmentCode);
		void use() const;
		void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;
		void setVec3(const std::string& name, const glm::vec3& value) const;
		void setMat3(const std::string& name, const glm::mat3& mat) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;
        void bindUniformBlock(const std::string& name, int index) const;
    private:
        void checkCompileErrors(GLuint shader, String type);
    private:
        unsigned int id;

    };
}

#endif