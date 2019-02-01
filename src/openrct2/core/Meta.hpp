/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <type_traits>

namespace Meta
{
    /**
     * Meta function for checking that all Conditions are true types.
     */
    template<typename... Conditions> struct all : std::true_type
    {
    };

    template<typename Condition, typename... Conditions>
    struct all<Condition, Conditions...> : std::conditional<Condition::value, all<Conditions...>, std::false_type>::type
    {
    };

    template<typename Type, typename... Types> using all_convertible = all<std::is_convertible<Types, Type>...>;

} // namespace Meta
