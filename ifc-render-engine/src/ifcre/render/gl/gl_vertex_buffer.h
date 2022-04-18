#pragma once
#ifndef IFCRE_VERTEX_BUFFER_H_
#define IFCRE_VERTEX_BUFFER_H_

#include "../../common/std_types.h"
#include "../../resource/model.h"

namespace ifcre {
	class GLVertexBuffer {
	public:
		GLVertexBuffer()
			:m_vaoid(-1)
			, m_vboid(-1)
			, m_eboid(-1)
			, m_size(0) {}
		void upload(Vector<Real>& vertices, Vector<uint32_t>& indices);
		void upload(Vector<Real>& vertices);
		void vertexAttribDesc(uint32_t index, int32_t size, int32_t stride, const void* pointer);
		void ssboAttribUpload(Vector<MaterialData>& mtlData);
		void draw();
		void drawNoTrans();
		void drawTrans();
		void drawByAddedEbo(uint32_t ebo_id);
		void uploadElementBufferOnly(Vector<uint32_t>& indices);
		void uploadNoTransElements(Vector<uint32_t>& indices);
		void uploadTransElements(Vector<uint32_t>& indices);

		bool useIndex();
		void destroy();

		~GLVertexBuffer() {
			destroy();
		}

	private:
		uint32_t m_size;
		uint32_t m_vaoid;
		uint32_t m_vboid;
		uint32_t m_eboid;
		uint32_t m_ssboid;
		uint32_t ebo_id_trip = 0;

		uint32_t m_eboid_for_no_trans;
		uint32_t no_trans_size;
		uint32_t m_eboid_for_trans;
		uint32_t trans_size;
		Vector<uint32_t> m_eboid_list;
		Vector<uint32_t> m_size_list;
	};
}

#endif