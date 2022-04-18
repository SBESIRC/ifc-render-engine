#include "gl_vertex_buffer.h"
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

	void GLVertexBuffer::ssboAttribUpload(Vector<MaterialData>& mtlData) {
		/*GLint SSBOBinding = 0, BlockDataSize = 0;*/
		glGenBuffers(1,&m_ssboid);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboid);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialData) * mtlData.size(), mtlData.data(), GL_DYNAMIC_COPY);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void GLVertexBuffer::draw()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawNoTrans()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_no_trans);
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, no_trans_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
	}


	void GLVertexBuffer::drawTrans()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_trans);
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, trans_size, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
	}

	void GLVertexBuffer::drawByAddedEbo(uint32_t ebo_id) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, m_size_list[ebo_id - 5], GL_UNSIGNED_INT, 0);
		//这里-5是因为在genBuffer m_ebo_list前，已经调用了4次genBuffer，导致m_ebo_list[0]=5，所以要-5
		//后续将改为某个参数
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
	}

	inline bool GLVertexBuffer::useIndex()
	{
		return m_eboid != -1;
	}

	void GLVertexBuffer::uploadElementBufferOnly(Vector<uint32_t>& indices)
	{
		uint32_t new_eboid;
		glGenBuffers(1, &new_eboid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_eboid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		m_eboid_list.emplace_back(new_eboid);
		m_size_list.emplace_back(indices.size());
	}

	void GLVertexBuffer::uploadNoTransElements(Vector<uint32_t>& indices)
	{
		glGenBuffers(1, &m_eboid_for_no_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_no_trans);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		no_trans_size = indices.size();
	}

	void GLVertexBuffer::uploadTransElements(Vector<uint32_t>& indices)
	{
		glGenBuffers(1, &m_eboid_for_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_trans);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		trans_size = indices.size();
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