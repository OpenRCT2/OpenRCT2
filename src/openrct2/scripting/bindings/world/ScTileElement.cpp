/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScTileElement.hpp"

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../core/Guard.hpp"
#    include "../../../entity/EntityRegistry.h"
#    include "../../../ride/Ride.h"
#    include "../../../ride/Track.h"
#    include "../../../world/Footpath.h"
#    include "../../../world/Scenery.h"
#    include "../../../world/Surface.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <cstdio>
#    include <cstring>
#    include <utility>

namespace OpenRCT2::Scripting
{
    ScTileElement::ScTileElement(const CoordsXY& coords, TileElement* element)
        : _coords(coords)
        , _element(element)
    {
    }

    std::string ScTileElement::type_get() const
    {
        switch (_element->GetType())
        {
            case TileElementType::Surface:
                return "surface";
            case TileElementType::Path:
                return "footpath";
            case TileElementType::Track:
                return "track";
            case TileElementType::SmallScenery:
                return "small_scenery";
            case TileElementType::Entrance:
                return "entrance";
            case TileElementType::Wall:
                return "wall";
            case TileElementType::LargeScenery:
                return "large_scenery";
            case TileElementType::Banner:
                return "banner";
            default:
                return "unknown";
        }
    }

    void ScTileElement::type_set(std::string value)
    {
        if (value == "surface")
            _element->SetType(TileElementType::Surface);
        else if (value == "footpath")
            _element->SetType(TileElementType::Path);
        else if (value == "track")
            _element->SetType(TileElementType::Track);
        else if (value == "small_scenery")
            _element->SetType(TileElementType::SmallScenery);
        else if (value == "entrance")
            _element->SetType(TileElementType::Entrance);
        else if (value == "wall")
            _element->SetType(TileElementType::Wall);
        else if (value == "large_scenery")
            _element->SetType(TileElementType::LargeScenery);
        else if (value == "banner")
            _element->SetType(TileElementType::Banner);
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Element type not recognised!");
            return;
        }

