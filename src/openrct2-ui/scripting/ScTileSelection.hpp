/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include <openrct2/world/MapSelection.h>

namespace OpenRCT2::Scripting
{
    class ScTileSelection;
    extern ScTileSelection gScTileSelection;

    class ScTileSelection final : public ScBase
    {
    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "TileSelection");
        }

        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("range", ScTileSelection::range_get, ScTileSelection::range_set),
                JS_CGETSET_DEF("tiles", ScTileSelection::tiles_get, ScTileSelection::tiles_set),
            };
            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        static JSValue range_get(JSContext* ctx, JSValue thisVal)
        {
            if (gMapSelectFlags.has(MapSelectFlag::enable))
            {
                JSValue range = JS_NewObject(ctx);

                JSValue leftTop = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, leftTop, "x", JS_NewInt32(ctx, gMapSelectPositionA.x));
                JS_SetPropertyStr(ctx, leftTop, "y", JS_NewInt32(ctx, gMapSelectPositionA.y));
                JS_SetPropertyStr(ctx, range, "leftTop", leftTop);

                JSValue rightBottom = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, rightBottom, "x", JS_NewInt32(ctx, gMapSelectPositionB.x));
                JS_SetPropertyStr(ctx, rightBottom, "y", JS_NewInt32(ctx, gMapSelectPositionB.y));
                JS_SetPropertyStr(ctx, range, "rightBottom", rightBottom);
                return range;
            }

            return JS_NULL;
        }

        static JSValue range_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            if (JS_IsObject(value))
            {
                auto range = GetMapRange(ctx, value);
                if (range)
                {
                    gMapSelectPositionA.x = range->GetX1();
                    gMapSelectPositionA.y = range->GetY1();
                    gMapSelectPositionB.x = range->GetX2();
                    gMapSelectPositionB.y = range->GetY2();
                    gMapSelectType = MapSelectType::full;
                    gMapSelectFlags.set(MapSelectFlag::enable);
                }
            }
            else
            {
                gMapSelectFlags.unset(MapSelectFlag::enable);
            }

            return JS_UNDEFINED;
        }

        static JSValue tiles_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue tiles = JS_NewArray(ctx);
            if (gMapSelectFlags.has(MapSelectFlag::enableConstruct))
            {
                int64_t index = 0;
                for (const auto& tile : MapSelection::getSelectedTiles())
                {
                    JSValue val = JS_NewObject(ctx);
                    JS_SetPropertyStr(ctx, val, "x", JS_NewInt32(ctx, tile.x));
                    JS_SetPropertyStr(ctx, val, "y", JS_NewInt32(ctx, tile.y));
                    JS_SetPropertyInt64(ctx, tiles, index, val);
                    index++;
                }
            }
            return tiles;
        }

        static JSValue tiles_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            MapSelection::clearSelectedTiles();
            if (JS_IsArray(value))
            {
                JSIterateArray(ctx, value, [](JSContext* ctx2, JSValue v) {
                    auto coords = GetCoordsXY(ctx2, v);
                    if (coords.has_value())
                    {
                        MapSelection::addSelectedTile(coords.value());
                    }
                });
            }

            if (MapSelection::getSelectedTiles().empty())
            {
                gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
                gMapSelectFlags.unset(MapSelectFlag::green);
            }
            else
            {
                gMapSelectFlags.set(MapSelectFlag::enableConstruct);
            }

            return JS_UNDEFINED;
        }

    private:
        static std::optional<CoordsXY> GetCoordsXY(JSContext* ctx, JSValue jsCoords)
        {
            if (JS_IsObject(jsCoords))
            {
                auto x = JSToOptionalInt(ctx, jsCoords, "x");
                auto y = JSToOptionalInt(ctx, jsCoords, "y");
                if (x.has_value() && y.has_value())
                {
                    return CoordsXY(x.value(), y.value());
                }
            }
            return std::nullopt;
        }

        static std::optional<MapRange> GetMapRange(JSContext* ctx, JSValue jsMapRange)
        {
            if (JS_IsObject(jsMapRange))
            {
                JSValue jsLeftTop = JS_GetPropertyStr(ctx, jsMapRange, "leftTop");
                auto leftTop = GetCoordsXY(ctx, jsLeftTop);
                JS_FreeValue(ctx, jsLeftTop);
                if (leftTop.has_value())
                {
                    JSValue jsRightBottom = JS_GetPropertyStr(ctx, jsMapRange, "rightBottom");
                    auto rightBottom = GetCoordsXY(ctx, jsRightBottom);
                    JS_FreeValue(ctx, jsRightBottom);
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
