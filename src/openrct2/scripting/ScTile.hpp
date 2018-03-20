#pragma once

#include <dukglue/dukglue.h>
#include "../common.h"
#include "../world/Sprite.h"
#include <cstdio>

namespace OpenRCT2::Scripting
{
    class ScTileElement
    {
    private:
        rct_tile_element * _element;

    public:
        ScTileElement(rct_tile_element * element)
            : _element(element)
        {
        }

        std::string type_get()
        {
            if (tile_element_get_type(_element) == TILE_ELEMENT_TYPE_PATH)
            {
                return "footpath";
            }
            return "unknown";
        }

        bool broken_get() { return _element->flags & TILE_ELEMENT_FLAG_BROKEN; }
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

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScTileElement::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScTileElement::broken_get, &ScTileElement::broken_set, "broken");
        }
    };

    class ScTile
    {
    private:
        rct_tile_element * _first;
        size_t _count = 0;

    public:
        ScTile(rct_tile_element * first)
            : _first(first)
        {
            _count = 0;
            if (first != nullptr)
            {
                auto element = first;
                do
                {
                    _count++;
                }
                while (!tile_element_is_last_for_tile(element++));
            }
        }

        size_t numElements_get() { return _count; }

        std::shared_ptr<ScTileElement> getElement(size_t index)
        {
            if (index >= _count) return nullptr;
            return std::make_shared<ScTileElement>(&_first[index]);
        }

        // This is not a good idea as the array is generated each time elements is accessed and
        // chances are scripts will do tile.elements[0], tile.elements[1]...
        // std::vector<std::shared_ptr<ScTileElement>> elements_get()
        // {
        //     std::vector<std::shared_ptr<ScTileElement>> elements;
        //     for (size_t i = 0; i < _count; i++)
        //     {
        //         elements.push_back(std::make_shared<ScTileElement>(&_first[i]));
        //     }
        //     return elements;
        // }

        static void Register(duk_context * ctx)
        {
            // dukglue_register_property(ctx, &ScTile::elements_get, nullptr, "elements");
            dukglue_register_property(ctx, &ScTile::numElements_get, nullptr, "numElements");
            dukglue_register_method(ctx, &ScTile::getElement, "getElement");
        }
    };
}
