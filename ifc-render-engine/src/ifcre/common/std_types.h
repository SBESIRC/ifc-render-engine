#pragma once
#ifndef IFCRE_STD_TYPES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <memory>
#include <sstream>

namespace ifcre {
	typedef std::string String;
	typedef std::stringstream StringStream;

#define SINGLE_PRECISION
#ifdef SINGLE_PRECISION
	typedef float Real;
#else
	typedef double Real;
#endif

	template <typename T>
	using Vector = std::vector<T>;
	template <typename K, typename V>
	using Map = std::map<K, V>;
	using std::make_pair;

// ------------- pointer -------------
	template <typename T>
	using SharedPtr = std::shared_ptr<T>;
	using std::make_shared;

	template<typename T>
	using UniquePtr = std::unique_ptr<T>;
	using std::make_unique;

	template<typename T>
	using WeakPtr = std::weak_ptr<T>;
// ----- ----- ----- ----- ----- -----

};

#endif