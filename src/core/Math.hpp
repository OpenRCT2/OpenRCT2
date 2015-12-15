#pragma once

#include <algorithm>

/**
 * Common mathematical functions.
 */
namespace Math {

	template<typename T>
	T Min(T a, T b) {
		return (std::min)(a, b);
	}

	template<typename T>
	T Max(T a, T b) {
		return (std::max)(a, b);
	}

	template<typename T>
	T Clamp(T low, T x, T high) {
		return (std::min)((std::max)(low, x), high);
	}

}
