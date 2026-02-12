/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Identifier.hpp"
#include "../network/NetworkTypes.h"
#include "../world/Location.hpp"

#include <string>
#include <string_view>

namespace OpenRCT2::GameActions
{
    // GCC 15.1 is overzealous with its 'final' warnings
#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

    class GameActionParameterVisitor
    {
    public:
        virtual ~GameActionParameterVisitor() = default;

        virtual void Visit(std::string_view name, bool& param)
        {
        }

        virtual void Visit(std::string_view name, int32_t& param)
        {
        }

        virtual void Visit(std::string_view name, std::string& param)
        {
        }

        void Visit(CoordsXY& param)
        {
            Visit("x", param.x);
            Visit("y", param.y);
        }

        void Visit(CoordsXYZ& param)
        {
            Visit("x", param.x);
            Visit("y", param.y);
            Visit("z", param.z);
        }

        void Visit(CoordsXYZD& param)
        {
            Visit("x", param.x);
            Visit("y", param.y);
            Visit("z", param.z);
            Visit("direction", param.direction);
        }

        void Visit(MapRange& param)
        {
            Visit("x1", param.Point1.x);
            Visit("y1", param.Point1.y);
            Visit("x2", param.Point2.x);
            Visit("y2", param.Point2.y);
        }

        template<typename T>
        void Visit(std::string_view name, T& param)
        {
            static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "Not an arithmetic type");
            auto value = static_cast<int32_t>(param);
            Visit(name, value);
            param = static_cast<T>(value);
        }

        template<typename T, T TNull, typename TTag>
        void Visit(std::string_view name, TIdentifier<T, TNull, TTag>& param)
        {
            auto value = param.ToUnderlying();
            Visit(name, value);
            param = TIdentifier<T, TNull, TTag>::FromUnderlying(value);
        }

        template<typename T, size_t _TypeID>
        void Visit(std::string_view name, Network::NetworkObjectId<T, _TypeID>& param)
        {
            Visit(name, param.id);
        }
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

} // namespace OpenRCT2::GameActions
