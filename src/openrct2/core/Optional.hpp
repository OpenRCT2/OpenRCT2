/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __has_include
#    if __has_include(<optional>)
#        include <optional>
#    elif __has_include(<experimental/optional>)
#        include <experimental/optional>
#    else
#        error Missing <optional>
#    endif
#else
#    error __has_include is not defined
#endif

// `optional` and friends will be available in NS `::opt`
#if __has_include(<optional>)
namespace opt = std;
#elif __has_include(<experimental/optional>)
namespace opt = std::experimental;
#endif
