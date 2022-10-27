#include "collider.h"


// INTERFACE

void Collider::bufferData(const vector<uint32_t>& _g_indices, const vector<float>& _g_vertices, const vector<vector<uint32_t>>& _c_indices) {
	vector<uint32_t>().swap(vert_indices);
	vector<float>().swap(verts);
	vector<vector<uint32_t>>().swap(search_m);
	idsC.clear();
	idsA.clear();
	idsB.clear();

	vert_indices = _g_indices;
	verts = _g_vertices;
	search_m = _c_indices;
}

void Collider::setTotalIds(const unordered_set<int>& totalIds) {
	idsC = totalIds;
	this->mComponmentSize = idsC.size();
}

void Collider::setRespetcIds(const unordered_set<int>& _idsA, const unordered_set<int>& _idsB) {
	idsA = _idsA;
	idsB = _idsB;
}

std::vector<Collider::indexPair> Collider::getCollisionPair() {
	clock_t fir, sec;
	TIME_(
		this->buildData();
	, fir, sec, "build : ");
#if defined(COLLIDER_USE_BVH)
	TIME_(
		this->broadPhaseProcess_BVH();
	, fir, sec, "broad phase : ");
#elif defined(COLLIDER_USE_SORT)	
	TIME_(
		this->broadPhaseProcess_SORT();
	, fir, sec, "broad phase : ");
#elif defined(COLLIDER_USE_SUPER_FILTER)	
	TIME_(
		this->broadPhaseProcess_FILTER();
	, fir, sec, "broad phase : ");
#endif
	TIME_(
		this->conditionFilter();
	, fir, sec, "condition : ");
	TIME_(
		this->narrowPhaseProcess();
	, fir, sec, "narrow phase : ");
	auto retVal = std::move(this->mIndexArr);
	return retVal;
}

void Collider::getCollisionPair(Collider::indexPair* const allocatedMemory, const uint64_t siz, uint64_t* const retSiz) {
	auto ret = Collider::getCollisionPair();
	*retSiz = std::min(siz, (uint64_t)ret.size());
	::memcpy(allocatedMemory, ret.data(), (uint64_t)sizeof(indexPair) * (*retSiz));
}

void Collider::buildData() {
	std::vector<Componment>(idsC.size()).swap(mBuildDat);
#pragma omp parallel for schedule(static, 8)
	int i = 0;
	for (const auto idc : idsC) {
		mBuildDat[i] = std::move(Componment(search_m[idc], verts));
		++i;
	}
	idsC.clear();
}

void Collider::broadPhaseProcess_BVH() {
	// build BVH
	Bvh mBvh;
#ifdef COLLIDER_USE_BVH_SWEEP_SAH
	auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(static_cast<const Componment*>(
		mBuildDat.data()
		), this->mComponmentSize);
	auto globalBox = bvh::compute_bounding_boxes_union(bboxes.get(), this->mComponmentSize);
	bvh::SweepSahBuilder<Bvh> bvhBuilder(mBvh);
	bvhBuilder.build(globalBox, bboxes.get(), centers.get(), this->mComponmentSize);
#endif
	// find Self-Intersection 
	this->processBVH(mBvh);
#if defined(COLLIDER_DEBUG) && defined(COLLIDER_PRINT_RUNTIME)
	std::cout << "after broad " << this->mIndexArr.size() << std::endl;
#endif
}

void Collider::conditionFilter() {
	decltype(this->mIndexArr) tempIndexArr;
	vector<indexPair>().swap(tempIndexArr);
	for (const auto idA : idsA) {
		for (const auto idB : idsB) {
			if (idA != idB) {
				tempIndexArr.emplace_back(idA, idB);
			}
		}
	}
	this->mIndexArr = std::move(tempIndexArr);
#if defined(COLLIDER_DEBUG) && defined(COLLIDER_PRINT_RUNTIME)
	std::cout << "after condition " << this->mIndexArr.size() << std::endl;
#endif
}

