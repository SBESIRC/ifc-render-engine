#pragma once
#ifndef IFCRE_STD_TYPES_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include <memory>

namespace ifcre {
	typedef std::string String;

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

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;

};

#endif