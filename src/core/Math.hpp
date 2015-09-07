#pragma once

/**
 * Common mathematical functions.
 */
namespace Math {

	template<typename T>
	T Min(T a, T b) {
		return a < b ? a : b;
	}

	template<typename T>
	T Max(T a, T b) {
		return a > b ? a : b;
	}

	template<typename T>
	T Clamp(T low, T x, T max) {
		return Min(Max(low, x), high);
	}

}