void Collider::narrowPhaseProcess() {
	decltype(this->mIndexArr) tempArr;
	tempArr.reserve(this->mIndexArr.size());
	std::unique_ptr<bool[]> legalDummys = std::make_unique<bool[]>(this->mIndexArr.size());
	auto siz = this->mIndexArr.size();
	auto procCnt = 8;
	auto chuckSiz = siz / std::size_t(procCnt);
	auto coreProgram = [this, mLegalDummys = legalDummys.get()](std::size_t rangeLhs, std::size_t rangeRhs) -> void {
		for (std::size_t p = rangeLhs; p < rangeRhs; ++p) {
			const auto [lhs, rhs] = this->mIndexArr[p];
			const auto& crefL = search_m[lhs];
			const auto& crefR = search_m[rhs];
			const auto& crefDat = verts;
			mLegalDummys[p] = false;
			for (std::size_t lPtr = 0; lPtr < crefL.size() && !mLegalDummys[p]; lPtr += 3) {
				for (std::size_t rPtr = 0; rPtr < crefR.size(); rPtr += 3) {
					if (this->fastTriangleIntersect(
						Triangle{
							Vector3{crefDat[3 * crefL[lPtr + 0] + 0],crefDat[3 * crefL[lPtr + 0] + 1], crefDat[3 * crefL[lPtr + 0] + 2]},
							Vector3{crefDat[3 * crefL[lPtr + 1] + 0],crefDat[3 * crefL[lPtr + 1] + 1], crefDat[3 * crefL[lPtr + 1] + 2]},
							Vector3{crefDat[3 * crefL[lPtr + 2] + 0],crefDat[3 * crefL[lPtr + 2] + 1], crefDat[3 * crefL[lPtr + 2] + 2]}
						},
						Triangle{
							Vector3{crefDat[3 * crefR[rPtr + 0] + 0],crefDat[3 * crefR[rPtr + 0] + 1], crefDat[3 * crefR[rPtr + 0] + 2]},
							Vector3{crefDat[3 * crefR[rPtr + 1] + 0],crefDat[3 * crefR[rPtr + 1] + 1], crefDat[3 * crefR[rPtr + 1] + 2]},
							Vector3{crefDat[3 * crefR[rPtr + 2] + 0],crefDat[3 * crefR[rPtr + 2] + 1], crefDat[3 * crefR[rPtr + 2] + 2]}
						}
					)) {
						mLegalDummys[p] = true;
						break;
					}
				}
			}
		}
	};
	std::list<std::thread> threadList;
	for (std::size_t begin = 0; begin < siz; begin += chuckSiz) {
		threadList.emplace_back(
			coreProgram,
			begin,
			std::min(begin + chuckSiz, siz)
		);
	}
	for (auto&& mThread : threadList)
		mThread.join();
	for (std::size_t p = 0; p < this->mIndexArr.size(); ++p)
		if (legalDummys[p])
			tempArr.emplace_back(this->mIndexArr[p]);

	this->mIndexArr = std::move(tempArr);
}


