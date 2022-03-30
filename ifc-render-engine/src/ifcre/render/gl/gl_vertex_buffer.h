#pragma once
#ifndef IFCRE_VERTEX_BUFFER_H_
#define IFCRE_VERTEX_BUFFER_H_

#include "../../common/std_types.h"

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

		void draw();

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
	};
}

#endif