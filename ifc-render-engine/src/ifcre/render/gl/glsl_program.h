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
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;
    private:
        void checkCompileErrors(GLuint shader, String type);
    private:
        unsigned int id;
    };
}

#endif