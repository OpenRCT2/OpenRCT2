/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../Context.h"
#    include "../common.h"
#    include "../world/Footpath.h"
#    include "../world/Scenery.h"
#    include "../world/Sprite.h"
#    include "../world/Surface.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <cstdio>
#    include <cstring>

namespace OpenRCT2::Scripting
{
    class ScSurfaceElement;

    class ScTileElement
    {
    protected:
        CoordsXY _coords;
        TileElement* _element;

    public:
        ScTileElement(const CoordsXY& coords, TileElement* element)
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

        uint8_t baseHeight_get() const
        {
            return _element->base_height;
        }
        void baseHeight_set(uint8_t newBaseHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->base_height = newBaseHeight;
        }

        uint8_t clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8_t newClearanceHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->clearance_height = newClearanceHeight;
        }

        uint8_t slope_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetSlope();
            return 0;
        }
        void slope_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetSlope(value);
            }
        }

        int32_t waterHeight_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetWaterHeight();
            return 0;
        }
        void waterHeight_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetWaterHeight(value);
            }
        }

        uint32_t surfaceStyle_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetSurfaceStyle();
            return 0;
        }
        void surfaceStyle_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetSurfaceStyle(value);
            }
        }

        uint32_t edgeStyle_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetEdgeStyle();
            return 0;
        }
        void edgeStyle_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetEdgeStyle(value);
            }
        }

        uint8_t grassLength_get() const
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

        bool hasOwnership_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetOwnership() & OWNERSHIP_OWNED;
            return false;
        }

        bool hasConstructionRights_get()
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                auto ownership = el->GetOwnership();
                return (ownership & OWNERSHIP_OWNED) || (ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED);
            }
            return false;
        }

        uint8_t ownership_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetOwnership();
            return 0;
        }
        void ownership_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetOwnership(value);
            }
        }

        uint8_t parkFences_get() const
        {
            auto el = _element->AsSurface();
            if (el != nullptr)
                return el->GetParkFences();
            return 0;
        }
        void parkFences_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetParkFences(value);
            }
        }

        uint8_t trackType_get() const
        {
            auto el = _element->AsTrack();
            if (el != nullptr)
                return el->GetTrackType();
            return 0;
        }
        void trackType_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetTrackType(value);
            }
        }

        uint8_t sequence_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    return el->GetSequenceIndex();
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    return el->GetSequenceIndex();
                }
            }
            return 0;
        }
        void sequence_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    el->SetSequenceIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetSequenceIndex(value);
                    break;
                }
            }
        }

        uint8_t ride_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    return el->GetRideIndex();
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    return el->GetRideIndex();
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    return el->GetRideIndex();
                }
            }
            return 0;
        }
        void ride_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    el->SetRideIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    el->SetRideIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetRideIndex(value);
                    break;
                }
            }
        }

        uint8_t station_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    return el->GetStationIndex();
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    return el->GetStationIndex();
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    return el->GetStationIndex();
                }
            }
            return 0;
        }
        void station_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    el->SetStationIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    el->SetStationIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetStationIndex(value);
                    break;
                }
            }
        }

        bool hasChainLift_get() const
        {
            auto el = _element->AsTrack();
            if (el != nullptr)
                return el->HasChain();
            return 0;
        }
        void hasChainLift_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetHasChain(value);
            }
        }

        uint32_t object_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    return el->GetSurfaceEntryIndex();
                }
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    return el->GetEntryIndex();
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    return el->GetEntryIndex();
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    return el->GetEntryIndex();
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    return el->GetEntranceType();
                }
            }
            return 0;
        }
        void object_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    el->SetSurfaceEntryIndex(value & 0xFF);
                    break;
                }
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    el->SetEntryIndex(value & 0xFF);
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetEntryIndex(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetEntryIndex(value & 0xFFFF);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetEntranceType(value & 0xFF);
                    break;
                }
            }
        }

        uint8_t age_get() const
        {
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
                return el->GetAge();
            return 0;
        }
        void age_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
            {
                el->SetAge(value);
            }
        }

        uint8_t primaryColour_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    return el->GetPrimaryColour();
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    return el->GetPrimaryColour();
                }
            }
            return 0;
        }
        void primaryColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    el->SetPrimaryColour(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetPrimaryColour(value);
                    break;
                }
            }
        }

        uint8_t secondaryColour_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    return el->GetSecondaryColour();
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    return el->GetSecondaryColour();
                }
            }
            return 0;
        }
        void secondaryColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    el->SetSecondaryColour(value);
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetSecondaryColour(value);
                    break;
                }
            }
        }

        bool railings_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->GetRailingEntryIndex() : false;
        }
        void railings_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetRailingEntryIndex(value);
            }
        }

        uint8_t edgesAndCorners_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->GetEdgesAndCorners() : 0;
        }
        void edgesAndCorners_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetEdgesAndCorners(value);
            }
        }

        DukValue slopeDirection_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->IsSloped())
            {
                auto slope = static_cast<uint8_t>(el->GetSlopeDirection());
                duk_push_int(ctx, slope);
                return DukValue::take_from_stack(ctx);
            }
            duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void slopeDirection_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                if (value.type() == DukValue::Type::NUMBER)
                {
                    el->SetSloped(true);
                    el->SetSlopeDirection(value.as_int());
                }
                else
                {
                    el->SetSloped(false);
                    el->SetSlopeDirection(0);
                }
            }
        }

        bool isQueue_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->IsQueue() : false;
        }
        void isQueue_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsQueue(value);
            }
        }

        DukValue queueBannerDirection_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasQueueBanner())
            {
                duk_push_int(ctx, el->GetQueueBannerDirection());
                return DukValue::take_from_stack(ctx);
            }
            duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void queueBannerDirection_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                if (value.type() == DukValue::Type::NUMBER)
                {
                    el->SetHasQueueBanner(true);
                    el->SetQueueBannerDirection(value.as_int());
                }
                else
                {
                    el->SetHasQueueBanner(false);
                    el->SetQueueBannerDirection(0);
                }
            }
        }

        bool isBlockedByVehicle_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->IsBlockedByVehicle() : false;
        }
        void isBlockedByVehicle_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsBlockedByVehicle(value);
            }
        }

        bool isWide_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->IsWide() : false;
        }
        void isWide_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetWide(value);
            }
        }

        DukValue addition_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                auto addition = el->GetAddition();
                if (addition != 0)
                {
                    duk_push_int(ctx, addition - 1);
                    return DukValue::take_from_stack(ctx);
                }
            }
            duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void addition_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                if (value.type() == DukValue::Type::NUMBER)
                {
                    auto addition = value.as_int();
                    if (addition >= 0 && addition <= 254)
                    {
                        el->SetAddition(addition + 1);
                    }
                }
                else
                {
                    el->SetAddition(0);
                }
            }
        }

        uint8_t additionStatus_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->GetAdditionStatus() : 0;
        }
        void additionStatus_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetAdditionStatus(value);
            }
        }

        bool isAdditionBroken_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->IsBroken() : false;
        }
        void isAdditionBroken_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsBroken(value);
            }
        }

        bool isAdditionGhost_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->AdditionIsGhost() : false;
        }
        void isAdditionGhost_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetAdditionIsGhost(value);
            }
        }

        uint8_t footpathObject_get() const
        {
            auto el = _element->AsEntrance();
            if (el != nullptr)
                return el->GetPathType();
            return 0;
        }
        void footpathObject_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsEntrance();
            if (el != nullptr)
            {
                el->SetPathType(value);
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            // All
            dukglue_register_property(ctx, &ScTileElement::type_get, &ScTileElement::type_set, "type");
            dukglue_register_property(ctx, &ScTileElement::baseHeight_get, &ScTileElement::baseHeight_set, "baseHeight");
            dukglue_register_property(
                ctx, &ScTileElement::clearanceHeight_get, &ScTileElement::clearanceHeight_set, "clearanceHeight");

            // Some
            dukglue_register_property(ctx, &ScTileElement::object_get, &ScTileElement::object_set, "object");

            // Small Scenery | Large Scenery
            dukglue_register_property(
                ctx, &ScTileElement::primaryColour_get, &ScTileElement::primaryColour_set, "primaryColour");
            dukglue_register_property(
                ctx, &ScTileElement::secondaryColour_get, &ScTileElement::secondaryColour_set, "secondaryColour");

            // Path | track | entrance
            dukglue_register_property(ctx, &ScTileElement::ride_get, &ScTileElement::ride_set, "ride");
            dukglue_register_property(ctx, &ScTileElement::station_get, &ScTileElement::station_set, "station");

            // Track | entrance
            dukglue_register_property(ctx, &ScTileElement::sequence_get, &ScTileElement::sequence_set, "sequence");

            // Surface only
            dukglue_register_property(ctx, &ScTileElement::slope_get, &ScTileElement::slope_set, "slope");
            dukglue_register_property(ctx, &ScTileElement::waterHeight_get, &ScTileElement::waterHeight_set, "waterHeight");
            dukglue_register_property(ctx, &ScTileElement::surfaceStyle_get, &ScTileElement::surfaceStyle_set, "surfaceStyle");
            dukglue_register_property(ctx, &ScTileElement::edgeStyle_get, &ScTileElement::edgeStyle_set, "edgeStyle");
            dukglue_register_property(ctx, &ScTileElement::grassLength_get, &ScTileElement::grassLength_set, "grassLength");
            dukglue_register_property(ctx, &ScTileElement::hasOwnership_get, nullptr, "hasOwnership");
            dukglue_register_property(ctx, &ScTileElement::hasConstructionRights_get, nullptr, "hasConstructionRights");
            dukglue_register_property(ctx, &ScTileElement::ownership_get, &ScTileElement::ownership_set, "ownership");
            dukglue_register_property(ctx, &ScTileElement::parkFences_get, &ScTileElement::parkFences_set, "parkFences");

            // Track only
            dukglue_register_property(ctx, &ScTileElement::trackType_get, &ScTileElement::trackType_set, "trackType");
            dukglue_register_property(ctx, &ScTileElement::hasChainLift_get, &ScTileElement::hasChainLift_set, "isChainLift");

            // Small Scenery only
            dukglue_register_property(ctx, &ScTileElement::age_get, &ScTileElement::age_set, "age");

            // Footpath only
            dukglue_register_property(ctx, &ScTileElement::railings_get, &ScTileElement::railings_set, "railings");
            dukglue_register_property(
                ctx, &ScTileElement::edgesAndCorners_get, &ScTileElement::edgesAndCorners_set, "edgesAndCorners");
            dukglue_register_property(
                ctx, &ScTileElement::slopeDirection_get, &ScTileElement::slopeDirection_set, "slopeDirection");
            dukglue_register_property(ctx, &ScTileElement::isQueue_get, &ScTileElement::isQueue_set, "isQueue");
            dukglue_register_property(
                ctx, &ScTileElement::queueBannerDirection_get, &ScTileElement::queueBannerDirection_set,
                "queueBannerDirection");

            dukglue_register_property(
                ctx, &ScTileElement::isBlockedByVehicle_get, &ScTileElement::isBlockedByVehicle_set, "isBlockedByVehicle");
            dukglue_register_property(ctx, &ScTileElement::isWide_get, &ScTileElement::isWide_set, "isWide");

            dukglue_register_property(ctx, &ScTileElement::addition_get, &ScTileElement::addition_set, "addition");
            dukglue_register_property(
                ctx, &ScTileElement::additionStatus_get, &ScTileElement::additionStatus_set, "additionStatus");
            dukglue_register_property(
                ctx, &ScTileElement::isAdditionBroken_get, &ScTileElement::isAdditionBroken_set, "isAdditionBroken");
            dukglue_register_property(
                ctx, &ScTileElement::isAdditionGhost_get, &ScTileElement::isAdditionGhost_set, "isAdditionGhost");

            // Entrance only
            dukglue_register_property(
                ctx, &ScTileElement::footpathObject_get, &ScTileElement::footpathObject_set, "footpathObject");
        }
    };

    class ScTile
    {
    private:
        CoordsXY _coords;

    public:
        ScTile(const CoordsXY& coords)
            : _coords(coords)
        {
        }

    private:
        int32_t x_get() const
        {
            return _coords.x / COORDS_XY_STEP;
        }

        int32_t y_get() const
        {
            return _coords.y / COORDS_XY_STEP;
        }

        size_t numElements_get() const
        {
            auto first = GetFirstElement();
            return GetNumElements(first);
        }

        std::vector<std::shared_ptr<ScTileElement>> elements_get() const
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

        DukValue data_get() const
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
                    map_set_tile_element(TileCoordsXY(_coords), nullptr);
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

        std::shared_ptr<ScTileElement> getElement(size_t index) const
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
            ThrowIfGameStateNotMutable();
            std::shared_ptr<ScTileElement> result;
            auto first = GetFirstElement();
            auto origNumElements = GetNumElements(first);
            if (index <= origNumElements)
            {
                std::vector<TileElement> data(first, first + origNumElements);

                auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0).ToCoordsXYZ();
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

        void removeElement(size_t index)
        {
            ThrowIfGameStateNotMutable();
            auto first = GetFirstElement();
            if (index < GetNumElements(first))
            {
                tile_element_remove(&first[index]);
                map_invalidate_tile_full(_coords);
            }
        }

        TileElement* GetFirstElement() const
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

        duk_context* GetDukContext() const
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
