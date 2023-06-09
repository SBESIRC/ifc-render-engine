﻿#include "gl_vertex_buffer.h"
#include <glad/glad.h>
namespace ifcre {
	void GLVertexBuffer::init_fun_ptr_map() {
		mFuncPtr.insert({ uint32_t(RenderPartEnum::ALL), &GLVertexBuffer::draw });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::DYNAMIC_ALL),&GLVertexBuffer::drawByDynamicEbo });
		//mFuncPtr.insert({ uint32_t(RenderPartEnum::NO_TRANS),&GLVertexBuffer::drawNoTrans });
		//mFuncPtr.insert({ uint32_t(RenderPartEnum::TRANS),&GLVertexBuffer::drawTrans });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::BBX_LINE),&GLVertexBuffer::drawBBXLines });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::EDGE_LINE),&GLVertexBuffer::drawEdges });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::DYNAMIC_NO_TRANS),&GLVertexBuffer::drawByDynamicEbo_no_trans });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::DYNAMIC_TRANS),&GLVertexBuffer::drawByDynamicEbo_trans });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::DYNAMIC_EDGE_LINE),&GLVertexBuffer::drawByDynamicEdge });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::CHOSEN_NO_TRANS),&GLVertexBuffer::drawByChosen_no_trans });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::CHOSEN_TRANS),&GLVertexBuffer::drawByChosen_trans });
		mFuncPtr.insert({ uint32_t(RenderPartEnum::COLLISION),&GLVertexBuffer::drawByCollision });
	}

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

	void GLVertexBuffer::vertexAttribDesc(uint32_t index, int32_t size, int32_t stride, const void* pointer)
	{
		// GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer
		glBindVertexArray(m_vaoid);
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
		glEnableVertexAttribArray(index);
		glBindVertexArray(0);
	}