#if defined(COLLIDER_USE_BVH)
void Collider::processBVH(const Bvh& mBvh) {
	std::vector<indexPair>().swap(this->mIndexArr);
	// auto nodeCnt = mBvh.node_count;
	struct alignas(16) RecursiveInfo {
		int phaseDummy;
		int args[3];
		/*
		* Recursivly Process Self Intersection
		* Phase 0 , args: [NodeIndex]
		* Build Intersection Among subTrees
		* Phase 1 , args: [NodeIndex]
		* Process Pair
		* Phase 2 , args: [NodeIndex, NodeIndex, IsLeaf]
		*/
	};
	std::stack<RecursiveInfo> recurStack;
	recurStack.push(RecursiveInfo{ 0, 0 });
	while (!recurStack.empty()) {
		// while (!recurStack.empty()){
		// 	if (bvh.nodes[recurStack.top().args[0]].is_leaf())
		// 		recurStack.pop();
		// 	else break;
		// }
		auto recurInfo = recurStack.top();
		recurStack.pop();
		if (recurInfo.phaseDummy == 0) {
			const auto nodeIndex = recurInfo.args[0];
			const auto isLeaf = mBvh.nodes[nodeIndex].is_leaf();
			if (!isLeaf) {
				recurStack.push(RecursiveInfo{ 1, nodeIndex });
				auto mIndex = mBvh.nodes[nodeIndex].first_child_or_primitive;
				recurStack.push(RecursiveInfo{ 0, (int)mIndex });
				recurStack.push(RecursiveInfo{ 0, (int)Bvh::sibling(mIndex) });
			}
			else {
				recurStack.push(RecursiveInfo{ 2, nodeIndex, nodeIndex, 1 }); // directly jump to phase 2
			}
		}
		else if (recurInfo.phaseDummy == 1) {
			const auto nodeIndex = recurInfo.args[0];
			const auto isLeaf = recurInfo.args[1];
			auto mIndex = mBvh.nodes[nodeIndex].first_child_or_primitive;
			auto anotherIndex = Bvh::sibling(mIndex);
			auto fir = mBvh.nodes[mIndex].bounding_box_proxy().to_bounding_box();
			auto sec = mBvh.nodes[anotherIndex].bounding_box_proxy().to_bounding_box();
			if (this->isIntersect(fir, sec))
				recurStack.push(RecursiveInfo{ 2, (int)mIndex, (int)Bvh::sibling(mIndex), 0 });

		}
		else if (recurInfo.phaseDummy == 2) {
			if (recurInfo.args[2] != 0) {
				const auto primitiveCount = mBvh.nodes[recurInfo.args[0]].primitive_count;
				const auto primitiveBeginIndex = mBvh.nodes[recurInfo.args[1]].first_child_or_primitive;
				const auto* const primitivePtr = mBvh.primitive_indices.get();
				for (std::size_t p1 = 0; p1 < primitiveCount; ++p1) {
					for (std::size_t p2 = p1 + 1; p2 < primitiveCount; ++p2) {
						auto [mMin, mMax] = std::minmax({ primitivePtr[primitiveBeginIndex + p1], primitivePtr[primitiveBeginIndex + p2] });
						this->mIndexArr.emplace_back((IndexType)mMin, (IndexType)mMax);
					}
				}
			}
			else {
				int firCnt = 2, secCnt = 2;
				if (mBvh.nodes[recurInfo.args[0]].is_leaf())
					firCnt = 1;
				if (mBvh.nodes[recurInfo.args[1]].is_leaf())
					secCnt = 1;
				if (firCnt == 1 && secCnt == 1) {

					auto firBox = mBvh.nodes[recurInfo.args[0]].bounding_box_proxy().to_bounding_box();
					auto secBox = mBvh.nodes[recurInfo.args[1]].bounding_box_proxy().to_bounding_box();
					if (!this->isIntersect(firBox, secBox)) {
						continue;
					}

					const auto* const ptr = mBvh.primitive_indices.get();
					auto firPmCnt = mBvh.nodes[recurInfo.args[0]].primitive_count;
					auto firBeginIndex = mBvh.nodes[recurInfo.args[0]].first_child_or_primitive;
					auto secPmCnt = mBvh.nodes[recurInfo.args[1]].primitive_count;
					auto secBeginIndex = mBvh.nodes[recurInfo.args[1]].first_child_or_primitive;
					for (std::size_t p1 = 0; p1 < firPmCnt; ++p1) {
						for (std::size_t p2 = 0; p2 < secPmCnt; ++p2) {
							auto [mMin, mMax] = std::minmax({ ptr[firBeginIndex + p1], ptr[secBeginIndex + p2] });
							this->mIndexArr.emplace_back((IndexType)mMin, (IndexType)mMax);
						}
					}
				}
				else {
					int firSubList[2], secSubList[2];
					if (firCnt == 1)
						firSubList[0] = recurInfo.args[0];
					else {
						firSubList[0] = mBvh.nodes[recurInfo.args[0]].first_child_or_primitive;
						firSubList[1] = Bvh::sibling(firSubList[0]);
					}
					if (secCnt == 1)
						secSubList[0] = recurInfo.args[1];
					else {
						secSubList[0] = mBvh.nodes[recurInfo.args[1]].first_child_or_primitive;
						secSubList[1] = Bvh::sibling(secSubList[0]);
					}
					for (int p1 = 0; p1 < firCnt; ++p1) {
						for (int p2 = 0; p2 < secCnt; ++p2) {
							auto firBox = mBvh.nodes[firSubList[p1]].bounding_box_proxy().to_bounding_box();
							auto secBox = mBvh.nodes[secSubList[p2]].bounding_box_proxy().to_bounding_box();
							if (!this->isIntersect(firBox, secBox)) {
								continue;
							}
							recurStack.push(RecursiveInfo{ 2, firSubList[p1], secSubList[p2], 0 });
						}
					}
				}
			}
		}
	}
	std::sort(this->mIndexArr.begin(), this->mIndexArr.end(), [](const indexPair fir, const indexPair sec) {
		if (fir.Lf != sec.Lf) {
			return fir.Lf < sec.Lf;
		}
		else return fir.Ri < sec.Ri;
		});
}
#endif

bool Collider::isIntersect(const Box lhs, const Box rhs) {
	for (int i = 0; i < 3; ++i)
		if (lhs.min.values[i] >= rhs.max.values[i] || rhs.min.values[i] >= lhs.max.values[i])
			return false;
	return true;
}

bool Collider::fastTriangleIntersect(Triangle lhs, Triangle rhs) {
	return 1 == tri_tri_overlap_test_3d(lhs.p0.values, lhs.p1().values, lhs.p2().values, rhs.p0.values, rhs.p1().values, rhs.p2().values);
}

std::vector<Collider::IndexType> Collider::getIndexArr() {
	auto tmp = this->getCollisionPair();
	std::vector<Collider::IndexType> ret;
	ret.reserve(tmp.size() << 1);
	for (auto pair : tmp)  ret.emplace_back(pair.Lf), ret.emplace_back(pair.Ri);
	return ret;
}
