#pragma once
#ifndef IFCRE_VERTEX_BUFFER_H_
#define IFCRE_VERTEX_BUFFER_H_

#include "../../common/std_types.h"
#include "../../resource/model.h"
#include "../../common/ifc_enum.h"

namespace ifcre {
	class GLVertexBuffer {
	public:
		GLVertexBuffer()
			:m_vaoid(-1)
			, m_vboid(-1)
			, m_eboid(-1)
			, m_size(0) {
			init_fun_ptr_map();
		}
		// make connection to draw calls & drawcall enums
		void init_fun_ptr_map();


		//draw calls
		void draw();
		void drawNoTrans();
		void drawTrans();
		void drawEdges();
		void drawByAddedEbo(uint32_t ebo_id);
		void drawBBXLines();
		void drawByDynamicEbo_no_trans();
		void drawByDynamicEbo_trans();
		void drawByDynamicEbo();
		void drawByDynamicEdge();
		void drawByChosen_no_trans();
		void drawByChosen_trans();
		void drawByCollision();

		//data update functions
		void upload(Vector<Real>& vertices, Vector<uint32_t>& indices);
		void upload(Vector<Real>& vertices);
		void uploadBBXData(const Vector<Real>& vertices, const Vector<uint32_t>& indices);
		void updateVertexAttributes(const Vector<Real>& vertices);
		void vertexAttribDesc(uint32_t index, int32_t size, int32_t stride, const void* pointer);
		[[deprecated("No use in this project.")]]
		void ssboAttribUpload(Vector<MaterialData>& mtlData);
		
		// sending ebos of all components' geometry to GPU
		void uploadElementBufferOnly(Vector<uint32_t>& indices);
		// sending ebos of all components' geometry to GPU
		// it will generate a vector<ebo> to let frawByAddedEbo() have the ability to draw the specified component
		void uploadElementBufferOnly(Vector<Vector<uint32_t>>& c_indices);
		//sending ebos of all edges of components to GPU
		void UploadElementEdge(Vector<uint32_t>& edge_indices);
		// sending ebos of all transparency components' geometry to GPU
		void uploadNoTransElements(Vector<uint32_t>& indices);
		// sending ebos of all no-transparency components' geometry to GPU
		void uploadTransElements(Vector<uint32_t>& indices);
		// sending ebos of filtered components' geometry to GPU
		// the first parameter is vector of all filtered components' geometry, no matter it's transparency or not
		// the second & third parameters are components' ebo divided by their transparency
		//void uploadDynamicElementBuffer(const Vector<uint32_t>& dynamic_all_ebo, const Vector<uint32_t>& dynamic_indices_no_trans, const Vector<uint32_t>& dynamic_indices_trans, const Vector<uint32_t>& dynamic_indices_edge);
		
		void upoadDynamicElementBuffer(const Vector<uint32_t>& dynamic_indices_no_trans, const Vector<uint32_t>& dynamic_indices_trans, const Vector<uint32_t>& dynamic_indices_edge);

		
		// sending ebos of chosen components' geometry to GPU
		// the input parameters are components' ebo divided by their transparency
		void uploadChosenElementBuffer(const Vector<uint32_t>& chosen_indices_no_trans, const Vector<uint32_t>& chosen_indices_trans);
		// sending ebos of collision components' geometry to GPU
		void uploadCollisionElementBuffer(const Vector<uint32_t>& collision_ebo);
		void run_draw_func(uint32_t draw_id);

		bool useIndex();
		void destroy();

		~GLVertexBuffer() {
			destroy();
		}

	private:
		typedef void (GLVertexBuffer::* Fun_ptr)();
		uint32_t m_size;
		uint32_t m_vaoid;
		uint32_t m_vboid;
		uint32_t m_eboid;
		uint32_t m_ssboid; // deprecated
		uint32_t ebo_id_trip = 0;

		bool no_trans_generated = false;
		uint32_t m_eboid_for_no_trans;
		uint32_t no_trans_size;

		bool trans_generated = false;
		uint32_t m_eboid_for_trans;
		uint32_t trans_size;

		bool edge_generated = false;
		uint32_t m_eboid_for_edge;
		uint32_t edge_size;

		bool list_generated = false;
		Vector<uint32_t> m_eboid_list;
		Vector<uint32_t> m_size_list;

		//dynamic ebo for isolation
		bool dyn_generated = false;
		uint32_t m_dynamic_eboid;
		uint32_t m_dynamic_eboid_for_no_trans;
		uint32_t m_dynamic_eboid_for_trans;
		uint32_t m_dynamic_eboid_for_edge;

		uint32_t dynamic_size;
		uint32_t no_tran_dynamic_size;
		uint32_t tran_dynamic_size;
		uint32_t edge_dynamic_size;

		//ebo for chosen
		uint32_t m_chosen_eboid_for_no_trans;
		uint32_t m_chosen_eboid_for_trans;

		uint32_t no_trans_chosen_size;
		uint32_t trans_chosen_size;

		//ebo for collision
		uint32_t m_collision_eboid;

		uint32_t collision_size;

	public:
		map<uint32_t, Fun_ptr> mFuncPtr;
	};
}

#endif