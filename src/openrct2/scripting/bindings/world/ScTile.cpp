/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScTile.hpp"

    #include "../../../Context.h"
    #include "../../../core/Guard.hpp"
    #include "../../../drawing/ScrollingText.h"
    #include "../../../entity/EntityRegistry.h"
    #include "../../../object/LargeSceneryEntry.h"
    #include "../../../ride/Track.h"
    #include "../../../world/Footpath.h"
    #include "../../../world/Map.h"
    #include "../../../world/Scenery.h"
    #include "../../../world/tile_element/LargeSceneryElement.h"
    #include "../../ScriptEngine.h"
    #include "ScTileElement.hpp"

    #include <cstdio>
    #include <cstring>
    #include <utility>

namespace OpenRCT2::Scripting
{
    using OpaqueTileData = struct
    {
        CoordsXY coords;
    };

    JSValue ScTile::x_get(JSContext* ctx, JSValue thisValue)
    {
        auto coords = GetCoordinates(thisValue);
        return JS_NewInt32(ctx, coords.x / kCoordsXYStep);
    }

    JSValue ScTile::y_get(JSContext* ctx, JSValue thisValue)
    {
        auto coords = GetCoordinates(thisValue);
        return JS_NewInt32(ctx, coords.y / kCoordsXYStep);
    }

    JSValue ScTile::numElements_get(JSContext* ctx, JSValue thisValue)
    {
        auto first = GetFirstElement(thisValue);
        return JS_NewUint32(ctx, GetNumElements(first));
    }

    JSValue ScTile::elements_get(JSContext* ctx, JSValue thisValue)
    {
        auto array = JS_NewArray(ctx);
        auto coords = GetCoordinates(thisValue);
        auto first = MapGetFirstElementAt(coords);
        auto currentNumElements = GetNumElements(first);
        if (currentNumElements != 0)
        {
            JS_SetLength(ctx, array, currentNumElements);
            for (size_t i = 0; i < currentNumElements; i++)
            {
                JS_SetPropertyInt64(ctx, array, i, gScTileElement.New(ctx, &first[i], coords));
            }
        }
        return array;
    }

    JSValue ScTile::data_get(JSContext* ctx, JSValue thisValue)
    {
        auto first = GetFirstElement(thisValue);
        auto dataLen = GetNumElements(first) * sizeof(TileElement);
        if (first != nullptr)
        {
            return JS_NewUint8ArrayCopy(ctx, reinterpret_cast<const uint8_t*>(first), dataLen);
        }
        return JS_NewUint8ArrayCopy(ctx, nullptr, 0);
    }

