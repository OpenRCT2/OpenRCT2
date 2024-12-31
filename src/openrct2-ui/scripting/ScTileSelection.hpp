/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include <openrct2/scripting/Duktape.hpp>
    #include <openrct2/world/Map.h>

namespace OpenRCT2::Scripting
{
    class ScTileSelection
    {
    private:
        duk_context* _ctx{};

    public:
        ScTileSelection(duk_context* ctx)
            : _ctx(ctx)
        {
        }

        DukValue range_get() const
        {
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE)
            {
                DukObject range(_ctx);

                DukObject leftTop(_ctx);
                leftTop.Set("x", gMapSelectPositionA.x);
                leftTop.Set("y", gMapSelectPositionA.y);
                range.Set("leftTop", leftTop.Take());

                DukObject rightBottom(_ctx);
                rightBottom.Set("x", gMapSelectPositionB.x);
                rightBottom.Set("y", gMapSelectPositionB.y);
                range.Set("rightBottom", rightBottom.Take());
                return range.Take();
            }

            duk_push_null(_ctx);
            return DukValue::take_from_stack(_ctx);
        }

        void range_set(DukValue value)
        {
            MapInvalidateSelectionRect();
            if (value.type() == DukValue::Type::OBJECT)
            {
                auto range = GetMapRange(value);
                if (range)
                {
                    gMapSelectPositionA.x = range->GetLeft();
                    gMapSelectPositionA.y = range->GetTop();
                    gMapSelectPositionB.x = range->GetRight();
                    gMapSelectPositionB.y = range->GetBottom();
                    gMapSelectType = MAP_SELECT_TYPE_FULL;
                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                }
            }
            else
            {
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            }
            MapInvalidateSelectionRect();
        }

        DukValue tiles_get() const
        {
            duk_push_array(_ctx);
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
            {
                duk_uarridx_t index = 0;
                for (const auto& tile : gMapSelectionTiles)
                {
                    duk_push_object(_ctx);
                    duk_push_int(_ctx, tile.x);
                    duk_put_prop_string(_ctx, -2, "x");
                    duk_push_int(_ctx, tile.y);
                    duk_put_prop_string(_ctx, -2, "y");
                    duk_put_prop_index(_ctx, -2, index);
                    index++;
                }
            }
            return DukValue::take_from_stack(_ctx);
        }

        void tiles_set(DukValue value)
        {
            MapInvalidateMapSelectionTiles();
            gMapSelectionTiles.clear();
            if (value.is_array())
            {
                value.push();
                auto arrayLen = duk_get_length(_ctx, -1);
                for (duk_uarridx_t i = 0; i < arrayLen; i++)
                {
                    if (duk_get_prop_index(_ctx, -1, i))
                    {
                        auto dukElement = DukValue::take_from_stack(_ctx);
                        auto coords = GetCoordsXY(dukElement);
                        if (coords)
                        {
                            gMapSelectionTiles.push_back(*coords);
                        }
                    }
                }
                duk_pop(_ctx);
            }

            if (gMapSelectionTiles.empty())
            {
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
            }
            else
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            }
            MapInvalidateMapSelectionTiles();
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTileSelection::range_get, &ScTileSelection::range_set, "range");
            dukglue_register_property(ctx, &ScTileSelection::tiles_get, &ScTileSelection::tiles_set, "tiles");
        }

    private:
        static std::optional<CoordsXY> GetCoordsXY(const DukValue& dukCoords)
        {
            if (dukCoords.type() == DukValue::Type::OBJECT)
            {
                auto dukX = dukCoords["x"];
                if (dukX.type() == DukValue::Type::NUMBER)
                {
                    auto dukY = dukCoords["y"];
                    if (dukY.type() == DukValue::Type::NUMBER)
                    {
                        return CoordsXY(dukX.as_int(), dukY.as_int());
                    }
                }
            }
            return std::nullopt;
        }

        static std::optional<MapRange> GetMapRange(const DukValue& dukMapRange)
        {
            if (dukMapRange.type() == DukValue::Type::OBJECT)
            {
                auto leftTop = GetCoordsXY(dukMapRange["leftTop"]);
                if (leftTop.has_value())
                {
                    auto rightBottom = GetCoordsXY(dukMapRange["rightBottom"]);
                    if (rightBottom.has_value())
                    {
                        return MapRange(leftTop->x, leftTop->y, rightBottom->x, rightBottom->y);
                    }
                }
            }
            return std::nullopt;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
