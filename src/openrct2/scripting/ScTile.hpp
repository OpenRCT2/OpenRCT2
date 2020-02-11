/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"
#include "Duktape.hpp"

#include <cstdio>

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

        bool broken_get() const
        {
            return _element->flags & TILE_ELEMENT_FLAG_BROKEN;
        }
        void broken_set(bool value)
        {
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
            _element->base_height = newBaseHeight;
        }

        auto clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8_t newClearanceHeight)
        {
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
            dukglue_register_property(ctx, &ScTileElement::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScTileElement::broken_get, &ScTileElement::broken_set, "broken");
            dukglue_register_property(ctx, &ScTileElement::baseHeight_get, &ScTileElement::baseHeight_set, "baseHeight");
            dukglue_register_property(
                ctx, &ScTileElement::clearanceHeight_get, &ScTileElement::clearanceHeight_set, "clearanceHeight");
            dukglue_register_property(ctx, &ScTileElement::grassLength_get, &ScTileElement::grassLength_set, "grassLength");
            dukglue_register_property(ctx, &ScTileElement::hasOwnership_get, nullptr, "hasOwnership");
        }
    };

    class ScTile
    {
    private:
        CoordsXY _coords;
        TileElement* _first;
        size_t _count = 0;

    public:
        ScTile(CoordsXY coords, TileElement* first)
            : _coords(coords)
            , _first(first)
        {
            _count = 0;
            if (first != nullptr)
            {
                auto element = first;
                do
                {
                    _count++;
                } while (!(element++)->IsLastForTile());
            }
        }

        size_t elements_get()
        {
            return _count;
        }

        std::shared_ptr<ScTileElement> getElement(size_t index)
        {
            if (index >= _count)
                return nullptr;
            return std::make_shared<ScTileElement>(_coords, &_first[index]);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTile::elements_get, nullptr, "elements");
            dukglue_register_method(ctx, &ScTile::getElement, "getElement");
        }
    };
} // namespace OpenRCT2::Scripting
