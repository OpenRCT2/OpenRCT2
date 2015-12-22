#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <cstdlib>

/**
 * Common utility functions.
 */
namespace Util {

// Based on http://www.g-truc.net/post-0708.html
template <typename T, size_t N>
constexpr size_t CountOf(T const (&)[N]) noexcept
{
	return N;
}

} // namespace Util

#endif // _UTIL_HPP_
