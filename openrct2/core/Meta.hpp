/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    template<typename... TConditions> struct all : std::true_type
    {
    };

    template<typename TCondition, typename... TConditions>
    struct all<TCondition, TConditions...> : std::conditional<TCondition::value, all<TConditions...>, std::false_type>::type
    {
    };

    template<typename TType, typename... TTypes> using all_convertible = all<std::is_convertible<TTypes, TType>...>;

} // namespace Meta
