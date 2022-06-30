#pragma once
#ifndef IFCRE_UNIFORM_BUFFER_H_
#define IFCRE_UNIFORM_BUFFER_H_

#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../common/std_types.h"
namespace ifcre {
	class GLUniformBuffer;

	struct GLUniformBufferMap {
		SharedPtr<GLUniformBuffer> transformsUBO;		// index 0
		SharedPtr<GLUniformBuffer> ifcRenderUBO;		// index 1
		SharedPtr<GLUniformBuffer> transformMVPUBO;		// index 2
	};

	class GLUniformBuffer {
	private:
		uint32_t m_uniform_id;
		size_t m_buffer_size;
	public:
		GLUniformBuffer(size_t buffer_size): m_buffer_size(buffer_size) {
			glGenBuffers(1, &m_uniform_id);										//创建一个Uniform缓冲对象id
			glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_id);						//将id绑定到GL_UNIFORM_BUFFER目标
			glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);	//分配内存
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void bindRange(int index, int offset = 0) { // 将 m_uniform_id 链接到 index 绑定点
			// glBindBufferBase(GL_UNIFORM_BUFFER, index, m_uniform_id); 
			glBindBufferRange(GL_UNIFORM_BUFFER, index, m_uniform_id, offset, m_buffer_size);
		}

		void update(size_t offset, size_t byte_size, const void* data) {
			glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_id);
			//更新ubo内存 // 填充缓冲
			glBufferSubData(GL_UNIFORM_BUFFER, offset, byte_size, data);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		uint32_t id() { return m_uniform_id; }
	};
}

#endif