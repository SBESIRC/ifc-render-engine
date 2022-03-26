#include "vertex_buffer.h"
#include <glad/glad.h>
namespace ifcre {

	void GLVertexBuffer::upload(Vector<Real>& vertices, Vector<uint32_t>& indices)
	{
		glGenVertexArrays(1, &m_vaoid);
		glGenBuffers(1, &m_vboid);
		glGenBuffers(1, &m_eboid);
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Real), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
		m_size = indices.size();
	}

	void GLVertexBuffer::upload(Vector<Real>& vertices)
	{
		glGenVertexArrays(1, &m_vaoid);
		glGenBuffers(1, &m_vboid);
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Real), vertices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::vertexAttribDesc(uint32_t index, int32_t size, int32_t stride, const void* pointer)
	{
		// GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
		glBindVertexArray(m_vaoid);
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
		glEnableVertexAttribArray(index);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::draw()
	{
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	inline bool GLVertexBuffer::useIndex()
	{
		return m_eboid != -1;
	}

	void GLVertexBuffer::destroy()
	{
		glDeleteVertexArrays(1, &m_vaoid);
		glDeleteBuffers(1, &m_vboid);

		if (m_eboid != -1) {
			glDeleteBuffers(1, &m_eboid);
		}
	}
}