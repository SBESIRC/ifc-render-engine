#include "collider.h"

// INTERFACE

void Collider::bufferData(const rawData * dataSet){
	this->mRawData = dataSet;
}

void Collider::addFilter(Filter filter) {
	this->mFilter = filter;
}

void Collider::addCondition(Condition condition) {
	this->mCondition = condition;
}

std::vector<Collider::indexPair> Collider::getCollisionPair() {
	clock_t fir, sec;
	TIME_(
	this->filterData();
	,fir, sec, "filter : ");
	TIME_(
	this->buildData();
	,fir, sec, "build : ");
#if defined(COLLIDER_USE_BVH)
	TIME_(
	this->broadPhaseProcess_BVH();
	,fir, sec, "broad phase : ");
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
	,fir, sec, "condition : ");
	TIME_(
	this->narrowPhaseProcess();
	,fir, sec, "narrow phase : ");
	auto retVal = std::move(this->mIndexArr);
	return retVal;	
}

void Collider::getCollisionPair(Collider::indexPair* const allocatedMemory, const uint64_t siz, uint64_t* const retSiz) {
	auto ret = Collider::getCollisionPair(); 
	*retSiz = std::min(siz, (uint64_t)ret.size());
	::memcpy(allocatedMemory, ret.data(), (uint64_t)sizeof(indexPair) * (*retSiz));
}

// PIPELINE

void Collider::filterData() {
	const auto sz = this->mRawData->componentDatas.size();
	for (std::size_t i = 0; i < sz; ++i) 
		if (this->mFilter(this->mRawData->componentDatas[i]))
			mFilterIndex.emplace_back(i);
	this->mComponmentSize = mFilterIndex.size();
}

void Collider::buildData() {
	mBuildDat.resize(mFilterIndex.size());
	#pragma omp parallel for schedule(static, 8)
	for (std::size_t i = 0; i < mFilterIndex.size(); ++i) 
		mBuildDat[i] = std::move(Componment(mRawData->search_m[mFilterIndex[i]], mRawData->verts));
	vector<unsigned>().swap(mFilterIndex);
	/*mFilterIndex.clear();
	mFilterIndex.shrink_to_fit();*/
}

