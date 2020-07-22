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
    template<typename... _TConditions> struct all : std::true_type
    {
    };

    template<typename _TCondition, typename... _TConditions>
    struct all<_TCondition, _TConditions...> : std::conditional<_TCondition::value, all<_TConditions...>, std::false_type>::type
    {
    };

    template<typename _TType, typename... _TTypes> using all_convertible = all<std::is_convertible<_TTypes, _TType>...>;

} // namespace Meta
