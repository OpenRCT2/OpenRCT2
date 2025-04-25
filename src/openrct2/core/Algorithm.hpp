/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <functional>

namespace OpenRCT2::Core::Algorithm
{

    template<class ForwardIt, class T, class Compare = std::less<>>
    ForwardIt binaryFind(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
    {
        first = std::lower_bound(first, last, value, comp);
        return first != last && !comp(value, *first) ? first : last;
    }

    template<typename Container, typename T, class Compare = std::less<>>
    auto sortedInsert(Container& cont, const T& value, Compare comp = {})
    {
        if (cont.empty() || value < cont.front())
        {
            return cont.insert(cont.begin(), value);
        }
        if (value > cont.back())
        {
            return cont.insert(cont.end(), value);
        }
        return cont.insert(std::lower_bound(cont.begin(), cont.end(), value, comp), value);
    }

} // namespace OpenRCT2::Core::Algorithm
