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
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
		glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawNoTrans()
	{
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_no_trans);
		glDrawElements(GL_TRIANGLES, no_trans_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	void GLVertexBuffer::drawTrans()
	{
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_trans);
		glDrawElements(GL_TRIANGLES, trans_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByAddedEbo(uint32_t ebo_id) {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
		glBindVertexArray(m_vaoid);
		glDrawElements(GL_TRIANGLES, m_size_list[ebo_id - ebo_id_trip], GL_UNSIGNED_INT, 0);
		//这里-ebo_id_trip是因为在genBuffer m_ebo_list前，已经调用了ebo_id_trip-1次genBuffer，导致m_ebo_list[0]=ebo_id_trip，所以要-ebo_id_trip
		//后续将改为某个参数
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
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
	void GLVertexBuffer::uploadElementBufferOnly(Vector<Vector<uint32_t>>& c_indices) {
		m_eboid_list.resize(c_indices.size());
		m_size_list.resize(c_indices.size());
		glBindVertexArray(m_vaoid);
		glGenBuffers(c_indices.size(), m_eboid_list.data());
		ebo_id_trip = m_eboid_list[0] - 0;
		for (int i = 0; i < c_indices.size(); i++) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_list[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glNamedBufferData(m_eboid_list[i], sizeof(uint32_t) * c_indices[i].size(), c_indices[i].data(), GL_STATIC_DRAW);
			m_size_list[i]=c_indices[i].size();
		}
		glBindVertexArray(0);
	}

	void GLVertexBuffer::uploadNoTransElements(Vector<uint32_t>& indices)
	{
		glBindVertexArray(m_vaoid);
		glGenBuffers(1, &m_eboid_for_no_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_no_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glNamedBufferData(m_eboid_for_no_trans, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		no_trans_size = indices.size();
		glBindVertexArray(0);
	}

	void GLVertexBuffer::uploadTransElements(Vector<uint32_t>& indices)
	{
		glBindVertexArray(m_vaoid);
		glGenBuffers(1, &m_eboid_for_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glNamedBufferData(m_eboid_for_trans, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		trans_size = indices.size();
		glBindVertexArray(0);
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