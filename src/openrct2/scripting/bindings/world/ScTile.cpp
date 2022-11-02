/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScTile.hpp"

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../core/Guard.hpp"
#    include "../../../entity/EntityRegistry.h"
#    include "../../../ride/Track.h"
#    include "../../../world/Footpath.h"
#    include "../../../world/Scenery.h"
#    include "../../../world/Surface.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "ScTileElement.hpp"

#    include <cstdio>
#    include <cstring>
#    include <utility>

namespace OpenRCT2::Scripting
{
    ScTile::ScTile(const CoordsXY& coords)
        : _coords(coords)
    {
    }

    int32_t ScTile::x_get() const
    {
        return _coords.x / COORDS_XY_STEP;
    }

    int32_t ScTile::y_get() const
    {
        return _coords.y / COORDS_XY_STEP;
    }

    uint32_t ScTile::numElements_get() const
    {
        auto first = GetFirstElement();
        return static_cast<int32_t>(GetNumElements(first));
    }

    std::vector<std::shared_ptr<ScTileElement>> ScTile::elements_get() const
    {
        std::vector<std::shared_ptr<ScTileElement>> result;
        auto first = GetFirstElement();
        auto currentNumElements = GetNumElements(first);
        if (currentNumElements != 0)
        {
            result.reserve(currentNumElements);
            for (size_t i = 0; i < currentNumElements; i++)
            {
                result.push_back(std::make_shared<ScTileElement>(_coords, &first[i]));
            }
        }
        return result;
    }

    DukValue ScTile::data_get() const
    {
        auto ctx = GetDukContext();
        auto first = MapGetFirstElementAt(_coords);
        auto dataLen = GetNumElements(first) * sizeof(TileElement);
        auto data = duk_push_fixed_buffer(ctx, dataLen);
        if (first != nullptr)
        {
            std::memcpy(data, first, dataLen);
        }
        duk_push_buffer_object(ctx, -1, 0, dataLen, DUK_BUFOBJ_UINT8ARRAY);
        return DukValue::take_from_stack(ctx);
    }

    void ScTile::data_set(DukValue value)
    {
        ThrowIfGameStateNotMutable();
        auto ctx = value.context();
        value.push();
        if (duk_is_buffer_data(ctx, -1))
        {
            duk_size_t dataLen{};
            auto data = duk_get_buffer_data(ctx, -1, &dataLen);
            auto numElements = dataLen / sizeof(TileElement);
            if (numElements == 0)
            {
                MapSetTileElement(TileCoordsXY(_coords), nullptr);
            }
            else
            {
                auto first = GetFirstElement();
                auto currentNumElements = GetNumElements(first);
                if (numElements > currentNumElements)
                {
                    // Allocate space for the extra tile elements (inefficient but works)
                    auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0).ToCoordsXYZ();
                    auto numToInsert = numElements - currentNumElements;
                    for (size_t i = 0; i < numToInsert; i++)
                    {
                        TileElementInsert(pos, 0, TileElementType::Surface);
                    }

                    // Copy data to element span
                    first = MapGetFirstElementAt(_coords);
                    currentNumElements = GetNumElements(first);
                    if (currentNumElements != 0)
                    {
                        std::memcpy(first, data, currentNumElements * sizeof(TileElement));
                        // Safely force last tile flag for last element to avoid read overrun
                        first[numElements - 1].SetLastForTile(true);
                    }
                }
                else
                {
                    std::memcpy(first, data, numElements * sizeof(TileElement));
                    // Safely force last tile flag for last element to avoid read overrun
                    first[numElements - 1].SetLastForTile(true);
                }
            }
            MapInvalidateTileFull(_coords);
        }
    }

    std::shared_ptr<ScTileElement> ScTile::getElement(uint32_t index) const
    {
        auto first = GetFirstElement();
        if (static_cast<size_t>(index) < GetNumElements(first))
        {
            return std::make_shared<ScTileElement>(_coords, &first[index]);
        }
        return {};
    }

    std::shared_ptr<ScTileElement> ScTile::insertElement(uint32_t index)
    {
        ThrowIfGameStateNotMutable();
        std::shared_ptr<ScTileElement> result;
        auto first = GetFirstElement();
        auto origNumElements = GetNumElements(first);
        if (index <= origNumElements)
        {
            std::vector<TileElement> data(first, first + origNumElements);

            auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0).ToCoordsXYZ();
            auto newElement = TileElementInsert(pos, 0, TileElementType::Surface);
            if (newElement == nullptr)
            {
                auto ctx = GetDukContext();
                duk_error(ctx, DUK_ERR_ERROR, "Unable to allocate element.");
            }
            else
            {
                // Inefficient, requires a dedicated method in tile element manager
                first = GetFirstElement();
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
                MapInvalidateTileFull(_coords);
                result = std::make_shared<ScTileElement>(_coords, &first[index]);
            }
        }
        else
        {
            auto ctx = GetDukContext();
            duk_error(ctx, DUK_ERR_RANGE_ERROR, "Index must be between zero and the number of elements on the tile.");
        }
        return result;
    }

    void ScTile::removeElement(uint32_t index)
    {
        ThrowIfGameStateNotMutable();
        auto first = GetFirstElement();
        if (index < GetNumElements(first))
        {
            TileElementRemove(&first[index]);
            MapInvalidateTileFull(_coords);
        }
    }

    TileElement* ScTile::GetFirstElement() const
    {
        return MapGetFirstElementAt(_coords);
    }

    size_t ScTile::GetNumElements(const TileElement* first)
    {
        size_t count = 0;
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

    duk_context* ScTile::GetDukContext() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto ctx = scriptEngine.GetContext();
        return ctx;
    }

    void ScTile::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScTile::x_get, nullptr, "x");
        dukglue_register_property(ctx, &ScTile::y_get, nullptr, "y");
        dukglue_register_property(ctx, &ScTile::elements_get, nullptr, "elements");
        dukglue_register_property(ctx, &ScTile::numElements_get, nullptr, "numElements");
        dukglue_register_property(ctx, &ScTile::data_get, &ScTile::data_set, "data");
        dukglue_register_method(ctx, &ScTile::getElement, "getElement");
        dukglue_register_method(ctx, &ScTile::insertElement, "insertElement");
        dukglue_register_method(ctx, &ScTile::removeElement, "removeElement");
    }

} // namespace OpenRCT2::Scripting

#endif