        Invalidate();
    }

    uint8_t ScTileElement::baseHeight_get() const
    {
        return _element->base_height;
    }
    void ScTileElement::baseHeight_set(uint8_t newBaseHeight)
    {
        ThrowIfGameStateNotMutable();
        _element->base_height = newBaseHeight;
        Invalidate();
    }

    uint16_t ScTileElement::baseZ_get() const
    {
        return _element->GetBaseZ();
    }
    void ScTileElement::baseZ_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        _element->SetBaseZ(value);
        Invalidate();
    }

    uint8_t ScTileElement::clearanceHeight_get() const
    {
        return _element->clearance_height;
    }
    void ScTileElement::clearanceHeight_set(uint8_t newClearanceHeight)
    {
        ThrowIfGameStateNotMutable();
        _element->clearance_height = newClearanceHeight;
        Invalidate();
    }

    uint16_t ScTileElement::clearanceZ_get() const
    {
        return _element->GetClearanceZ();
    }
    void ScTileElement::clearanceZ_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        _element->SetClearanceZ(value);
        Invalidate();
    }

    DukValue ScTileElement::slope_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        switch (_element->GetType())
        {
            case TileElementType::Surface:
            {
                auto* el = _element->AsSurface();
                duk_push_int(ctx, el->GetSlope());
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                duk_push_int(ctx, el->GetSlope());
                break;
            }
            default:
            {
                scriptEngine.LogPluginInfo(
                    "Cannot read 'slope' property, tile element is not a SurfaceElement or WallElement.");
                duk_push_null(ctx);
                break;
            }
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::slope_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        const auto type = _element->GetType();

        if (type == TileElementType::Surface)
        {
            auto* el = _element->AsSurface();
            el->SetSlope(value);
            Invalidate();
        }
        else if (type == TileElementType::Wall)
        {
            auto* el = _element->AsWall();
            el->SetSlope(value);
            Invalidate();
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'slope' property, tile element is not a SurfaceElement or WallElement.");
        }
    }

    DukValue ScTileElement::waterHeight_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetWaterHeight());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'waterHeight' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::waterHeight_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'waterHeight' property, tile element is not a SurfaceElement.");
            return;
        }

        el->SetWaterHeight(value);
        Invalidate();
    }

    DukValue ScTileElement::surfaceStyle_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetSurfaceStyle());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'surfaceStyle' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::surfaceStyle_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'surfaceStyle' property, tile element is not a SurfaceElement.");
            return;
        }

        el->SetSurfaceStyle(value);
        Invalidate();
    }

    DukValue ScTileElement::edgeStyle_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetEdgeStyle());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'edgeStyle' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::edgeStyle_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'edgeStyle' property, tile element is not a SurfaceElement.");
            return;
        }

        el->SetEdgeStyle(value);
        Invalidate();
    }

    DukValue ScTileElement::grassLength_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetGrassLength());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'grassLength' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::grassLength_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'grassLength' property, tile element is not a SurfaceElement.");
            return;
        }

        // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
        el->SetGrassLengthAndInvalidate(value, _coords);
        Invalidate();
    }

    DukValue ScTileElement::hasOwnership_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_boolean(ctx, el->GetOwnership() & OWNERSHIP_OWNED);
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'hasOwnership' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScTileElement::hasConstructionRights_get()
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            auto ownership = el->GetOwnership();
            duk_push_boolean(ctx, (ownership & OWNERSHIP_OWNED) || (ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED));
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'hasConstructionRights' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }

    DukValue ScTileElement::ownership_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetOwnership());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'ownership' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::ownership_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'ownership' property, tile element is not a SurfaceElement.");
            return;
        }

        el->SetOwnership(value);
        Invalidate();
    }

    DukValue ScTileElement::parkFences_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSurface();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetParkFences());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'parkFences' property, tile element is not a SurfaceElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::parkFences_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'parkFences' property, tile element is not a SurfaceElement.");
            return;
        }

        el->SetParkFences(value);
        Invalidate();
    }

    DukValue ScTileElement::trackType_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsTrack();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetTrackType());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'trackType' property, tile element is not a TrackElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::trackType_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'trackType' property, tile element is not a TrackElement.");
            return;
        }

        el->SetTrackType(value);
        Invalidate();
    }

    DukValue ScTileElement::rideType_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsTrack();
        if (el != nullptr)
        {
            duk_push_int(ctx, el->GetRideType());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot set 'rideType' property, tile element is not a TrackElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::rideType_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value < RIDE_TYPE_COUNT)
                throw DukException() << "'rideType' value is invalid.";

            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot set 'rideType' property, tile element is not a TrackElement.";

            el->SetRideType(value);
            Invalidate();
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::sequence_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            switch (_element->GetType())
            {
                case TileElementType::LargeScenery:
                {
                    auto* el = _element->AsLargeScenery();
                    duk_push_int(ctx, el->GetSequenceIndex());
                    break;
                }
                case TileElementType::Track:
                {
                    auto* el = _element->AsTrack();
                    auto* ride = get_ride(el->GetRideIndex());
                    if (ride != nullptr && ride->type == RIDE_TYPE_MAZE)
                        throw DukException() << "Cannot read 'sequence' property, TrackElement belongs to a maze.";

                    duk_push_int(ctx, el->GetSequenceIndex());
                    break;
                }
                case TileElementType::Entrance:
                {
                    auto* el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetSequenceIndex());
                    break;
                }
                default:
                    throw DukException() << "Cannot read 'sequence' property, tile element is not a TrackElement, "
                                            "LargeSceneryElement, or EntranceElement.";
            }
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::sequence_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value.type() != DukValue::Type::NUMBER)
                throw DukException() << "'sequence' must be a number.";

            switch (_element->GetType())
            {
                case TileElementType::LargeScenery:
                {
                    auto* el = _element->AsLargeScenery();
                    el->SetSequenceIndex(value.as_uint());
                    Invalidate();
                    break;
                }
                case TileElementType::Track:
                {
                    auto* el = _element->AsTrack();
                    auto ride = get_ride(el->GetRideIndex());
                    if (ride != nullptr && ride->type == RIDE_TYPE_MAZE)
                        throw DukException() << "Cannot set 'sequence' property, TrackElement belongs to a maze.";

                    el->SetSequenceIndex(value.as_uint());
                    Invalidate();
                    break;
                }
                case TileElementType::Entrance:
                {
                    auto* el = _element->AsEntrance();
                    el->SetSequenceIndex(value.as_uint());
                    Invalidate();
                    break;
                }
                default:
                    throw DukException() << "Cannot set 'rideType' property, tile element is not a TrackElement, "
                                            "LargeSceneryElement, or EntranceElement.";
            }
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::ride_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            switch (_element->GetType())
            {
                case TileElementType::Path:
                {
                    auto* el = _element->AsPath();
                    if (!el->IsQueue())
                        throw DukException() << "Cannot read 'ride' property, path is not a queue.";

                    if (!el->GetRideIndex().IsNull())
                        duk_push_int(ctx, el->GetRideIndex().ToUnderlying());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TileElementType::Track:
                {
                    auto* el = _element->AsTrack();
                    duk_push_int(ctx, el->GetRideIndex().ToUnderlying());
                    break;
                }
                case TileElementType::Entrance:
                {
                    auto* el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetRideIndex().ToUnderlying());
                    break;
                }
                default:
                    throw DukException()
                        << "Cannot read 'ride' property, tile element is not PathElement, TrackElement, or EntranceElement";
            }
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::ride_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            switch (_element->GetType())
            {
                case TileElementType::Path:
                {
                    auto* el = _element->AsPath();
                    if (!el->IsQueue())
                        throw DukException() << "Cannot set ride property, path is not a queue.";

                    if (value.type() == DukValue::Type::NUMBER)
                        el->SetRideIndex(RideId::FromUnderlying(value.as_uint()));
                    else if (value.type() == DukValue::Type::NULLREF)
                        el->SetRideIndex(RideId::GetNull());
                    else
                        throw DukException() << "'ride' must be a number or null.";
                    Invalidate();
                    break;
                }
                case TileElementType::Track:
                {
                    if (value.type() != DukValue::Type::NUMBER)
                        throw DukException() << "'ride' must be a number.";

                    auto* el = _element->AsTrack();
                    el->SetRideIndex(RideId::FromUnderlying(value.as_uint()));
                    Invalidate();
                    break;
                }
                case TileElementType::Entrance:
                {
                    if (value.type() != DukValue::Type::NUMBER)
                        throw DukException() << "'ride' must be a number.";

                    auto* el = _element->AsEntrance();
                    el->SetRideIndex(RideId::FromUnderlying(value.as_uint()));
                    Invalidate();
                    break;
                }
                default:
                    throw DukException()
                        << "Cannot set 'ride' property, tile element is not PathElement, TrackElement, or EntranceElement";
            }
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::station_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            switch (_element->GetType())
            {
                case TileElementType::Path:
                {
                    auto* el = _element->AsPath();
                    if (!el->IsQueue())
                        throw DukException() << "Cannot read 'station' property, path is not a queue.";

                    if (el->GetRideIndex().IsNull())
                        throw DukException() << "Cannot read 'station' property, queue is not linked to a ride.";

                    if (!el->GetStationIndex().IsNull())
                        duk_push_int(ctx, el->GetStationIndex().ToUnderlying());
                    else
                        duk_push_null(ctx);
                    break;
                }
                case TileElementType::Track:
                {
                    auto* el = _element->AsTrack();
                    if (!el->IsStation())
                        throw DukException() << "Cannot read 'station' property, track is not a station.";

                    duk_push_int(ctx, el->GetStationIndex().ToUnderlying());
                    break;
                }
                case TileElementType::Entrance:
                {
                    auto* el = _element->AsEntrance();
                    duk_push_int(ctx, el->GetStationIndex().ToUnderlying());
                    break;
                }
                default:
                    throw DukException()
                        << "Cannot set 'station' property, tile element is not PathElement, TrackElement, or EntranceElement";
            }
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::station_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            switch (_element->GetType())
            {
                case TileElementType::Path:
                {
                    auto* el = _element->AsPath();
                    if (value.type() == DukValue::Type::NUMBER)
                        el->SetStationIndex(StationIndex::FromUnderlying(value.as_uint()));
                    else if (value.type() == DukValue::Type::NULLREF)
                        el->SetStationIndex(StationIndex::GetNull());
                    else
                        throw DukException() << "'station' must be a number or null.";
                    Invalidate();
                    break;
                }
                case TileElementType::Track:
                {
                    if (value.type() != DukValue::Type::NUMBER)
                        throw DukException() << "'station' must be a number.";

                    auto* el = _element->AsTrack();
                    el->SetStationIndex(StationIndex::FromUnderlying(value.as_uint()));
                    Invalidate();
                    break;
                }
                case TileElementType::Entrance:
                {
                    if (value.type() != DukValue::Type::NUMBER)
                        throw DukException() << "'station' must be a number.";

                    auto* el = _element->AsEntrance();
                    el->SetStationIndex(StationIndex::FromUnderlying(value.as_uint()));
                    Invalidate();
                    break;
                }
                default:
                    break;
            }
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::hasChainLift_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsTrack();
        if (el != nullptr)
        {
            duk_push_boolean(ctx, el->HasChain());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'hasChainLift' property, tile element is not a TrackElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::hasChainLift_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasChainLift' property, tile element is not a TrackElement.");
            return;
        }

        el->SetHasChain(value);
        Invalidate();
    }

    DukValue ScTileElement::mazeEntry_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot read 'mazeEntry' property, element is not a TrackElement.";

            Ride* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot read 'mazeEntry' property, ride is invalid.";

            if (ride->type != RIDE_TYPE_MAZE)
                throw DukException() << "Cannot read 'mazeEntry' property, ride is not a maze.";

            duk_push_int(ctx, el->GetMazeEntry());
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::mazeEntry_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value.type() != DukValue::Type::NUMBER)
                throw DukException() << "'mazeEntry' property must be a number.";

            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot set 'mazeEntry' property, tile element is not a TrackElement.";

            auto* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot set 'mazeEntry' property, ride is invalid.";

            if (ride->type != RIDE_TYPE_MAZE)
                throw DukException() << "Cannot set 'mazeEntry' property, ride is not a maze.";

            el->SetMazeEntry(value.as_uint());
            Invalidate();
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::colourScheme_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot read 'colourScheme' property, tile element is not a TrackElement.";

            auto* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot read 'colourScheme' property, ride is invalid.";

            if (ride->type == RIDE_TYPE_MAZE)
                throw DukException() << "Cannot read 'colourScheme' property, TrackElement belongs to a maze.";

            duk_push_int(ctx, el->GetColourScheme());
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::colourScheme_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value.type() != DukValue::Type::NUMBER)
                throw DukException() << "'colourScheme' must be a number.";

            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot set 'colourScheme' property, tile element is not a TrackElement.";

            auto* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot set 'colourScheme', ride is invalid.";

            if (ride->type == RIDE_TYPE_MAZE)
                throw DukException() << "Cannot set 'colourScheme' property, TrackElement belongs to a maze.";

            el->SetColourScheme(value.as_uint());
            Invalidate();
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::seatRotation_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot read 'seatRotation' property, tile element is not a TrackElement.";

            auto* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot read 'seatRotation' property, ride is invalid.";

            if (ride->type == RIDE_TYPE_MAZE)
                throw DukException() << "Cannot read 'seatRotation' property, TrackElement belongs to a maze.";

            duk_push_int(ctx, el->GetSeatRotation());
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::seatRotation_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value.type() != DukValue::Type::NUMBER)
                throw DukException() << "'seatRotation' must be a number.";

            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot set 'seatRotation' property, tile element is not a TrackElement.";

            auto* ride = get_ride(el->GetRideIndex());
            if (ride == nullptr)
                throw DukException() << "Cannot set 'seatRotation' property, ride is invalid.";

            if (ride->type != RIDE_TYPE_MAZE)
                throw DukException() << "Cannot set 'seatRotation' property, TrackElement belongs to a maze.";

            el->SetSeatRotation(value.as_uint());
            Invalidate();
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::brakeBoosterSpeed_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        try
        {
            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot read 'brakeBoosterSpeed' property, tile element is not a TrackElement.";

            if (!TrackTypeHasSpeedSetting(el->GetTrackType()))
                throw DukException() << "Cannot read 'brakeBoosterSpeed' property, track element has no speed setting.";

            duk_push_int(ctx, el->GetBrakeBoosterSpeed());
        }
        catch (const DukException& e)
        {
            scriptEngine.LogPluginInfo(e.what());
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::brakeBoosterSpeed_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        try
        {
            if (value.type() != DukValue::Type::NUMBER)
                throw DukException() << "'brakeBoosterSpeed' must be a number.";

            auto* el = _element->AsTrack();
            if (el == nullptr)
                throw DukException() << "Cannot set 'brakeBoosterSpeed' property, tile element is not a TrackElement.";

            if (TrackTypeHasSpeedSetting(el->GetTrackType()))
                throw DukException() << "Cannot set 'brakeBoosterSpeed' property, track element has no speed setting.";

            el->SetBrakeBoosterSpeed(value.as_uint());
            Invalidate();
        }
        catch (const DukException& e)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo(e.what());
        }
    }

    DukValue ScTileElement::isInverted_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsTrack();
        if (el != nullptr)
        {
            duk_push_boolean(ctx, el->IsInverted());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'isInverted' property, tile element is not a TrackElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isInverted_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'isInverted' property, tile element is not a TrackElement.");
            return;
        }

        el->SetInverted(value);
        Invalidate();
    }

    DukValue ScTileElement::hasCableLift_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsTrack();
        if (el != nullptr)
        {
            duk_push_boolean(ctx, el->HasCableLift());
        }
        else
        {
            scriptEngine.LogPluginInfo("Cannot read 'hasCableLift' property, tile element is not a TrackElement.");
            duk_push_null(ctx);
        }
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::hasCableLift_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasCableLift' property, tile element is not a TrackElement.");
            return;
        }

        el->SetHasCableLift(value);
        Invalidate();
    }

    DukValue ScTileElement::isHighlighted_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto el = _element->AsTrack();
        if (el != nullptr)
            duk_push_boolean(ctx, el->IsHighlighted());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isHighlighted_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto el = _element->AsTrack();
        if (el != nullptr)
        {
            el->SetHighlight(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::object_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        switch (_element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = _element->AsPath();
                auto index = el->GetLegacyPathEntryIndex();
                if (index != OBJECT_ENTRY_INDEX_NULL)
                    duk_push_int(ctx, index);
                else
                    duk_push_null(ctx);
                break;
            }
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                duk_push_int(ctx, el->GetEntryIndex());
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                duk_push_int(ctx, el->GetEntryIndex());
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                duk_push_int(ctx, el->GetEntryIndex());
                break;
            }
            case TileElementType::Entrance:
            {
                auto* el = _element->AsEntrance();
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

    void ScTileElement::object_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();

        auto index = FromDuk<ObjectEntryIndex>(value);
        switch (_element->GetType())
        {
            case TileElementType::Path:
            {
                if (value.type() == DukValue::Type::NUMBER)
                {
                    auto* el = _element->AsPath();
                    el->SetLegacyPathEntryIndex(index);
                    Invalidate();
                }
                break;
            }
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                el->SetEntryIndex(index);
                Invalidate();
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                el->SetEntryIndex(index);
                Invalidate();
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                el->SetEntryIndex(index);
                Invalidate();
                break;
            }
            case TileElementType::Entrance:
            {
                auto* el = _element->AsEntrance();
                el->SetEntranceType(index);
                Invalidate();
                break;
            }
            default:
                break;
        }
    }

    bool ScTileElement::isHidden_get() const
    {
        return _element->IsInvisible();
    }

    void ScTileElement::isHidden_set(bool hide)
    {
        ThrowIfGameStateNotMutable();
        _element->SetInvisible(hide);
        Invalidate();
    }

    DukValue ScTileElement::age_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSmallScenery();
        if (el != nullptr)
            duk_push_int(ctx, el->GetAge());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::age_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSmallScenery();
        if (el != nullptr)
        {
            el->SetAge(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::quadrant_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsSmallScenery();
        if (el != nullptr)
            duk_push_int(ctx, el->GetSceneryQuadrant());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::quadrant_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsSmallScenery();
        if (el != nullptr)
        {
            el->SetSceneryQuadrant(value);
            Invalidate();
        }
    }

    uint8_t ScTileElement::occupiedQuadrants_get() const
    {
        return _element->GetOccupiedQuadrants();
    }
    void ScTileElement::occupiedQuadrants_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        _element->SetOccupiedQuadrants(value);
        Invalidate();
    }

    bool ScTileElement::isGhost_get() const
    {
        return _element->IsGhost();
    }
    void ScTileElement::isGhost_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        _element->SetGhost(value);
        Invalidate();
    }

    DukValue ScTileElement::primaryColour_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        switch (_element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                duk_push_int(ctx, el->GetPrimaryColour());
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                duk_push_int(ctx, el->GetPrimaryColour());
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
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
    void ScTileElement::primaryColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        switch (_element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                el->SetPrimaryColour(value);
                Invalidate();
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                el->SetPrimaryColour(value);
                Invalidate();
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                el->SetPrimaryColour(value);
                Invalidate();
                break;
            }
            default:
                break;
        }
    }

    DukValue ScTileElement::secondaryColour_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        switch (_element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                duk_push_int(ctx, el->GetSecondaryColour());
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                duk_push_int(ctx, el->GetSecondaryColour());
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
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
    void ScTileElement::secondaryColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        switch (_element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = _element->AsSmallScenery();
                el->SetSecondaryColour(value);
                Invalidate();
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                el->SetSecondaryColour(value);
                Invalidate();
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                el->SetSecondaryColour(value);
                Invalidate();
                break;
            }
            default:
                break;
        }
    }

    DukValue ScTileElement::tertiaryColour_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsWall();
        if (el != nullptr)
            duk_push_int(ctx, el->GetTertiaryColour());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::tertiaryColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsWall();
        if (el != nullptr)
        {
            el->SetTertiaryColour(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::bannerIndex_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        BannerIndex idx = _element->GetBannerIndex();
        if (idx == BannerIndex::GetNull())
            duk_push_null(ctx);
        else
            duk_push_int(ctx, idx.ToUnderlying());
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::bannerIndex_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        switch (_element->GetType())
        {
            case TileElementType::LargeScenery:
            {
                auto* el = _element->AsLargeScenery();
                if (value.type() == DukValue::Type::NUMBER)
                    el->SetBannerIndex(BannerIndex::FromUnderlying(value.as_uint()));
                else
                    el->SetBannerIndex(BannerIndex::GetNull());
                Invalidate();
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = _element->AsWall();
                if (value.type() == DukValue::Type::NUMBER)
                    el->SetBannerIndex(BannerIndex::FromUnderlying(value.as_uint()));
                else
                    el->SetBannerIndex(BannerIndex::GetNull());
                Invalidate();
                break;
            }
            case TileElementType::Banner:
            {
                auto* el = _element->AsBanner();
                if (value.type() == DukValue::Type::NUMBER)
                    el->SetIndex(BannerIndex::FromUnderlying(value.as_uint()));
                else
                    el->SetIndex(BannerIndex::GetNull());
                Invalidate();
                break;
            }
            default:
                break;
        }
    }

    // Deprecated in favor of seperate 'edges' and 'corners' properties,
    // left here to maintain compatibility with older plugins.
    /** @deprecated */
    uint8_t ScTileElement::edgesAndCorners_get() const
    {
        auto* el = _element->AsPath();
        return el != nullptr ? el->GetEdgesAndCorners() : 0;
    }
    /** @deprecated */
    void ScTileElement::edgesAndCorners_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetEdgesAndCorners(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::edges_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr)
            duk_push_int(ctx, el->GetEdges());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::edges_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetEdges(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::corners_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr)
            duk_push_int(ctx, el->GetCorners());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::corners_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetCorners(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::slopeDirection_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->IsSloped())
            duk_push_int(ctx, el->GetSlopeDirection());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::slopeDirection_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
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

    DukValue ScTileElement::isQueue_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr)
            duk_push_boolean(ctx, el->IsQueue());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isQueue_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetIsQueue(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::queueBannerDirection_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->HasQueueBanner())
            duk_push_int(ctx, el->GetQueueBannerDirection());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::queueBannerDirection_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
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

    DukValue ScTileElement::isBlockedByVehicle_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr)
            duk_push_boolean(ctx, el->IsBlockedByVehicle());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isBlockedByVehicle_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetIsBlockedByVehicle(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::isWide_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr)
            duk_push_boolean(ctx, el->IsWide());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isWide_set(bool value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
        if (el != nullptr)
        {
            el->SetWide(value);
            Invalidate();
        }
    }

    DukValue ScTileElement::surfaceObject_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        if (_element->GetType() == TileElementType::Path)
        {
            auto* el = _element->AsPath();
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

    void ScTileElement::surfaceObject_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::NUMBER)
        {
            ThrowIfGameStateNotMutable();
            if (_element->GetType() == TileElementType::Path)
            {
                auto* el = _element->AsPath();
                el->SetSurfaceEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::railingsObject_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        if (_element->GetType() == TileElementType::Path)
        {
            auto* el = _element->AsPath();
            auto index = el->GetRailingsEntryIndex();
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

    void ScTileElement::railingsObject_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::NUMBER)
        {
            ThrowIfGameStateNotMutable();
            if (_element->GetType() == TileElementType::Path)
            {
                auto* el = _element->AsPath();
                el->SetRailingsEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::addition_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->HasAddition())
            duk_push_int(ctx, el->GetAddition() - 1);
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::addition_set(const DukValue& value)
    {
        ThrowIfGameStateNotMutable();
        auto* el = _element->AsPath();
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

    DukValue ScTileElement::additionStatus_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->HasAddition() && !el->IsQueue())
            duk_push_int(ctx, el->GetAdditionStatus());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::additionStatus_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::NUMBER)
        {
            ThrowIfGameStateNotMutable();
            auto* el = _element->AsPath();
            if (el != nullptr)
                if (el->HasAddition() && !el->IsQueue())
                {
                    el->SetAdditionStatus(value.as_uint());
                    Invalidate();
                }
        }
    }

    DukValue ScTileElement::isAdditionBroken_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->HasAddition())
            duk_push_boolean(ctx, el->IsBroken());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isAdditionBroken_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::BOOLEAN)
        {
            ThrowIfGameStateNotMutable();
            auto* el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetIsBroken(value.as_bool());
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::isAdditionGhost_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsPath();
        if (el != nullptr && el->HasAddition())
            duk_push_boolean(ctx, el->AdditionIsGhost());
        else
            duk_push_null(ctx);
        return DukValue::take_from_stack(ctx);
    }
    void ScTileElement::isAdditionGhost_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::BOOLEAN)
        {
            ThrowIfGameStateNotMutable();
            auto* el = _element->AsPath();
            if (el != nullptr)
            {
                el->SetAdditionIsGhost(value.as_bool());
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::footpathObject_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsEntrance();
        if (el != nullptr)
        {
            auto index = el->GetLegacyPathEntryIndex();
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
    void ScTileElement::footpathObject_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::NUMBER)
        {
            ThrowIfGameStateNotMutable();
            auto* el = _element->AsEntrance();
            if (el != nullptr)
            {
                el->SetLegacyPathEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::footpathSurfaceObject_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        auto* el = _element->AsEntrance();
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

    void ScTileElement::footpathSurfaceObject_set(const DukValue& value)
    {
        if (value.type() == DukValue::Type::NUMBER)
        {
            ThrowIfGameStateNotMutable();
            auto* el = _element->AsEntrance();
            if (el != nullptr)
            {
                el->SetSurfaceEntryIndex(FromDuk<ObjectEntryIndex>(value));
                Invalidate();
            }
        }
    }

    DukValue ScTileElement::direction_get() const
    {
        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto* ctx = scriptEngine.GetContext();
        switch (_element->GetType())
        {
            case TileElementType::Banner:
            {
                auto* el = _element->AsBanner();
                duk_push_int(ctx, el->GetPosition());
                break;
            }
            case TileElementType::Path:
            case TileElementType::Surface:
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
    void ScTileElement::direction_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        switch (_element->GetType())
        {
            case TileElementType::Banner:
            {
                auto* el = _element->AsBanner();
                el->SetPosition(value);
                Invalidate();
                break;
            }
            case TileElementType::Path:
            case TileElementType::Surface:
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

    void ScTileElement::Invalidate()
    {
        map_invalidate_tile_full(_coords);
    }

    void ScTileElement::Register(duk_context* ctx)
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
        dukglue_register_property(ctx, &ScTileElement::primaryColour_get, &ScTileElement::primaryColour_set, "primaryColour");
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
            ctx, &ScTileElement::queueBannerDirection_get, &ScTileElement::queueBannerDirection_set, "queueBannerDirection");

        dukglue_register_property(
            ctx, &ScTileElement::isBlockedByVehicle_get, &ScTileElement::isBlockedByVehicle_set, "isBlockedByVehicle");
        dukglue_register_property(ctx, &ScTileElement::isWide_get, &ScTileElement::isWide_set, "isWide");

        dukglue_register_property(ctx, &ScTileElement::surfaceObject_get, &ScTileElement::surfaceObject_set, "surfaceObject");
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
        dukglue_register_property(ctx, &ScTileElement::rideType_get, &ScTileElement::rideType_set, "rideType");
        dukglue_register_property(ctx, &ScTileElement::mazeEntry_get, &ScTileElement::mazeEntry_set, "mazeEntry");
        dukglue_register_property(ctx, &ScTileElement::colourScheme_get, &ScTileElement::colourScheme_set, "colourScheme");
        dukglue_register_property(ctx, &ScTileElement::seatRotation_get, &ScTileElement::seatRotation_set, "seatRotation");
        dukglue_register_property(
            ctx, &ScTileElement::brakeBoosterSpeed_get, &ScTileElement::brakeBoosterSpeed_set, "brakeBoosterSpeed");
        dukglue_register_property(ctx, &ScTileElement::hasChainLift_get, &ScTileElement::hasChainLift_set, "hasChainLift");
        dukglue_register_property(ctx, &ScTileElement::isInverted_get, &ScTileElement::isInverted_set, "isInverted");
        dukglue_register_property(ctx, &ScTileElement::hasCableLift_get, &ScTileElement::hasCableLift_set, "hasCableLift");
        dukglue_register_property(ctx, &ScTileElement::isHighlighted_get, &ScTileElement::isHighlighted_set, "isHighlighted");

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
            ctx, &ScTileElement::footpathSurfaceObject_get, &ScTileElement::footpathSurfaceObject_set, "footpathSurfaceObject");
    }

} // namespace OpenRCT2::Scripting

#endif
