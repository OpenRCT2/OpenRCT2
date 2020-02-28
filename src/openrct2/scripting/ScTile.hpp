/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../common.h"
#    include "../world/Footpath.h"
#    include "../world/Scenery.h"
#    include "../world/Sprite.h"
#    include "../world/Surface.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <cstdio>

namespace OpenRCT2::Scripting
{
    class ScSurfaceElement;

    class ScTileElement
    {
    protected:
        CoordsXY _coords;
        TileElement* _element;

    public:
        ScTileElement(CoordsXY coords, TileElement* element)
            : _coords(coords)
            , _element(element)
        {
        }

    private:
        std::string type_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SURFACE:
                    return "surface";
                case TILE_ELEMENT_TYPE_PATH:
                    return "footpath";
                case TILE_ELEMENT_TYPE_TRACK:
                    return "track";
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    return "small-scenery";
                case TILE_ELEMENT_TYPE_ENTRANCE:
                    return "entrance";
                case TILE_ELEMENT_TYPE_WALL:
                    return "wall";
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    return "large-scenery";
                case TILE_ELEMENT_TYPE_BANNER:
                    return "banner";
                case TILE_ELEMENT_TYPE_CORRUPT:
                    return "openrct2-corrupt";
                default:
                    return "unknown";
            }
        }

        void type_set(std::string value)
        {
            auto type = _element->type;
            if (value == "surface")
                type = TILE_ELEMENT_TYPE_SURFACE;
            else if (value == "footpath")
                type = TILE_ELEMENT_TYPE_PATH;
            else if (value == "track")
                type = TILE_ELEMENT_TYPE_TRACK;
            else if (value == "small-scenery")
                type = TILE_ELEMENT_TYPE_SMALL_SCENERY;
            else if (value == "entrance")
                type = TILE_ELEMENT_TYPE_ENTRANCE;
            else if (value == "wall")
                type = TILE_ELEMENT_TYPE_WALL;
            else if (value == "large-scenery")
                type = TILE_ELEMENT_TYPE_LARGE_SCENERY;
            else if (value == "banner")
                type = TILE_ELEMENT_TYPE_BANNER;
            else if (value == "openrct2-corrupt")
                type = TILE_ELEMENT_TYPE_CORRUPT;
            else
                return;
            _element->type = type;
            map_invalidate_tile_full(_coords);
        }

        bool broken_get() const
        {
            return _element->flags & TILE_ELEMENT_FLAG_BROKEN;
        }
        void broken_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            if (value)
            {
                _element->flags |= TILE_ELEMENT_FLAG_BROKEN;
            }
            else
            {
                _element->flags &= ~TILE_ELEMENT_FLAG_BROKEN;
            }
        }

        auto baseHeight_get() const
        {
            return _element->base_height;
        }
        void baseHeight_set(uint8_t newBaseHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->base_height = newBaseHeight;
        }

        auto clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8_t newClearanceHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->clearance_height = newClearanceHeight;
        }

        uint8_t grassLength_get()
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetGrassLength();
            return 0;
        }
        void grassLength_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
                el->SetGrassLengthAndInvalidate(value, _coords);
            }
        }

        bool hasOwnership_get()
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetOwnership() & OWNERSHIP_OWNED;
            return false;
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTileElement::type_get, &ScTileElement::type_set, "type");
            dukglue_register_property(ctx, &ScTileElement::baseHeight_get, &ScTileElement::baseHeight_set, "baseHeight");
            dukglue_register_property(
                ctx, &ScTileElement::clearanceHeight_get, &ScTileElement::clearanceHeight_set, "clearanceHeight");

            dukglue_register_property(ctx, &ScTileElement::broken_get, &ScTileElement::broken_set, "broken");
            dukglue_register_property(ctx, &ScTileElement::grassLength_get, &ScTileElement::grassLength_set, "grassLength");
            dukglue_register_property(ctx, &ScTileElement::hasOwnership_get, nullptr, "hasOwnership");
        }
    };

    class ScTile
    {
    private:
        CoordsXY _coords;

    public:
        ScTile(CoordsXY coords)
            : _coords(coords)
        {
        }

    private:
        int32_t x_get()
        {
            return _coords.x / 32;
        }

        int32_t y_get()
        {
            return _coords.y / 32;
        }

        size_t numElements_get()
        {
            auto first = GetFirstElement();
            return GetNumElements(first);
        }

        std::vector<std::shared_ptr<ScTileElement>> elements_get()
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

        DukValue data_get()
        {
            auto ctx = GetDukContext();
            auto first = map_get_first_element_at(_coords);
            auto dataLen = GetNumElements(first) * sizeof(TileElement);
            auto data = duk_push_fixed_buffer(ctx, dataLen);
            if (first != nullptr)
            {
                std::memcpy(data, first, dataLen);
            }
            duk_push_buffer_object(ctx, -1, 0, dataLen, DUK_BUFOBJ_UINT8ARRAY);
            return DukValue::take_from_stack(ctx);
        }

        void data_set(DukValue value)
        {
            auto ctx = value.context();
            value.push();
            if (duk_is_buffer_data(ctx, -1))
            {
                duk_size_t dataLen{};
                auto data = duk_get_buffer_data(ctx, -1, &dataLen);
                auto numElements = dataLen / sizeof(TileElement);
                if (numElements == 0)
                {
                    map_set_tile_element(TileCoordsXY(_coords), nullptr);
                }
                else
                {
                    auto first = GetFirstElement();
                    auto currentNumElements = GetNumElements(first);
                    if (numElements > currentNumElements)
                    {
                        // Allocate space for the extra tile elements (inefficient but works)
                        auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0);
                        auto numToInsert = numElements - currentNumElements;
                        for (size_t i = 0; i < numToInsert; i++)
                        {
                            tile_element_insert(pos, 0);
                        }

                        // Copy data to element span
                        first = map_get_first_element_at(_coords);
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
                map_invalidate_tile_full(_coords);
            }
        }

        std::shared_ptr<ScTileElement> getElement(size_t index)
        {
            auto first = GetFirstElement();
            if (index < GetNumElements(first))
            {
                return std::make_shared<ScTileElement>(_coords, &first[index]);
            }
            return {};
        }

        std::shared_ptr<ScTileElement> insertElement(size_t index)
        {
            auto first = GetFirstElement();
            auto origNumElements = GetNumElements(first);
            if (index >= 0 && index <= origNumElements)
            {
                std::vector<TileElement> data(first, first + origNumElements);

                auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0);
                auto newElement = tile_element_insert(pos, 0);
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
                    map_invalidate_tile_full(_coords);
                    return std::make_shared<ScTileElement>(_coords, &first[index]);
                }
            }
            else
            {
                auto ctx = GetDukContext();
                duk_error(ctx, DUK_ERR_RANGE_ERROR, "Index must be between zero and the number of elements on the tile.");
            }
        }

        void removeElement(size_t index)
        {
            auto first = GetFirstElement();
            if (index < GetNumElements(first))
            {
                tile_element_remove(&first[index]);
                map_invalidate_tile_full(_coords);
            }
        }

        TileElement* GetFirstElement()
        {
            return map_get_first_element_at(_coords);
        }

        static size_t GetNumElements(const TileElement* first)
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

        duk_context* GetDukContext()
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();
            return ctx;
        }

    public:
        static void Register(duk_context* ctx)
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
    };
} // namespace OpenRCT2::Scripting

#endif