#if defined(COLLIDER_USE_BVH)
void Collider::broadPhaseProcess_BVH() {
	// build BVH
	Bvh mBvh;
#ifdef COLLIDER_USE_BVH_SWEEP_SAH
	auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(static_cast<const Componment *>(
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
#endif

#if defined(COLLIDER_USE_SORT)
void Collider::broadPhaseProcess_SORT() {
	/*
	* first build boxes, then sort them in x, y, z axis.
	* second step build [indexProxy]s(index[0] < and always < index[1]), then put them into ::std::unordered_map<unsigned long long, unsigned>
	* last, iterator the set, find those who val equals to **three**(intersect in 3 - axis)
	*/

	// struct build 
	struct indexProxy {
		static_assert(sizeof(unsigned) * 2 == sizeof(unsigned long long));
		union {
			// index[0] < index[1]
			unsigned index[2];
			unsigned long long hashVal;
		};
		indexProxy() = default;
		indexProxy(unsigned long long t) : hashVal(t) {}
		indexProxy(unsigned t, unsigned tt) {
			index[0] = t;
			index[1] = tt;
		}
		bvh_always_inline unsigned getCode() const {
			return (index[0] ^ index[1]);
		}
	};

	struct rangeProxy {
		unsigned componentIndex;
		unsigned isEnd;
		Collider::Scalar rangeT;
		rangeProxy() = default;
		rangeProxy(unsigned one, unsigned two, Collider::Scalar three) {
			componentIndex = one;
			isEnd = two;
			rangeT = three;
		}
	};

	std::vector<rangeProxy> xRanges, yRanges, zRanges;
	xRanges.reserve(2 * this->mBuildDat.size());
	yRanges.reserve(2 * this->mBuildDat.size());
	zRanges.reserve(2 * this->mBuildDat.size());

	std::size_t curIndex = 0;

	// build ranges
	for (const auto& dat : this->mBuildDat) {
		const auto min_ = dat.bounding_box().min;
		const auto max_ = dat.bounding_box().max;
		
		/* ComponentIndex, isEnd, range */
		xRanges.emplace_back(curIndex, 0, min_[0]);
		xRanges.emplace_back(curIndex, 1, max_[0]);
		yRanges.emplace_back(curIndex, 0, min_[1]);
		yRanges.emplace_back(curIndex, 1, max_[1]);
		zRanges.emplace_back(curIndex, 0, min_[2]);
		zRanges.emplace_back(curIndex, 1, max_[2]);

		++curIndex;
	}

	const auto sortRule = [](rangeProxy fir, rangeProxy sec) -> bool {
		return fir.rangeT < sec.rangeT;
	};
#pragma omp task shared(xRanges)
	std::sort(xRanges.begin(), xRanges.end(), sortRule);
#pragma omp task shared(yRanges)
	std::sort(yRanges.begin(), yRanges.end(), sortRule);
#pragma omp task shared(zRanges)
	std::sort(zRanges.begin(), zRanges.end(), sortRule);
#pragma omp taskwait

	std::map<unsigned, unsigned> isKingdomOf;
	std::map<unsigned long long, unsigned> componentPairTable;

	for (auto Val : xRanges) {
		if (!Val.isEnd) {
			for (auto [useful, unuseful] : isKingdomOf) {
				auto [lo, hi] = std::minmax({ useful, Val.componentIndex });
				indexProxy temp;
				temp.index[0] = lo;
				temp.index[1] = hi;
				componentPairTable[temp.hashVal]++;
			}
			++isKingdomOf[Val.componentIndex];
		}
		else {
			isKingdomOf.erase(isKingdomOf.find(Val.componentIndex));
		}
	}

	for (auto Val : yRanges) {
		if (!Val.isEnd) {
			for (auto [useful, unuseful] : isKingdomOf) {
				auto [lo, hi] = std::minmax({ useful, Val.componentIndex });
				indexProxy temp;
				temp.index[0] = lo;
				temp.index[1] = hi;
				auto it = componentPairTable.find(temp.hashVal);
				if (it != componentPairTable.end()) {
					//if(componentPairTable.find(in))
					componentPairTable[temp.hashVal]++;
				}
			}
			++isKingdomOf[Val.componentIndex];
		}
		else {
			isKingdomOf.erase(isKingdomOf.find(Val.componentIndex));
		}
	}

	for (auto Val : zRanges) {
		if (!Val.isEnd) {
			for (auto [useful, unuseful] : isKingdomOf) {
				auto [lo, hi] = std::minmax({ useful, Val.componentIndex });
				indexProxy temp;
				temp.index[0] = lo;
				temp.index[1] = hi;
				auto it = componentPairTable.find(temp.hashVal);
				if (it != componentPairTable.end()) {
					//if(componentPairTable.find(in))
					componentPairTable[temp.hashVal]++;
				}
			}
			++isKingdomOf[Val.componentIndex];
		}
		else {
			isKingdomOf.erase(isKingdomOf.find(Val.componentIndex));
		}
	}
	
	for (auto [proxyDat, val] : componentPairTable) {
		if (val != 3) continue;
		else {
			indexProxy temp;
			temp.hashVal = proxyDat;
			this->mIndexArr.emplace_back(temp.index[0], temp.index[1]);
		}
	}

	
}
#endif

#if defined(COLLIDER_USE_SUPER_FILTER)
void Collider::broadPhaseProcess_FILTER() {
	Collider::IndexType componmentSize = this->mBuildDat.size();
	struct rangeProxy {
		Collider::IndexType componentIndex;
		Collider::Scalar rangeT;
		rangeProxy() = default;
		rangeProxy(unsigned one, Collider::Scalar three) {
			componentIndex = one;
			rangeT = three;
		}
	};

	std::vector<rangeProxy>
		xBeginIndex, xEndIndex,
		yBeginIndex, yEndIndex, 
		zBeginIndex, zEndIndex;
	xBeginIndex.reserve(componmentSize);
	yBeginIndex.reserve(componmentSize);
	zBeginIndex.reserve(componmentSize);
	xEndIndex.reserve(componmentSize);
	yEndIndex.reserve(componmentSize);
	zEndIndex.reserve(componmentSize);

	Collider::IndexType curIndex = 0;
	for (const auto& dat : this->mBuildDat) {
		const auto Bx = dat.bounding_box();
		xBeginIndex.emplace_back(curIndex, Bx.min[0]);
		yBeginIndex.emplace_back(curIndex, Bx.min[1]);
		zBeginIndex.emplace_back(curIndex, Bx.min[2]);
		xEndIndex.emplace_back(curIndex, Bx.max[0]);
		yEndIndex.emplace_back(curIndex, Bx.max[1]);
		zEndIndex.emplace_back(curIndex, Bx.max[2]);
		++curIndex;
	}

	const auto sortRule = [](rangeProxy fir, rangeProxy sec) -> bool {
		return fir.rangeT < sec.rangeT;
	};
#pragma omp task shared(xBeginIndex)
	std::sort(xBeginIndex.begin(), xBeginIndex.end(), sortRule);
#pragma omp task shared(yBeginIndex)
	std::sort(yBeginIndex.begin(), yBeginIndex.end(), sortRule);
#pragma omp task shared(zBeginIndex)
	std::sort(zBeginIndex.begin(), zBeginIndex.end(), sortRule);
#pragma omp task shared(xEndIndex)
	std::sort(xEndIndex.begin(), xEndIndex.end(), sortRule);
#pragma omp task shared(yEndIndex)
	std::sort(yEndIndex.begin(), yEndIndex.end(), sortRule);
#pragma omp task shared(zEndIndex)
	std::sort(zEndIndex.begin(), zEndIndex.end(), sortRule);
#pragma omp taskwait

	std::unique_ptr<uint8_t[]> isImpossibleMap
		= std::make_unique<uint8_t[]>(componmentSize);
	for (Collider::IndexType p = 0; p < componmentSize; ++p) {
		::memset(isImpossibleMap.get(), 0, componmentSize);
		const auto Bx = this->mBuildDat[p].bounding_box();
		const auto lox = Bx.min[0], loy = Bx.min[1], loz = Bx.min[2];
		const auto hix = Bx.max[0], hiy = Bx.max[1], hiz = Bx.max[2];
		auto* const nakePtr = isImpossibleMap.get();
		const auto xlob = std::lower_bound(xEndIndex.begin(), xEndIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)lox }, sortRule) - xEndIndex.begin();
		const auto xhib = std::upper_bound(xBeginIndex.begin(), xBeginIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)hix }, sortRule) - xBeginIndex.begin();
		const auto xBeginSize = xBeginIndex.size();
#pragma omp task firstprivate(xlob, xhib, nakePtr, xBeginSize) shared(xBeginIndex, xEndIndex)
		{
			for (std::size_t pointer = 0; pointer < xlob; ++pointer) {
				nakePtr[xEndIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
			for (std::size_t pointer = xhib; pointer < xBeginSize; ++pointer) {
				nakePtr[xBeginIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
		}

		const auto ylob = std::lower_bound(yEndIndex.begin(), yEndIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)loy }, sortRule) - yEndIndex.begin();
		const auto yhib = std::upper_bound(yBeginIndex.begin(), yBeginIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)hiy }, sortRule) - yBeginIndex.begin();
		const auto yBeginSize = yBeginIndex.size();
#pragma omp task firstprivate(ylob, yhib, nakePtr, yBeginSize) shared(yBeginIndex, yEndIndex)
		{
			for (std::size_t pointer = 0; pointer < ylob; ++pointer) {
				nakePtr[yEndIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
			for (std::size_t pointer = yhib; pointer < yBeginSize; ++pointer) {
				nakePtr[yBeginIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
		}

		const auto zlob = std::lower_bound(zEndIndex.begin(), zEndIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)loz }, sortRule) - zEndIndex.begin();
		const auto zhib = std::upper_bound(zBeginIndex.begin(), zBeginIndex.end(), rangeProxy{ (Collider::IndexType)p, (Collider::Scalar)hiz }, sortRule) - zBeginIndex.begin();
		const auto zBeginSize = zBeginIndex.size();
#pragma omp task firstprivate(zlob, zhib, nakePtr, zBeginSize) shared(zBeginIndex, zEndIndex)
		{
			for (std::size_t pointer = 0; pointer < zlob; ++pointer) {
				nakePtr[zEndIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
			for (std::size_t pointer = zhib; pointer < zBeginSize; ++pointer) {
				nakePtr[zBeginIndex[pointer].componentIndex] |= uint8_t(0xFF);
			}
		}
		
#pragma omp taskwait

		for (Collider::IndexType fir = p + 1; fir < componmentSize; ++fir) {
			if (isImpossibleMap[fir] == (uint8_t)0) {
				this->mIndexArr.emplace_back(p, fir);
			}
		}

	}
}
#endif

void Collider::conditionFilter() {
	decltype(this->mIndexArr) tempIndexArr;
	tempIndexArr.reserve(mIndexArr.size());
	for (auto [lhs, rhs] : mIndexArr) {
		if (this->mCondition(mRawData->componentDatas[lhs], mRawData->componentDatas[rhs]))
		{
			tempIndexArr.emplace_back(lhs, rhs);
		}
	}
	this->mIndexArr = std::move(tempIndexArr);
	#if defined(COLLIDER_DEBUG) && defined(COLLIDER_PRINT_RUNTIME)
	std::cout << "after condition " << this->mIndexArr.size() << std::endl;
	#endif
}

void Collider::narrowPhaseProcess(){
	decltype(this->mIndexArr) tempArr;
	tempArr.reserve(this->mIndexArr.size());
	std::unique_ptr<bool[]> legalDummys = std::make_unique<bool[]>(this->mIndexArr.size());
	auto siz = this->mIndexArr.size();
#ifndef _MSC_VER
		auto procCnt = omp_get_num_threads();
#pragma omp parallel for schedule(static, procCnt)
	for (std::size_t p = 0; p < siz; ++p) {
		const auto [lhs, rhs] = this->mIndexArr[p];
		const auto& crefL = this->mRawData->search_m[lhs]; // 
		const auto& crefR = this->mRawData->search_m[rhs]; // point
		const auto& crefDat = this->mRawData->verts;
		legalDummys[p] = false;
		for (std::size_t lPtr = 0; lPtr < crefL.size() && !legalDummys[p]; lPtr += 3) {
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
					legalDummys[p] = true;
					break;
				}
			}
		}
	}
#else
	// fuck MSVC
	auto procCnt = 8;
	auto chuckSiz = siz / std::size_t(procCnt);
	auto coreProgram = [this, mLegalDummys = legalDummys.get()](std::size_t rangeLhs, std::size_t rangeRhs) -> void {
		for (std::size_t p = rangeLhs; p < rangeRhs; ++p) {
			const auto [lhs, rhs] = this->mIndexArr[p];
			const auto& crefL = this->mRawData->search_m[lhs];
			const auto& crefR = this->mRawData->search_m[rhs]; 
			const auto& crefDat = this->mRawData->verts;
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
#endif
	for (std::size_t p = 0; p < this->mIndexArr.size(); ++p) 
		if (legalDummys[p]) 
			tempArr.emplace_back(this->mIndexArr[p]);
		
	this->mIndexArr = std::move(tempArr);
}

#if defined(COLLIDER_USE_BVH)
void Collider::processBVH(const Bvh & mBvh){
	// auto nodeCnt = mBvh.node_count;
	struct alignas(16) RecursiveInfo{
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
	recurStack.push(RecursiveInfo{0, 0});
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
					for (int p1 = 0; p1 < firCnt; ++p1){
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
	std::sort(this->mIndexArr.begin(), this->mIndexArr.end(), [](const indexPair fir, const indexPair sec){
		if(fir.Lf != sec.Lf){
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
