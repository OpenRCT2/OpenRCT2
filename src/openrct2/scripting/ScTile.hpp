#pragma once

#include <dukglue/dukglue.h>
#include "../common.h"
#include "../world/Sprite.h"
#include <cstdio>

namespace OpenRCT2::Scripting
{
    class ScTileElement
    {
    protected:
        rct_tile_element * _element;

    public:
        ScTileElement(rct_tile_element * element)
            : _element(element)
        {
        }

        std::string type_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SURFACE: return "surface";
                case TILE_ELEMENT_TYPE_PATH: return "footpath";
                case TILE_ELEMENT_TYPE_TRACK: return "track";
                case TILE_ELEMENT_TYPE_SMALL_SCENERY: return "small-scenery";
                case TILE_ELEMENT_TYPE_ENTRANCE: return "entrance";
                case TILE_ELEMENT_TYPE_WALL: return "wall";
                case TILE_ELEMENT_TYPE_LARGE_SCENERY: return "large-scenery";
                case TILE_ELEMENT_TYPE_BANNER: return "banner";
                case TILE_ELEMENT_TYPE_CORRUPT: return "openrct2-corrupt";
                default: return "unknown";
            }
        }

        template<typename T>
        T* getAs()
        {
            return static_cast<T*>(this);
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
        void baseHeight_set(uint8 newBaseHeight)
        {
            _element->base_height = newBaseHeight;
        }

        auto clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8 newClearanceHeight)
        {
            _element->clearance_height = newClearanceHeight;
        }

        static void Register(duk_context* ctx);
        template<typename T>
        static void RegisterSharedProperties(duk_context* ctx);
    };

    class ScSurfaceElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScSurfaceElement>(ctx);
        }
    };

    class ScFootpathElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScFootpathElement>(ctx);
        }
    };

    class ScTrackElement final : public ScTileElement
    {
        auto track_ride_index_get() const
        {
            return _element->properties.track.ride_index;
        }

    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScTrackElement>(ctx);
            dukglue_register_property(ctx, &ScTrackElement::track_ride_index_get, nullptr, "rideIndex");
        }
    };

    class ScSmallSceneryElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScSmallSceneryElement>(ctx);
        }
    };

    class ScEntranceElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScEntranceElement>(ctx);
        }
    };

    class ScWallElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScWallElement>(ctx);
        }
    };

    class ScLargeSceneryElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScLargeSceneryElement>(ctx);
        }
    };

    class ScBannerElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScBannerElement>(ctx);
        }
    };

    void ScTileElement::Register(duk_context* ctx)
    {
        dukglue_register_method(ctx, &ScTileElement::getAs<ScSurfaceElement>, "asSurface");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScTrackElement>, "asTrack");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScSmallSceneryElement>, "asSmallScenery");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScEntranceElement>, "asEntrance");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScWallElement>, "asWall");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScLargeSceneryElement>, "asLargeScenery");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScBannerElement>, "asBanner");

        // Shared properties
        RegisterSharedProperties<ScTileElement>(ctx);

        // Register type specific properties
        ScSurfaceElement ::Register(ctx);
        ScTrackElement ::Register(ctx);
        ScSmallSceneryElement ::Register(ctx);
        ScEntranceElement ::Register(ctx);
        ScWallElement ::Register(ctx);
        ScLargeSceneryElement ::Register(ctx);
        ScBannerElement ::Register(ctx);
    }

    template<typename T>
    void ScTileElement::RegisterSharedProperties(duk_context* ctx)
    {
        // When you register a member function of a parent class, it belongs to the parent and cannot be accessed by its
        // children. To get around this issue, we need to force the function pointer types to be of the class we want them to be
        // available for. This needs to be done for every child.
        dukglue_register_property(ctx, static_cast<std::string (T::*)() const>(&ScTileElement::type_get), nullptr, "type");
        dukglue_register_property(
            ctx,
            static_cast<bool (T::*)() const>(&ScTileElement::broken_get),
            static_cast<void (T::*)(bool)>(&ScTileElement::broken_set),
            "broken");
        dukglue_register_property(
            ctx,
            static_cast<uint8 (T::*)() const>(&ScTileElement::baseHeight_get),
            static_cast<void (T::*)(uint8)>(&ScTileElement::baseHeight_set),
            "baseHeight");
        dukglue_register_property(
            ctx,
            static_cast<uint8 (T::*)() const>(&ScTileElement::clearanceHeight_get),
            static_cast<void (T::*)(uint8)>(&ScTileElement::clearanceHeight_set),
            "clearanceHeight");
    }

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
                while (!(element++)->IsLastForTile());
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
