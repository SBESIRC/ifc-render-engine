#pragma once
#include <vector>
#include <iostream>
#include <omp.h>
#include <stack>
#include <queue>
#include <type_traits>
#include <cstdint>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <unordered_map>
#include <map>
#ifdef _MSC_VER
#include <thread>
#include <list>
#endif


// MACROS
// #define COLLIDER_DEBUG
#define COLLIDER_PRINT_RUNTIME
// if ndef, class will use container with higher efficiency.
// but the cost is that it will be unfriendly on debug window
#define COLLIDER_USE_DBG_CONTAINER
// #define COLLIDER_USE_DBG_HEADER
/*********************************/
// use broad algorithm based on bvh
#define COLLIDER_USE_BVH 
// sbvh
// #define COLLIDER_USE_NVIDIA_SBVH
// default
#define COLLIDER_USE_BVH_SWEEP_SAH
// linear bvh
// #define COLLIDER_USE_LBVH
// morton
// #define COLLIDER_USE_MORTON_BVH
/*********************************/
// use sort algorithm
//#define COLLIDER_USE_SORT
/*********************************/
// use fast filter
//#define COLLIDER_USE_SUPER_FILTER
/*********************************/
// use 64bit floating point
// #define COLLIDER_USE_FLOAT64

#ifdef COLLIDER_USE_DBG_HEADER
#include "data.h"
#else
#include "ifc2OpenGLDatas.h"
#endif

#include "tri_tri_intersect.h"

#include "bvh/bvh.hpp"
#include "bvh/vector.hpp"
#include "bvh/triangle.hpp"
#include "bvh/sweep_sah_builder.hpp"
#include <unordered_set>
// #include "bvh/binned_sah_builder.hpp"

#if defined(COLLIDER_DEBUG) && defined(COLLIDER_PRINT_RUNTIME)
#define TIME_(func, fir, sec, message)\
fir = ::time(NULL);\
func;\
sec = ::time(NULL);\
std::cout << message << ::difftime(sec, fir) << std::endl
#else
#define TIME_(func, fir, sec, message) func
#endif 



template<class _IndexType, class _ScalarType>
class ComponmentWrapper {
public:

	using ScalarType = _ScalarType;
	ComponmentWrapper() = default;
	ComponmentWrapper(const std::vector<_IndexType>& _idx, const std::vector<ScalarType>& _dat) {
		const auto indexPtr = _idx.data();
		const auto indexSiz = _idx.size();
		// this->pointCnt = indexSiz;
		if (0 != indexSiz) {
			bvh::BoundingBox<ScalarType> tempBox(bvh::Vector3<ScalarType>(_dat[indexPtr[0] * 3], _dat[indexPtr[0] * 3 + 1], _dat[indexPtr[0] * 3 + 2]));
			for (std::size_t p = 1; p < indexSiz; ++p) {
				const auto pos = indexPtr[p] * std::size_t(3);
				tempBox.extend(bvh::Vector3<ScalarType>(_dat[pos], _dat[pos + 1], _dat[pos + 2]));
				// ::memcpy(dat.get() + 3 * p, &_dat[indexPtr[p]], 3 * sizeof(ScalarType));
			}
			this->mBox = tempBox;
		}
	}

	bvh_always_inline bvh::BoundingBox<ScalarType> bounding_box() const {
		return this->mBox;
	}

	bvh_always_inline bvh::Vector3<ScalarType> center() const {
		return this->bounding_box().center();
	}

private:
	bvh::BoundingBox<ScalarType> mBox;
};

class Collider {

public:
#ifndef COLLIDER_USE_FLOAT64
	using Scalar = float;
#else
	using Scalar = double;
#endif
	using IndexType = unsigned;
private:
	using Vector3 = bvh::Vector3<Scalar>;
	using Triangle = bvh::Triangle<Scalar>;
	using Bvh = bvh::Bvh<Scalar>;
	using Box = bvh::BoundingBox<Scalar>;
	using Componment = ComponmentWrapper<IndexType, Scalar>;
public:
	struct indexPair {
		IndexType Lf, Ri;
		indexPair() = default;
		indexPair(IndexType a, IndexType b) {
			Lf = a; Ri = b;
		}
		indexPair(const indexPair&) = default;
	};

public:
	void bufferData(const vector<uint32_t>& _g_indices, const vector<float>& _g_vertices, const vector<vector<uint32_t>>& _c_indices);
	void setTotalIds(const unordered_set<int>& totalIds);
	void setRespetcIds(const unordered_set<int>& idsA, const unordered_set<int>& idsB);
	void getCollisionPair(indexPair* const allocatedMemory, const uint64_t siz, uint64_t* const retSiz);
	std::vector<indexPair> getCollisionPair();
	std::vector<IndexType> getIndexArr();
protected:
	//void filterData();
	void buildData();
#if defined(COLLIDER_USE_BVH) && !defined(COLLIDER_USE_SORT) && !defined(COLLIDER_USE_SUPER_FILTER)
	void broadPhaseProcess_BVH();
#elif defined(COLLIDER_USE_SORT) && !defined(COLLIDER_USE_SUPER_FILTER)
	void broadPhaseProcess_SORT();
#elif defined(COLLIDER_USE_SUPER_FILTER)
	void broadPhaseProcess_FILTER();
#endif
	void conditionFilter();
	void narrowPhaseProcess();
private:
#if defined(COLLIDER_USE_BVH)
	void processBVH(const Bvh& bvh);
#endif
	bool isIntersect(const Box lhs, const Box rhs);
	bool fastTriangleIntersect(Triangle lhs, Triangle rhs);
private:
	std::size_t mComponmentSize;
	std::vector<Componment> mBuildDat; // 存bvh适应data
	std::unordered_set<int> idsC; // 要进行碰撞的ids
	std::unordered_set<int> idsA; // 要进行碰撞的ids
	std::unordered_set<int> idsB; // 要进行碰撞的ids
	std::vector<indexPair> mIndexArr;

	vector<uint32_t> vert_indices;//vert_indices// 顶点的索引，数量为面个数的三倍，每3个顶点一个面
	vector<float> verts;//verts// 依次存储各个顶点位置的x、y、z信息，数量为顶点数量的三倍
	vector<vector<uint32_t>> search_m; //search_m// 物件->顶点的索引，1级数量为物件的个数，2级为物件拥有顶点数
}
;
