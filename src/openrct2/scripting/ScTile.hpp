/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#    include "../core/Guard.hpp"
#    include "../ride/Track.h"
#    include "../world/Footpath.h"
#    include "../world/Scenery.h"
#    include "../world/Sprite.h"
#    include "../world/Surface.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <cstdio>
#    include <cstring>
#    include <utility>

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
                    return "small_scenery";
                case TILE_ELEMENT_TYPE_ENTRANCE:
                    return "entrance";
                case TILE_ELEMENT_TYPE_WALL:
                    return "wall";
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    return "large_scenery";
                case TILE_ELEMENT_TYPE_BANNER:
                    return "banner";
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
            else if (value == "small_scenery")
                type = TILE_ELEMENT_TYPE_SMALL_SCENERY;
            else if (value == "entrance")
                type = TILE_ELEMENT_TYPE_ENTRANCE;
            else if (value == "wall")
                type = TILE_ELEMENT_TYPE_WALL;
            else if (value == "large_scenery")
                type = TILE_ELEMENT_TYPE_LARGE_SCENERY;
            else if (value == "banner")
                type = TILE_ELEMENT_TYPE_BANNER;
            else
            {
                std::puts("Element type not recognised!");
                return;
            }

            _element->type = type;
            Invalidate();
        }

        uint8_t baseHeight_get() const
        {
            return _element->base_height;
        }
        void baseHeight_set(uint8_t newBaseHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->base_height = newBaseHeight;
            Invalidate();
        }

        uint16_t baseZ_get() const
        {
            return _element->GetBaseZ();
        }
        void baseZ_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            _element->SetBaseZ(value);
            Invalidate();
        }

        uint8_t clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8_t newClearanceHeight)
        {
            ThrowIfGameStateNotMutable();
            _element->clearance_height = newClearanceHeight;
            Invalidate();
        }

        uint16_t clearanceZ_get() const
        {
            return _element->GetClearanceZ();
        }
        void clearanceZ_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            _element->SetClearanceZ(value);
            Invalidate();
        }

        DukValue slope_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SURFACE:
                {
                    auto el = _element->AsSurface();
                    duk_push_int(ctx, el->GetSlope());
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    duk_push_int(ctx, el->GetSlope());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
        }
        void slope_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SURFACE:
                {
                    auto el = _element->AsSurface();
                    el->SetSlope(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetSlope(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue waterHeight_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetWaterHeight());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void waterHeight_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetWaterHeight(value);
                Invalidate();
            }
        }

        DukValue surfaceStyle_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetSurfaceStyle());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void surfaceStyle_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetSurfaceStyle(value);
                Invalidate();
            }
        }

        DukValue edgeStyle_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetEdgeStyle());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void edgeStyle_set(uint32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetEdgeStyle(value);
                Invalidate();
            }
        }

        DukValue grassLength_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetGrassLength());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void grassLength_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
                el->SetGrassLengthAndInvalidate(value, _coords);
                Invalidate();
            }
        }

        DukValue hasOwnership_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_boolean(ctx, el->GetOwnership() & OWNERSHIP_OWNED);
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }

        DukValue hasConstructionRights_get()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                auto ownership = el->GetOwnership();
                duk_push_boolean(ctx, (ownership & OWNERSHIP_OWNED) || (ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED));
            }
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }

        DukValue ownership_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetOwnership());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void ownership_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetOwnership(value);
                Invalidate();
            }
        }

        DukValue parkFences_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSurface();
            if (el != nullptr)
                duk_push_int(ctx, el->GetParkFences());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void parkFences_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSurface();
            if (el != nullptr)
            {
                el->SetParkFences(value);
                Invalidate();
            }
        }

        DukValue trackType_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr)
                duk_push_int(ctx, el->GetTrackType());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void trackType_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetTrackType(value);
                Invalidate();
            }
        }

        DukValue sequence_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    duk_push_int(ctx, el->GetSequenceIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    if (get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                        duk_push_int(ctx, el->GetSequenceIndex());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetSequenceIndex());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
        }
        void sequence_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetSequenceIndex(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    if (get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                    {
                        el->SetSequenceIndex(value);
                        Invalidate();
                    }
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetSequenceIndex(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue ride_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    if (el->IsQueue() && el->GetRideIndex() != RIDE_ID_NULL)
                        duk_push_int(ctx, el->GetRideIndex());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    duk_push_int(ctx, el->GetRideIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetRideIndex());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
        }
        void ride_set(ride_id_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    if (!el->HasAddition())
                    {
                        el->SetRideIndex(value);
                        Invalidate();
                    }
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    el->SetRideIndex(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetRideIndex(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue station_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    if (el->IsQueue() && el->GetRideIndex() != RIDE_ID_NULL)
                        duk_push_int(ctx, el->GetStationIndex());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    if (el->IsStation())
                        duk_push_int(ctx, el->GetStationIndex());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetStationIndex());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
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
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_TRACK:
                {
                    auto el = _element->AsTrack();
                    el->SetStationIndex(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetStationIndex(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue hasChainLift_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr)
                duk_push_boolean(ctx, el->HasChain());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void hasChainLift_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetHasChain(value);
                Invalidate();
            }
        }

        DukValue mazeEntry_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr && get_ride(el->GetRideIndex())->type == RIDE_TYPE_MAZE)
                duk_push_int(ctx, el->GetMazeEntry());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void mazeEntry_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
                if (get_ride(el->GetRideIndex())->type == RIDE_TYPE_MAZE)
                {
                    el->SetMazeEntry(value);
                    Invalidate();
                }
        }

        DukValue colourScheme_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr && get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                duk_push_int(ctx, el->GetColourScheme());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void colourScheme_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
                if (get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                {
                    el->SetColourScheme(value);
                    Invalidate();
                }
        }

        DukValue seatRotation_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr && get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                duk_push_int(ctx, el->GetSeatRotation());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void seatRotation_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
                if (get_ride(el->GetRideIndex())->type != RIDE_TYPE_MAZE)
                {
                    el->SetSeatRotation(value);
                    Invalidate();
                }
        }

        DukValue brakeBoosterSpeed_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr && TrackTypeHasSpeedSetting(el->GetTrackType()))
                duk_push_int(ctx, el->GetBrakeBoosterSpeed());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void brakeBoosterSpeed_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
                if (TrackTypeHasSpeedSetting(el->GetTrackType()))
                {
                    el->SetBrakeBoosterSpeed(value);
                    Invalidate();
                }
        }

        DukValue isInverted_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr)
                duk_push_boolean(ctx, el->IsInverted());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isInverted_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetInverted(value);
                Invalidate();
            }
        }

        DukValue hasCableLift_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsTrack();
            if (el != nullptr)
                duk_push_boolean(ctx, el->HasCableLift());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void hasCableLift_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsTrack();
            if (el != nullptr)
            {
                el->SetHasCableLift(value);
                Invalidate();
            }
        }

        DukValue object_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    duk_push_int(ctx, el->GetPathEntryIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    duk_push_int(ctx, el->GetEntryIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    duk_push_int(ctx, el->GetEntryIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    duk_push_int(ctx, el->GetEntryIndex());
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetEntranceType());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
        }
        void object_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();

            auto index = FromDuk<ObjectEntryIndex>(value);
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_PATH:
                {
                    auto el = _element->AsPath();
                    el->SetPathEntryIndex(index);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    el->SetEntryIndex(index);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetEntryIndex(index);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetEntryIndex(index);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_ENTRANCE:
                {
                    auto el = _element->AsEntrance();
                    el->SetEntranceType(index);
                    Invalidate();
                    break;
                }
            }
        }

        bool isHidden_get() const
        {
            return _element->IsInvisible();
        }
        void isHidden_set(bool hide)
        {
            ThrowIfGameStateNotMutable();
            _element->SetInvisible(hide);
            Invalidate();
        }

        DukValue age_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
                duk_push_int(ctx, el->GetAge());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void age_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
            {
                el->SetAge(value);
                Invalidate();
            }
        }

        DukValue quadrant_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
                duk_push_int(ctx, el->GetSceneryQuadrant());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void quadrant_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsSmallScenery();
            if (el != nullptr)
            {
                el->SetSceneryQuadrant(value);
                Invalidate();
            }
        }

        uint8_t occupiedQuadrants_get() const
        {
            return _element->GetOccupiedQuadrants();
        }
        void occupiedQuadrants_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            _element->SetOccupiedQuadrants(value);
            Invalidate();
        }

        bool isGhost_get() const
        {
            return _element->IsGhost();
        }
        void isGhost_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            _element->SetGhost(value);
            Invalidate();
        }

        DukValue primaryColour_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    duk_push_int(ctx, el->GetPrimaryColour());
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    duk_push_int(ctx, el->GetPrimaryColour());
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    duk_push_int(ctx, el->GetPrimaryColour());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
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
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetPrimaryColour(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetPrimaryColour(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue secondaryColour_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                {
                    auto el = _element->AsSmallScenery();
                    duk_push_int(ctx, el->GetSecondaryColour());
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    duk_push_int(ctx, el->GetSecondaryColour());
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    duk_push_int(ctx, el->GetSecondaryColour());
                    break;
                }
                default:
                {
                    duk_push_null(ctx);
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
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
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetSecondaryColour(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetSecondaryColour(value);
                    Invalidate();
                    break;
                }
            }
        }

        DukValue tertiaryColour_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsWall();
            if (el != nullptr)
                duk_push_int(ctx, el->GetTertiaryColour());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void tertiaryColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsWall();
            if (el != nullptr)
            {
                el->SetTertiaryColour(value);
                Invalidate();
            }
        }

        DukValue bannerIndex_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            BannerIndex idx = _element->GetBannerIndex();
            if (idx == BANNER_INDEX_NULL)
                duk_push_null(ctx);
            else
                duk_push_int(ctx, idx);
            return DukValue::take_from_stack(ctx);
        }
        void bannerIndex_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                {
                    auto el = _element->AsLargeScenery();
                    el->SetBannerIndex(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_WALL:
                {
                    auto el = _element->AsWall();
                    el->SetBannerIndex(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_BANNER:
                {
                    auto el = _element->AsBanner();
                    el->SetIndex(value);
                    Invalidate();
                    break;
                }
            }
        }

        // Deprecated in favor of seperate 'edges' and 'corners' properties,
        // left here to maintain compatibility with older plugins.
        /** @deprecated */
        uint8_t edgesAndCorners_get() const
        {
            auto el = _element->AsPath();
            return el != nullptr ? el->GetEdgesAndCorners() : 0;
        }
        /** @deprecated */
        void edgesAndCorners_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetEdgesAndCorners(value);
                Invalidate();
            }
        }

        DukValue edges_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
                duk_push_int(ctx, el->GetEdges());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void edges_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetEdges(value);
                Invalidate();
            }
        }

        DukValue corners_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
                duk_push_int(ctx, el->GetCorners());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void corners_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetCorners(value);
                Invalidate();
            }
        }

        DukValue slopeDirection_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->IsSloped())
                duk_push_int(ctx, el->GetSlopeDirection());
            else
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
                Invalidate();
            }
        }

        DukValue isQueue_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
                duk_push_boolean(ctx, el->IsQueue());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isQueue_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsQueue(value);
                Invalidate();
            }
        }

        DukValue queueBannerDirection_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasQueueBanner())
                duk_push_int(ctx, el->GetQueueBannerDirection());
            else
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
                Invalidate();
            }
        }

        DukValue isBlockedByVehicle_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
                duk_push_boolean(ctx, el->IsBlockedByVehicle());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isBlockedByVehicle_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsBlockedByVehicle(value);
                Invalidate();
            }
        }

        DukValue isWide_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr)
                duk_push_boolean(ctx, el->IsWide());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isWide_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetWide(value);
                Invalidate();
            }
        }

        DukValue surfaceObject_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                auto el = _element->AsPath();
                auto index = el->GetSurfaceEntryIndex();
                if (index != OBJECT_ENTRY_INDEX_NULL)
                {
                    duk_push_int(ctx, index);
                }
                else
                {
                    duk_push_null(ctx);
                }
            }
            else
            {
                duk_push_null(ctx);
            }
            return DukValue::take_from_stack(ctx);
        }

        void surfaceObject_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            if (_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                auto el = _element->AsPath();
                el->SetSurfaceEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }

        DukValue railingsObject_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                auto el = _element->AsPath();
                auto index = el->GetRailingEntryIndex();
                if (index != OBJECT_ENTRY_INDEX_NULL)
                {
                    duk_push_int(ctx, index);
                }
                else
                {
                    duk_push_null(ctx);
                }
            }
            else
            {
                duk_push_null(ctx);
            }
            return DukValue::take_from_stack(ctx);
        }

        void railingsObject_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            if (_element->GetType() == TILE_ELEMENT_TYPE_PATH)
            {
                auto el = _element->AsPath();
                el->SetRailingEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }

        DukValue addition_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasAddition())
                duk_push_int(ctx, el->GetAddition() - 1);
            else
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
                Invalidate();
            }
        }

        DukValue additionStatus_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasAddition())
                duk_push_int(ctx, el->GetAdditionStatus());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void additionStatus_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
                if (el->HasAddition())
                {
                    el->SetAdditionStatus(value);
                    Invalidate();
                }
        }

        DukValue isAdditionBroken_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasAddition())
                duk_push_boolean(ctx, el->IsBroken());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isAdditionBroken_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsBroken(value);
                Invalidate();
            }
        }

        DukValue isAdditionGhost_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsPath();
            if (el != nullptr && el->HasAddition())
                duk_push_boolean(ctx, el->AdditionIsGhost());
            else
                duk_push_null(ctx);
            return DukValue::take_from_stack(ctx);
        }
        void isAdditionGhost_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetAdditionIsGhost(value);
                Invalidate();
            }
        }

        DukValue footpathObject_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsEntrance();
            if (el != nullptr)
            {
                auto index = el->GetPathEntryIndex();
                if (index != OBJECT_ENTRY_INDEX_NULL)
                {
                    duk_push_int(ctx, index);
                }
                else
                {
                    duk_push_null(ctx);
                }
            }
            else
            {
                duk_push_null(ctx);
            }
            return DukValue::take_from_stack(ctx);
        }
        void footpathObject_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsEntrance();
            if (el != nullptr)
            {
                el->SetPathEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }

        DukValue footpathSurfaceObject_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto el = _element->AsEntrance();
            if (el != nullptr)
            {
                auto index = el->GetSurfaceEntryIndex();
                if (index != OBJECT_ENTRY_INDEX_NULL)
                {
                    duk_push_int(ctx, index);
                }
                else
                {
                    duk_push_null(ctx);
                }
            }
            else
            {
                duk_push_null(ctx);
            }
            return DukValue::take_from_stack(ctx);
        }
        void footpathSurfaceObject_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto el = _element->AsEntrance();
            if (el != nullptr)
            {
                el->SetSurfaceEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }

        DukValue direction_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_BANNER:
                {
                    auto el = _element->AsBanner();
                    duk_push_int(ctx, el->GetPosition());
                    break;
                }
                case TILE_ELEMENT_TYPE_PATH:
                case TILE_ELEMENT_TYPE_SURFACE:
                {
                    duk_push_null(ctx);
                    break;
                }
                default:
                {
                    duk_push_int(ctx, _element->GetDirection());
                    break;
                }
            }
            return DukValue::take_from_stack(ctx);
        }
        void direction_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_BANNER:
                {
                    auto el = _element->AsBanner();
                    el->SetPosition(value);
                    Invalidate();
                    break;
                }
                case TILE_ELEMENT_TYPE_PATH:
                case TILE_ELEMENT_TYPE_SURFACE:
                {
                    break;
                }
                default:
                {
                    _element->SetDirection(value);
                    Invalidate();
                }
            }
        }

        void Invalidate()
        {
            map_invalidate_tile_full(_coords);
        }

    public:
        static void Register(duk_context* ctx)
        {
            // All
            dukglue_register_property(ctx, &ScTileElement::type_get, &ScTileElement::type_set, "type");
            dukglue_register_property(ctx, &ScTileElement::baseHeight_get, &ScTileElement::baseHeight_set, "baseHeight");
            dukglue_register_property(ctx, &ScTileElement::baseZ_get, &ScTileElement::baseZ_set, "baseZ");
            dukglue_register_property(
                ctx, &ScTileElement::clearanceHeight_get, &ScTileElement::clearanceHeight_set, "clearanceHeight");
            dukglue_register_property(ctx, &ScTileElement::clearanceZ_get, &ScTileElement::clearanceZ_set, "clearanceZ");
            dukglue_register_property(
                ctx, &ScTileElement::occupiedQuadrants_get, &ScTileElement::occupiedQuadrants_set, "occupiedQuadrants");
            dukglue_register_property(ctx, &ScTileElement::isGhost_get, &ScTileElement::isGhost_set, "isGhost");
            dukglue_register_property(ctx, &ScTileElement::isHidden_get, &ScTileElement::isHidden_set, "isHidden");

            // Track | Small Scenery | Wall | Entrance | Large Scenery | Banner
            dukglue_register_property(ctx, &ScTileElement::direction_get, &ScTileElement::direction_set, "direction");

            // Path | Small Scenery | Wall | Entrance | Large Scenery
            dukglue_register_property(ctx, &ScTileElement::object_get, &ScTileElement::object_set, "object");

            // Small Scenery | Wall | Large Scenery
            dukglue_register_property(
                ctx, &ScTileElement::primaryColour_get, &ScTileElement::primaryColour_set, "primaryColour");
            dukglue_register_property(
                ctx, &ScTileElement::secondaryColour_get, &ScTileElement::secondaryColour_set, "secondaryColour");

            // Wall | Large Scenery | Banner
            dukglue_register_property(ctx, &ScTileElement::bannerIndex_get, &ScTileElement::bannerIndex_set, "bannerIndex");

            // Path | Track | Entrance
            dukglue_register_property(ctx, &ScTileElement::ride_get, &ScTileElement::ride_set, "ride");
            dukglue_register_property(ctx, &ScTileElement::station_get, &ScTileElement::station_set, "station");

            // Track | Entrance | Large Scenery
            dukglue_register_property(ctx, &ScTileElement::sequence_get, &ScTileElement::sequence_set, "sequence");

            // Surface | Wall
            dukglue_register_property(ctx, &ScTileElement::slope_get, &ScTileElement::slope_set, "slope");

            // Surface only
            dukglue_register_property(ctx, &ScTileElement::waterHeight_get, &ScTileElement::waterHeight_set, "waterHeight");
            dukglue_register_property(ctx, &ScTileElement::surfaceStyle_get, &ScTileElement::surfaceStyle_set, "surfaceStyle");
            dukglue_register_property(ctx, &ScTileElement::edgeStyle_get, &ScTileElement::edgeStyle_set, "edgeStyle");
            dukglue_register_property(ctx, &ScTileElement::grassLength_get, &ScTileElement::grassLength_set, "grassLength");
            dukglue_register_property(ctx, &ScTileElement::hasOwnership_get, nullptr, "hasOwnership");
            dukglue_register_property(ctx, &ScTileElement::hasConstructionRights_get, nullptr, "hasConstructionRights");
            dukglue_register_property(ctx, &ScTileElement::ownership_get, &ScTileElement::ownership_set, "ownership");
            dukglue_register_property(ctx, &ScTileElement::parkFences_get, &ScTileElement::parkFences_set, "parkFences");

            // Footpath only
            dukglue_register_property(
                ctx, &ScTileElement::edgesAndCorners_get, &ScTileElement::edgesAndCorners_set, "edgesAndCorners");
            dukglue_register_property(ctx, &ScTileElement::edges_get, &ScTileElement::edges_set, "edges");
            dukglue_register_property(ctx, &ScTileElement::corners_get, &ScTileElement::corners_set, "corners");
            dukglue_register_property(
                ctx, &ScTileElement::slopeDirection_get, &ScTileElement::slopeDirection_set, "slopeDirection");
            dukglue_register_property(ctx, &ScTileElement::isQueue_get, &ScTileElement::isQueue_set, "isQueue");
            dukglue_register_property(
                ctx, &ScTileElement::queueBannerDirection_get, &ScTileElement::queueBannerDirection_set,
                "queueBannerDirection");

            dukglue_register_property(
                ctx, &ScTileElement::isBlockedByVehicle_get, &ScTileElement::isBlockedByVehicle_set, "isBlockedByVehicle");
            dukglue_register_property(ctx, &ScTileElement::isWide_get, &ScTileElement::isWide_set, "isWide");

            dukglue_register_property(
                ctx, &ScTileElement::surfaceObject_get, &ScTileElement::surfaceObject_set, "surfaceObject");
            dukglue_register_property(
                ctx, &ScTileElement::railingsObject_get, &ScTileElement::railingsObject_set, "railingsObject");

            dukglue_register_property(ctx, &ScTileElement::addition_get, &ScTileElement::addition_set, "addition");
            dukglue_register_property(
                ctx, &ScTileElement::additionStatus_get, &ScTileElement::additionStatus_set, "additionStatus");
            dukglue_register_property(
                ctx, &ScTileElement::isAdditionBroken_get, &ScTileElement::isAdditionBroken_set, "isAdditionBroken");
            dukglue_register_property(
                ctx, &ScTileElement::isAdditionGhost_get, &ScTileElement::isAdditionGhost_set, "isAdditionGhost");

            // Track only
            dukglue_register_property(ctx, &ScTileElement::trackType_get, &ScTileElement::trackType_set, "trackType");
            dukglue_register_property(ctx, &ScTileElement::mazeEntry_get, &ScTileElement::mazeEntry_set, "mazeEntry");
            dukglue_register_property(ctx, &ScTileElement::colourScheme_get, &ScTileElement::colourScheme_set, "colourScheme");
            dukglue_register_property(ctx, &ScTileElement::seatRotation_get, &ScTileElement::seatRotation_set, "seatRotation");
            dukglue_register_property(
                ctx, &ScTileElement::brakeBoosterSpeed_get, &ScTileElement::brakeBoosterSpeed_set, "brakeBoosterSpeed");
            dukglue_register_property(ctx, &ScTileElement::hasChainLift_get, &ScTileElement::hasChainLift_set, "hasChainLift");
            dukglue_register_property(ctx, &ScTileElement::isInverted_get, &ScTileElement::isInverted_set, "isInverted");
            dukglue_register_property(ctx, &ScTileElement::hasCableLift_get, &ScTileElement::hasCableLift_set, "hasCableLift");

            // Small Scenery only
            dukglue_register_property(ctx, &ScTileElement::age_get, &ScTileElement::age_set, "age");
            dukglue_register_property(ctx, &ScTileElement::quadrant_get, &ScTileElement::quadrant_set, "quadrant");

            // Wall only
            dukglue_register_property(
                ctx, &ScTileElement::tertiaryColour_get, &ScTileElement::tertiaryColour_set, "tertiaryColour");

            // Entrance only
            dukglue_register_property(
                ctx, &ScTileElement::footpathObject_get, &ScTileElement::footpathObject_set, "footpathObject");
            dukglue_register_property(
                ctx, &ScTileElement::footpathSurfaceObject_get, &ScTileElement::footpathSurfaceObject_set,
                "footpathSurfaceObject");
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

        uint32_t numElements_get() const
        {
            auto first = GetFirstElement();
            return static_cast<int32_t>(GetNumElements(first));
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
                            tile_element_insert(pos, 0, TileElementType::Surface);
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

        std::shared_ptr<ScTileElement> getElement(uint32_t index) const
        {
            auto first = GetFirstElement();
            if (static_cast<size_t>(index) < GetNumElements(first))
            {
                return std::make_shared<ScTileElement>(_coords, &first[index]);
            }
            return {};
        }

        std::shared_ptr<ScTileElement> insertElement(uint32_t index)
        {
            ThrowIfGameStateNotMutable();
            std::shared_ptr<ScTileElement> result;
            auto first = GetFirstElement();
            auto origNumElements = GetNumElements(first);
            if (index <= origNumElements)
            {
                std::vector<TileElement> data(first, first + origNumElements);

                auto pos = TileCoordsXYZ(TileCoordsXY(_coords), 0).ToCoordsXYZ();
                auto newElement = tile_element_insert(pos, 0, TileElementType::Surface);
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

        void removeElement(uint32_t index)
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