    JSValue ScTile::data_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        if (JS_GetTypedArrayType(jsValue) == JSTypedArrayEnum::JS_TYPED_ARRAY_UINT8)
        {
            auto coords = GetCoordinates(thisValue);
            int64_t dataLength{};
            JS_GetLength(ctx, jsValue, &dataLength);
            auto dataSize = static_cast<size_t>(dataLength);
            auto* array = JS_GetUint8Array(ctx, &dataSize, jsValue);
            auto numElements = dataLength / sizeof(TileElement);
            if (numElements == 0)
            {
                MapSetTileElement(TileCoordsXY(coords), nullptr);
            }
            else
            {
                auto first = MapGetFirstElementAt(coords);
                auto currentNumElements = GetNumElements(first);
                if (numElements > currentNumElements)
                {
                    // Allocate space for the extra tile elements (inefficient but works)
                    auto pos = TileCoordsXYZ(TileCoordsXY(coords), 0).ToCoordsXYZ();
                    auto numToInsert = numElements - currentNumElements;
                    for (size_t i = 0; i < numToInsert; i++)
                    {
                        TileElementInsert(pos, 0, TileElementType::Surface);
                    }

                    // Copy data to element span
                    first = MapGetFirstElementAt(coords);
                    currentNumElements = GetNumElements(first);
                    if (currentNumElements != 0)
                    {
                        std::memcpy(first, array, currentNumElements * sizeof(TileElement));
                        // Safely force last tile flag for last element to avoid read overrun
                        first[numElements - 1].SetLastForTile(true);
                    }
                }
                else
                {
                    std::memcpy(first, array, numElements * sizeof(TileElement));
                    // Safely force last tile flag for last element to avoid read overrun
                    first[numElements - 1].SetLastForTile(true);
                }
            }
            MapInvalidateTileFull(coords);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTile::getElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv)
    {
        JS_UNPACK_UINT32(index, ctx, argv[0]);
        auto coords = GetCoordinates(thisValue);
        auto first = MapGetFirstElementAt(coords);
        if (static_cast<size_t>(index) < GetNumElements(first))
        {
            return gScTileElement.New(ctx, &first[index], coords);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTile::insertElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_UINT32(index, ctx, argv[0]);
        auto coords = GetCoordinates(thisValue);
        auto first = MapGetFirstElementAt(coords);
        auto origNumElements = GetNumElements(first);
        if (index <= origNumElements)
        {
            std::vector<TileElement> data(first, first + origNumElements);

            auto pos = TileCoordsXYZ(TileCoordsXY(coords), 0).ToCoordsXYZ();
            auto newElement = TileElementInsert(pos, 0, TileElementType::Surface);
            if (newElement == nullptr)
            {
                JS_ThrowPlainError(ctx, "Unable to allocate element.");
                return JS_EXCEPTION;
            }

            // Inefficient, requires a dedicated method in tile element manager
            first = MapGetFirstElementAt(coords);
            // Copy elements before index
            if (index > 0)
            {
                std::memcpy(first, &data[0], index * sizeof(TileElement));
            }
            // Zero new element
            std::memset(first + index, 0, sizeof(TileElement));
            // Copy elements after index
            if (index < origNumElements)
            {
                std::memcpy(first + index + 1, &data[index], (origNumElements - index) * sizeof(TileElement));
            }
            for (size_t i = 0; i < origNumElements; i++)
            {
                first[i].SetLastForTile(false);
            }
            first[origNumElements].SetLastForTile(true);
            MapInvalidateTileFull(coords);
            return gScTileElement.New(ctx, &first[index], coords);
        }
        else
        {
            JS_ThrowPlainError(ctx, "Index must be between zero and the number of elements on the tile.");
            return JS_EXCEPTION;
        }
    }

    JSValue ScTile::removeElement(JSContext* ctx, JSValue thisValue, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_UINT32(index, ctx, argv[0]);
        auto coords = GetCoordinates(thisValue);
        auto first = MapGetFirstElementAt(coords);
        if (index < GetNumElements(first))
        {
            auto element = &first[index];
            if (element->GetType() != TileElementType::LargeScenery
                || element->AsLargeScenery()->GetEntry()->scrolling_mode == kScrollingModeNone
                || ScTileElement::GetOtherLargeSceneryElement(coords, element->AsLargeScenery()) == nullptr)
            {
                element->RemoveBannerEntry();
            }
            TileElementRemove(&first[index]);
            MapInvalidateTileFull(coords);
        }
        return JS_UNDEFINED;
    }

    CoordsXY ScTile::GetCoordinates(JSValue thisValue)
    {
        return gScTile.GetOpaque<OpaqueTileData*>(thisValue)->coords;
    }

    TileElement* ScTile::GetFirstElement(JSValue thisValue)
    {
        auto coords = GetCoordinates(thisValue);
        return MapGetFirstElementAt(coords);
    }

    uint32_t ScTile::GetNumElements(const TileElement* first)
    {
        uint32_t count = 0;
        if (first != nullptr)
        {
            auto element = first;
            do
            {
                count++;
            } while (!(element++)->IsLastForTile());
        }
        return count;
    }

    JSValue ScTile::New(JSContext* ctx, CoordsXY& coords)
    {
        static constexpr JSCFunctionListEntry funcs[] = { JS_CGETSET_DEF("x", ScTile::x_get, nullptr),
                                                          JS_CGETSET_DEF("y", ScTile::y_get, nullptr),
                                                          JS_CGETSET_DEF("elements", ScTile::elements_get, nullptr),
                                                          JS_CGETSET_DEF("numElements", ScTile::numElements_get, nullptr),
                                                          JS_CGETSET_DEF("data", ScTile::data_get, ScTile::data_set),
                                                          JS_CFUNC_DEF("getElement", 1, ScTile::getElement),
                                                          JS_CFUNC_DEF("insertElement", 1, ScTile::insertElement),
                                                          JS_CFUNC_DEF("removeElement", 0, ScTile::removeElement) };
        return MakeWithOpaque(ctx, funcs, new OpaqueTileData{ coords });
    }

    void ScTile::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Tile", Finalize);
    }

    void ScTile::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueTileData* data = gScTile.GetOpaque<OpaqueTileData*>(thisVal);
        if (data)
            delete data;
    }

} // namespace OpenRCT2::Scripting

#endif