#pragma region draws
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
	void GLVertexBuffer::drawEdges() {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_edge);
		glDrawElements(GL_LINES, edge_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByAddedEbo(uint32_t ebo_id) {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
		glDrawElements(GL_TRIANGLES, m_size_list[ebo_id - ebo_id_trip], GL_UNSIGNED_INT, 0);
		//这里-ebo_id_trip是因为在genBuffer m_ebo_list前，已经调用了ebo_id_trip-1次genBuffer，导致m_ebo_list[0]=ebo_id_trip，所以要-ebo_id_trip
		//后续将改为某个参数
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawBBXLines() {
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
		glDrawElements(GL_LINE_STRIP, m_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	void GLVertexBuffer::drawByDynamicEbo_no_trans() {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_no_trans);
		glDrawElements(GL_TRIANGLES, no_tran_dynamic_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByDynamicEbo_trans() {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_trans);
		glDrawElements(GL_TRIANGLES, tran_dynamic_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByDynamicEbo() {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid);
		glDrawElements(GL_TRIANGLES, dynamic_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByDynamicEdge() {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_edge);
		glDrawElements(GL_LINES, edge_dynamic_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByChosen_no_trans()
	{
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chosen_eboid_for_no_trans);
		glDrawElements(GL_TRIANGLES, no_trans_chosen_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByChosen_trans()
	{
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chosen_eboid_for_trans);
		glDrawElements(GL_TRIANGLES, trans_chosen_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GLVertexBuffer::drawByCollision()
	{
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_collision_eboid);
		glDrawElements(GL_TRIANGLES, collision_size, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

#pragma endregion

	inline bool GLVertexBuffer::useIndex()
	{
		return m_eboid != -1;
	}

#pragma region updates

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
		Vector<uint32_t>(c_indices.size()).swap(m_eboid_list);
		Vector<uint32_t>(c_indices.size()).swap(m_size_list);
		glBindVertexArray(m_vaoid);
		glGenBuffers(c_indices.size(), m_eboid_list.data());
		ebo_id_trip = m_eboid_list.size() > 0 ? m_eboid_list[0] - 0 : 0;
		for (int i = 0; i < c_indices.size(); i++) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_list[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glNamedBufferData(m_eboid_list[i], sizeof(uint32_t) * c_indices[i].size(), c_indices[i].data(), GL_STATIC_DRAW);
			m_size_list[i] = c_indices[i].size();
		}
		glBindVertexArray(0);
	}
	void GLVertexBuffer::UploadElementEdge(Vector<uint32_t>& edge_indices) {
		edge_size = edge_indices.size();
		glBindVertexArray(m_vaoid);
		glGenBuffers(1, &m_eboid_for_edge);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid_for_edge);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glNamedBufferData(m_eboid_for_edge, edge_size * sizeof(uint32_t), edge_indices.data(), GL_STATIC_DRAW);
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

	void GLVertexBuffer::uploadDynamicElementBuffer(const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& dynamic_indices_no_trans, const Vector<uint32_t>& dynamic_indices_trans, const Vector<uint32_t>& dynamic_indices_edge) {
		//both, use for comp id
		glBindVertexArray(m_vaoid);
		glDeleteBuffers(1, &m_dynamic_eboid);
		glGenBuffers(1, &m_dynamic_eboid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid);
		glNamedBufferData(m_dynamic_eboid, dynamic_all_ebo.size() * sizeof(uint32_t), dynamic_all_ebo.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//no trans
		glDeleteBuffers(1, &m_dynamic_eboid_for_no_trans);
		glGenBuffers(1, &m_dynamic_eboid_for_no_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_no_trans);
		glNamedBufferData(m_dynamic_eboid_for_no_trans, dynamic_indices_no_trans.size() * sizeof(uint32_t), dynamic_indices_no_trans.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//trans
		glDeleteBuffers(1, &m_dynamic_eboid_for_trans);
		glGenBuffers(1, &m_dynamic_eboid_for_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_trans);
		glNamedBufferData(m_dynamic_eboid_for_trans, dynamic_indices_trans.size() * sizeof(uint32_t), dynamic_indices_trans.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//edges
		glDeleteBuffers(1, &m_dynamic_eboid_for_edge);
		glGenBuffers(1, &m_dynamic_eboid_for_edge);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_dynamic_eboid_for_edge);
		glNamedBufferData(m_dynamic_eboid_for_edge, dynamic_indices_edge.size() * sizeof(uint32_t), dynamic_indices_edge.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		dynamic_size = dynamic_all_ebo.size();
		no_tran_dynamic_size = dynamic_indices_no_trans.size();
		tran_dynamic_size = dynamic_indices_trans.size();
		edge_dynamic_size = dynamic_indices_edge.size();
	}

	void GLVertexBuffer::uploadChosenElementBuffer(const Vector<uint32_t>& chosen_indices_no_trans, const Vector<uint32_t>& chosen_indices_trans)
	{
		//no trans
		glBindVertexArray(m_vaoid);
		glDeleteBuffers(1, &m_chosen_eboid_for_no_trans);
		glGenBuffers(1, &m_chosen_eboid_for_no_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chosen_eboid_for_no_trans);
		glNamedBufferData(m_chosen_eboid_for_no_trans, chosen_indices_no_trans.size() * sizeof(uint32_t), chosen_indices_no_trans.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//trans
		glDeleteBuffers(1, &m_chosen_eboid_for_trans);
		glGenBuffers(1, &m_chosen_eboid_for_trans);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_chosen_eboid_for_trans);
		glNamedBufferData(m_chosen_eboid_for_trans, chosen_indices_trans.size() * sizeof(uint32_t), chosen_indices_trans.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		no_trans_chosen_size = chosen_indices_no_trans.size();
		trans_chosen_size = chosen_indices_trans.size();
	}

	void GLVertexBuffer::uploadCollisionElementBuffer(const Vector<uint32_t>& collision_ebo) {
		glBindVertexArray(m_vaoid);
		glDeleteBuffers(1, &m_collision_eboid);
		glGenBuffers(1, &m_collision_eboid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_collision_eboid);
		glNamedBufferData(m_collision_eboid, collision_ebo.size() * sizeof(uint32_t), collision_ebo.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		collision_size = collision_ebo.size();

	}

	void GLVertexBuffer::run_draw_func(RenderPartEnum draw_id) {
		if (mFuncPtr.count(uint32_t(draw_id))) {
			(this->*mFuncPtr[uint32_t(draw_id)])();
		}
	}

	void GLVertexBuffer::uploadBBXData(const Vector<Real>& vertices, const  Vector<uint32_t>& indices) {
		glGenVertexArrays(1, &m_vaoid);
		glGenBuffers(1, &m_vboid);
		glGenBuffers(1, &m_eboid);
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Real), vertices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboid);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
		m_size = indices.size();
	}

	void GLVertexBuffer::updateVertexAttributes(const Vector<Real>& vertices) {
		glBindVertexArray(m_vaoid);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Real), vertices.data(), GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}

#pragma endregion

	void GLVertexBuffer::destroy()
	{
		glDeleteVertexArrays(1, &m_vaoid);
		glDeleteBuffers(1, &m_vboid);

		if (m_eboid != -1) {
			glDeleteBuffers(1, &m_eboid);
		}

		glDeleteBuffers(m_eboid_list.size(), m_eboid_list.data());

		glDeleteBuffers(1, &m_eboid_for_no_trans);
		glDeleteBuffers(1, &m_dynamic_eboid_for_no_trans);
		glDeleteBuffers(1, &m_chosen_eboid_for_no_trans);

		glDeleteBuffers(1, &m_eboid_for_trans);
		glDeleteBuffers(1, &m_eboid_for_edge);

		glDeleteBuffers(1, &m_dynamic_eboid);
		glDeleteBuffers(1, &m_dynamic_eboid_for_trans);
		glDeleteBuffers(1, &m_dynamic_eboid_for_edge);

		glDeleteBuffers(1, &m_chosen_eboid_for_trans);

		glDeleteBuffers(1, &m_collision_eboid);
		mFuncPtr.clear();
		std::cout << "vertex buffer clear.\n";
	}
}
