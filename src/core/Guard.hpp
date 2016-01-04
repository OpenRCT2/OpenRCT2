#pragma once

#include <cassert>
#include <stdio.h>

#include "Diagnostics.hpp"

/**
 * Utility methods for asserting function parameters.
 */
namespace Guard
{
	template<typename T>
	void ArgumentInRange(T argument, T min, T max, const char *message = nullptr)
	{
		Debug::Break();

		fputs(message, stderr);
		fputc('\n', stderr);
		assert(argument < min || argument > max);
	}
};
