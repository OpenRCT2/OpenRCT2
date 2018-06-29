/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <cstdlib>

/**
 * Common utility functions.
 */
namespace Util {

// Based on http://www.g-truc.net/post-0708.html
template <typename T, size_t N>
static constexpr size_t CountOf(T const (&)[N]) noexcept
{
    return N;
}

} // namespace Util

#endif // _UTIL_HPP_
