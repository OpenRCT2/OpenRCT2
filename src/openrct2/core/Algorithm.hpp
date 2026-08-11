/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <functional>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"

namespace OpenRCT2::Core::Algorithm
{

    template<class ForwardIt, class T, class Compare = std::less<>>
    ForwardIt binaryFind(ForwardIt first, ForwardIt last, T&& value, const Compare comp = {})
    {
        first = std::lower_bound(first, last, std::forward<T>(value), comp);
        return first != last && !comp(value, *first) ? first : last;
    }

    template<typename Container, typename T, class Compare = std::less<>>
    auto sortedInsert(Container& cont, T&& value, const Compare comp = {})
    {
        if (cont.empty() || value < cont.front())
        {
            return cont.insert(cont.begin(), value);
        }
        if (value > cont.back())
        {
            return cont.insert(cont.end(), value);
        }
        return cont.insert(std::lower_bound(cont.begin(), cont.end(), std::forward<T>(value), comp), std::forward<T>(value));
    }

} // namespace OpenRCT2::Core::Algorithm

#pragma GCC diagnostic pop
