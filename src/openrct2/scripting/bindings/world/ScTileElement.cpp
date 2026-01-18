/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScTileElement.hpp"

    #include "../../../Context.h"
    #include "../../../core/Guard.hpp"
    #include "../../../drawing/ScrollingText.h"
    #include "../../../entity/EntityRegistry.h"
    #include "../../../object/LargeSceneryEntry.h"
    #include "../../../object/WallSceneryEntry.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideData.h"
    #include "../../../ride/Track.h"
    #include "../../../world/Footpath.h"
    #include "../../../world/Map.h"
    #include "../../../world/Scenery.h"
    #include "../../../world/tile_element/BannerElement.h"
    #include "../../../world/tile_element/EntranceElement.h"
    #include "../../../world/tile_element/LargeSceneryElement.h"
    #include "../../../world/tile_element/PathElement.h"
    #include "../../../world/tile_element/SmallSceneryElement.h"
    #include "../../../world/tile_element/SurfaceElement.h"
    #include "../../../world/tile_element/TrackElement.h"
    #include "../../../world/tile_element/WallElement.h"
    #include "../../ScriptEngine.h"

    #include <cstdio>
    #include <cstring>
    #include <utility>

namespace OpenRCT2::Scripting
{
    static inline std::string TileElementTypeToString(const TileElement* element)
    {
        switch (element->GetType())
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

    using OpaqueTileElementData = struct
    {
        TileElement* element;
        CoordsXY coords;
    };

    static inline void Invalidate(OpaqueTileElementData* data)
    {
        MapInvalidateTileFull(data->coords);
    }

    JSValue ScTileElement::type_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        auto type = TileElementTypeToString(element);
        return JSFromStdString(ctx, type);
    }

    JSValue ScTileElement::type_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_STR(value, ctx, jsValue);
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        RemoveBannerEntryIfNeeded(element, data->coords);
        if (value == "surface")
            element->SetType(TileElementType::Surface);
        else if (value == "footpath")
            element->SetType(TileElementType::Path);
        else if (value == "track")
            element->SetType(TileElementType::Track);
        else if (value == "small_scenery")
            element->SetType(TileElementType::SmallScenery);
        else if (value == "entrance")
            element->SetType(TileElementType::Entrance);
        else if (value == "wall")
            element->SetType(TileElementType::Wall);
        else if (value == "large_scenery")
            element->SetType(TileElementType::LargeScenery);
        else if (value == "banner")
            element->SetType(TileElementType::Banner);
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Element type not recognised!");
            return JS_UNDEFINED;
        }
        CreateBannerEntryIfNeeded(element, data->coords);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::baseHeight_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->BaseHeight);
    }
    JSValue ScTileElement::baseHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(newBaseHeight, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->BaseHeight = newBaseHeight;
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::baseZ_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->GetBaseZ());
    }
    JSValue ScTileElement::baseZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->SetBaseZ(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::clearanceHeight_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->ClearanceHeight);
    }
    JSValue ScTileElement::clearanceHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(newClearanceHeight, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->ClearanceHeight = newClearanceHeight;
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::clearanceZ_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->GetClearanceZ());
    }
    JSValue ScTileElement::clearanceZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->SetClearanceZ(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::slope_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Surface:
            {
                auto* el = element->AsSurface();
                return JS_NewUint32(ctx, el->GetSlope());
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                return JS_NewUint32(ctx, el->GetSlope());
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot read 'slope' property, tile element is not a SurfaceElement or WallElement.");
                return JS_NULL;
            }
        }
    }
    JSValue ScTileElement::slope_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        const auto type = element->GetType();

        if (type == TileElementType::Surface)
        {
            auto* el = element->AsSurface();
            el->SetSlope(value);
            Invalidate(data);
        }
        else if (type == TileElementType::Wall)
        {
            auto* el = element->AsWall();
            el->SetSlope(value);
            Invalidate(data);
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'slope' property, tile element is not a SurfaceElement or WallElement.");
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::waterHeight_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewInt32(ctx, el->GetWaterHeight());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'waterHeight' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::waterHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'waterHeight' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->SetWaterHeight(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::surfaceStyle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetSurfaceObjectIndex());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'surfaceStyle' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::surfaceStyle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'surfaceStyle' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->SetSurfaceObjectIndex(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::edgeStyle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetEdgeObjectIndex());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'edgeStyle' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::edgeStyle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'edgeStyle' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->SetEdgeObjectIndex(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::grassLength_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetGrassLength());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'grassLength' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::grassLength_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'grassLength' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
        el->SetGrassLengthAndInvalidate(value, data->coords);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::hasOwnership_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->GetOwnership() & OWNERSHIP_OWNED);
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'hasOwnership' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }

    JSValue ScTileElement::hasConstructionRights_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            auto ownership = el->GetOwnership();
            return JS_NewBool(ctx, (ownership & OWNERSHIP_OWNED) || (ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED));
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'hasConstructionRights' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }

    JSValue ScTileElement::ownership_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetOwnership());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'ownership' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::ownership_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'ownership' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->SetOwnership(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::parkFences_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetParkFences());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'parkFences' property, tile element is not a SurfaceElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::parkFences_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'parkFences' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->SetParkFences(value);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::trackType_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, EnumValue(el->GetTrackType()));
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'trackType' property, tile element is not a TrackElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::trackType_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'trackType' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->SetTrackType(static_cast<TrackElemType>(value));
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::rideType_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->GetRideType());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'rideType' property, tile element is not a TrackElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::rideType_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        if (value >= RIDE_TYPE_COUNT)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("'rideType' value is invalid.");
            return JS_UNDEFINED;
        }

        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'rideType' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->SetRideType(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::sequence_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                return JS_NewUint32(ctx, el->GetSequenceIndex());
            }
            case TileElementType::Track:
            {
                auto* el = element->AsTrack();
                auto* ride = GetRide(el->GetRideIndex());

                if (ride != nullptr)
                {
                    const auto& rtd = ride->getRideTypeDescriptor();
                    if (rtd.specialType == RtdSpecialType::maze)
                    {
                        auto& scriptEngine = GetContext()->GetScriptEngine();
                        scriptEngine.LogPluginInfo("Cannot read 'sequence' property, TrackElement belongs to a maze.");
                        return JS_NULL;
                    }
                }

                return JS_NewUint32(ctx, el->GetSequenceIndex());
            }
            case TileElementType::Entrance:
            {
                auto* el = element->AsEntrance();
                return JS_NewUint32(ctx, el->GetSequenceIndex());
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot read 'sequence' property, tile element is not a TrackElement, "
                    "LargeSceneryElement, or EntranceElement.");
                return JS_NULL;
            }
        }
    }
    JSValue ScTileElement::sequence_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;

        switch (element->GetType())
        {
            case TileElementType::LargeScenery:
            {
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->AsLargeScenery();
                el->SetSequenceIndex(value);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::Track:
            {
                auto* el = element->AsTrack();
                auto ride = GetRide(el->GetRideIndex());

                if (ride != nullptr)
                {
                    const auto& rtd = ride->getRideTypeDescriptor();
                    if (rtd.specialType == RtdSpecialType::maze)
                    {
                        auto& scriptEngine = GetContext()->GetScriptEngine();
                        scriptEngine.LogPluginInfo("Cannot read 'sequence' property, TrackElement belongs to a maze.");
                        return JS_UNDEFINED;
                    }
                }

                el->SetSequenceIndex(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Entrance:
            {
                auto* el = element->AsEntrance();
                el->SetSequenceIndex(value);
                Invalidate(data);
                break;
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot read 'sequence' property, tile element is not a TrackElement, "
                    "LargeSceneryElement, or EntranceElement.");
                break;
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::ride_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = element->AsPath();
                if (!el->IsQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'ride' property, path is not a queue.");
                    return JS_NULL;
                }

                if (!el->GetRideIndex().IsNull())
                    return JS_NewUint32(ctx, el->GetRideIndex().ToUnderlying());

                return JS_NULL;
            }
            case TileElementType::Track:
            {
                auto* el = element->AsTrack();
                return JS_NewUint32(ctx, el->GetRideIndex().ToUnderlying());
            }
            case TileElementType::Entrance:
            {
                auto* el = element->AsEntrance();
                return JS_NewUint32(ctx, el->GetRideIndex().ToUnderlying());
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot read 'ride' property, tile element is not PathElement, TrackElement, or EntranceElement");
                return JS_NULL;
            }
        }
    }
    JSValue ScTileElement::ride_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;

        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = element->AsPath();
                if (!el->IsQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot set ride property, path is not a queue.");
                    return JS_UNDEFINED;
                }

                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetRideIndex(RideId::FromUnderlying(value));
                }
                else if (JS_IsNull(jsValue))
                {
                    el->SetRideIndex(RideId::GetNull());
                }
                else
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'ride' must be a number or null.");
                    return JS_UNDEFINED;
                }
                Invalidate(data);
                break;
            }
            case TileElementType::Track:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'ride' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsTrack();
                el->SetRideIndex(RideId::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            case TileElementType::Entrance:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'ride' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsEntrance();
                el->SetRideIndex(RideId::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot set 'ride' property, tile element is not PathElement, TrackElement, or EntranceElement");
                break;
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::station_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = element->AsPath();
                if (!el->IsQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, path is not a queue.");
                    return JS_NULL;
                }

                if (el->GetRideIndex().IsNull())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, queue is not linked to a ride.");
                    return JS_NULL;
                }

                if (!el->GetStationIndex().IsNull())
                    return JS_NewUint32(ctx, el->GetStationIndex().ToUnderlying());

                return JS_NULL;
            }
            case TileElementType::Track:
            {
                auto* el = element->AsTrack();
                if (!el->IsStation())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, track is not a station.");
                    return JS_NULL;
                }

                return JS_NewUint32(ctx, el->GetStationIndex().ToUnderlying());
            }
            case TileElementType::Entrance:
            {
                auto* el = element->AsEntrance();
                return JS_NewUint32(ctx, el->GetStationIndex().ToUnderlying());
            }
            default:
            {
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.LogPluginInfo(
                    "Cannot read 'station' property, tile element is not PathElement, TrackElement, or EntranceElement");
                return JS_NULL;
            }
        }
    }
    JSValue ScTileElement::station_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;

        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = element->AsPath();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetStationIndex(StationIndex::FromUnderlying(value));
                }
                else if (JS_IsNull(jsValue))
                {
                    el->SetStationIndex(StationIndex::GetNull());
                }
                else
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'station' must be a number or null.");
                    return JS_UNDEFINED;
                }
                Invalidate(data);
                break;
            }
            case TileElementType::Track:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'station' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsTrack();
                el->SetStationIndex(StationIndex::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            case TileElementType::Entrance:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'station' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsEntrance();
                el->SetStationIndex(StationIndex::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::hasChainLift_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->HasChain());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'hasChainLift' property, tile element is not a TrackElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::hasChainLift_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasChainLift' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->SetHasChain(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::mazeEntry_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'mazeEntry' property, element is not a TrackElement.");
            return JS_NULL;
        }

        Ride* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'mazeEntry' property, ride is invalid.");
            return JS_NULL;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType != RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'mazeEntry' property, ride is not a maze.");
            return JS_NULL;
        }

        return JS_NewUint32(ctx, el->GetMazeEntry());
    }
    JSValue ScTileElement::mazeEntry_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        if (!JS_IsNumber(jsValue))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("'mazeEntry' property must be a number.");
            return JS_UNDEFINED;
        }

        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'mazeEntry' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'mazeEntry' property, ride is invalid.");
            return JS_UNDEFINED;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType != RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'mazeEntry' property, ride is not a maze.");
            return JS_UNDEFINED;
        }

        JS_UNPACK_UINT32(value, ctx, jsValue);
        el->SetMazeEntry(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::colourScheme_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'colourScheme' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        auto* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'colourScheme' property, ride is invalid.");
            return JS_NULL;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'colourScheme' property, TrackElement belongs to a maze.");
            return JS_NULL;
        }

        return JS_NewUint32(ctx, el->GetColourScheme());
    }
    JSValue ScTileElement::colourScheme_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        if (!JS_IsNumber(jsValue))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("'colourScheme' must be a number.");
            return JS_UNDEFINED;
        }

        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'colourScheme' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'colourScheme', ride is invalid.");
            return JS_UNDEFINED;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'colourScheme' property, TrackElement belongs to a maze.");
            return JS_UNDEFINED;
        }

        JS_UNPACK_UINT32(value, ctx, jsValue);
        el->SetColourScheme(static_cast<RideColourScheme>(value));
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::seatRotation_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'seatRotation' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        auto* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'seatRotation' property, ride is invalid.");
            return JS_NULL;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'seatRotation' property, TrackElement belongs to a maze.");
            return JS_NULL;
        }

        return JS_NewUint32(ctx, el->GetSeatRotation());
    }
    JSValue ScTileElement::seatRotation_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        if (!JS_IsNumber(jsValue))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("'seatRotation' must be a number.");
            return JS_UNDEFINED;
        }

        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'seatRotation' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->GetRideIndex());
        if (ride == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'seatRotation' property, ride is invalid.");
            return JS_UNDEFINED;
        }

        const auto& rtd = ride->getRideTypeDescriptor();
        if (rtd.specialType != RtdSpecialType::maze)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'seatRotation' property, TrackElement belongs to a maze.");
            return JS_UNDEFINED;
        }

        JS_UNPACK_UINT32(value, ctx, jsValue);
        el->SetSeatRotation(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::brakeBoosterSpeed_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'brakeBoosterSpeed' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        if (!TrackTypeHasSpeedSetting(el->GetTrackType()))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'brakeBoosterSpeed' property, track element has no speed setting.");
            return JS_NULL;
        }

        return JS_NewUint32(ctx, el->GetBrakeBoosterSpeed());
    }
    JSValue ScTileElement::brakeBoosterSpeed_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        if (!JS_IsNumber(jsValue))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("'brakeBoosterSpeed' must be a number.");
            return JS_UNDEFINED;
        }

        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'brakeBoosterSpeed' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        if (!TrackTypeHasSpeedSetting(el->GetTrackType()))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'brakeBoosterSpeed' property, track element has no speed setting.");
            return JS_UNDEFINED;
        }

        JS_UNPACK_UINT32(value, ctx, jsValue);
        el->SetBrakeBoosterSpeed(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isInverted_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->IsInverted());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'isInverted' property, tile element is not a TrackElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::isInverted_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'isInverted' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->SetInverted(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::hasCableLift_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->HasCableLift());
        }
        else
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'hasCableLift' property, tile element is not a TrackElement.");
            return JS_NULL;
        }
    }
    JSValue ScTileElement::hasCableLift_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasCableLift' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->SetHasCableLift(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isHighlighted_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto el = data->element->AsTrack();
        if (el != nullptr)
            return JS_NewBool(ctx, el->IsHighlighted());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isHighlighted_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto el = data->element->AsTrack();
        if (el != nullptr)
        {
            el->SetHighlight(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::object_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                auto* el = element->AsPath();
                auto index = el->GetLegacyPathEntryIndex();
                if (index != kObjectEntryIndexNull)
                    return JS_NewUint32(ctx, index);

                return JS_NULL;
            }
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                return JS_NewUint32(ctx, el->GetEntryIndex());
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                return JS_NewUint32(ctx, el->GetEntryIndex());
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                return JS_NewUint32(ctx, el->GetEntryIndex());
            }
            case TileElementType::Entrance:
            {
                auto* el = element->AsEntrance();
                return JS_NewUint32(ctx, el->GetEntranceType());
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                return JS_NewUint32(ctx, el->GetBanner()->type);
            }
            default:
                return JS_NULL;
        }
    }

    JSValue ScTileElement::object_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;

        switch (element->GetType())
        {
            case TileElementType::Path:
            {
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(index, ctx, jsValue);
                    auto* el = element->AsPath();
                    el->SetLegacyPathEntryIndex(index);
                    Invalidate(data);
                }
                break;
            }
            case TileElementType::SmallScenery:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->AsSmallScenery();
                el->SetEntryIndex(index);
                Invalidate(data);
                break;
            }
            case TileElementType::LargeScenery:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->AsLargeScenery();
                el->SetEntryIndex(index);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::Wall:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->AsWall();
                el->SetEntryIndex(index);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::Entrance:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->AsEntrance();
                el->SetEntranceType(index);
                Invalidate(data);
                break;
            }
            case TileElementType::Banner:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->AsBanner();
                el->GetBanner()->type = index;
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isHidden_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        return JS_NewBool(ctx, data->element->IsInvisible());
    }

    JSValue ScTileElement::isHidden_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(hide, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->SetInvisible(hide);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::age_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSmallScenery();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->GetAge());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::age_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSmallScenery();
        if (el != nullptr)
        {
            el->SetAge(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::quadrant_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSmallScenery();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->GetSceneryQuadrant());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::quadrant_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsSmallScenery();
        if (el != nullptr)
        {
            el->SetSceneryQuadrant(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::occupiedQuadrants_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        return JS_NewUint32(ctx, data->element->GetOccupiedQuadrants());
    }
    JSValue ScTileElement::occupiedQuadrants_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->SetOccupiedQuadrants(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isGhost_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        return JS_NewBool(ctx, data->element->IsGhost());
    }
    JSValue ScTileElement::isGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->SetGhost(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::primaryColour_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                return JS_NewUint32(ctx, el->GetPrimaryColour());
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                return JS_NewUint32(ctx, el->GetPrimaryColour());
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                return JS_NewUint32(ctx, el->GetPrimaryColour());
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                return JS_NewUint32(ctx, el->GetBanner()->colour);
            }
            default:
                return JS_NULL;
        }
    }
    JSValue ScTileElement::primaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                el->SetPrimaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                el->SetPrimaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                el->SetPrimaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                el->GetBanner()->colour = value;
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::secondaryColour_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                return JS_NewUint32(ctx, el->GetSecondaryColour());
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                return JS_NewUint32(ctx, el->GetSecondaryColour());
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                return JS_NewUint32(ctx, el->GetSecondaryColour());
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                return JS_NewUint32(ctx, EnumValue(el->GetBanner()->textColour));
            }
            default:
                return JS_NULL;
        }
    }
    JSValue ScTileElement::secondaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                el->SetSecondaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                el->SetSecondaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                el->SetSecondaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                el->GetBanner()->textColour = static_cast<Drawing::TextColour>(value);
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::tertiaryColour_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                return JS_NewUint32(ctx, el->GetTertiaryColour());
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                return JS_NewUint32(ctx, el->GetTertiaryColour());
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                return JS_NewUint32(ctx, el->GetTertiaryColour());
            }
            default:
                return JS_NULL;
        }
    }
    JSValue ScTileElement::tertiaryColour_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::SmallScenery:
            {
                auto* el = element->AsSmallScenery();
                el->SetTertiaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                el->SetTertiaryColour(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                el->SetTertiaryColour(value);
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::bannerIndex_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        BannerIndex idx = data->element->GetBannerIndex();
        if (idx == BannerIndex::GetNull())
            return JS_NULL;
        else
            return JS_NewUint32(ctx, idx.ToUnderlying());
    }
    JSValue ScTileElement::bannerIndex_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::LargeScenery:
            {
                auto* el = element->AsLargeScenery();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetBannerIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->SetBannerIndex(BannerIndex::GetNull());
                Invalidate(data);
                break;
            }
            case TileElementType::Wall:
            {
                auto* el = element->AsWall();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetBannerIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->SetBannerIndex(BannerIndex::GetNull());
                Invalidate(data);
                break;
            }
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->SetIndex(BannerIndex::GetNull());
                Invalidate(data);
                break;
            }
            default:
                break;
        }
        return JS_UNDEFINED;
    }

    // Deprecated in favor of separate 'edges' and 'corners' properties,
    // left here to maintain compatibility with older plugins.
    JSValue ScTileElement::edgesAndCorners_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        return JS_NewUint32(ctx, el != nullptr ? el->GetEdgesAndCorners() : 0);
    }
    JSValue ScTileElement::edgesAndCorners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetEdgesAndCorners(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::edges_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->GetEdges());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::edges_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetEdges(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::corners_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->GetCorners());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::corners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetCorners(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::slopeDirection_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->IsSloped())
            return JS_NewUint32(ctx, el->GetSlopeDirection());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::slopeDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->SetSloped(true);
                el->SetSlopeDirection(value);
            }
            else
            {
                el->SetSloped(false);
                el->SetSlopeDirection(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isQueue_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->IsQueue());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isQueue_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetIsQueue(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::queueBannerDirection_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->HasQueueBanner())
            return JS_NewUint32(ctx, el->GetQueueBannerDirection());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::queueBannerDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->SetHasQueueBanner(true);
                el->SetQueueBannerDirection(value);
            }
            else
            {
                el->SetHasQueueBanner(false);
                el->SetQueueBannerDirection(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isBlockedByVehicle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->IsBlockedByVehicle());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isBlockedByVehicle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetIsBlockedByVehicle(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isWide_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->IsWide());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isWide_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            el->SetWide(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::surfaceObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        if (element->GetType() == TileElementType::Path)
        {
            auto* el = element->AsPath();
            auto index = el->GetSurfaceEntryIndex();
            if (index != kObjectEntryIndexNull)
            {
                return JS_NewUint32(ctx, index);
            }
        }
        return JS_NULL;
    }

    JSValue ScTileElement::surfaceObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto element = data->element;
            if (element->GetType() == TileElementType::Path)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsPath();
                el->SetSurfaceEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::railingsObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        if (element->GetType() == TileElementType::Path)
        {
            auto* el = element->AsPath();
            auto index = el->GetRailingsEntryIndex();
            if (index != kObjectEntryIndexNull)
            {
                return JS_NewUint32(ctx, index);
            }
        }
        return JS_NULL;
    }

    JSValue ScTileElement::railingsObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto element = data->element;
            if (element->GetType() == TileElementType::Path)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->AsPath();
                el->SetRailingsEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::addition_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->HasAddition())
            return JS_NewUint32(ctx, el->GetAdditionEntryIndex());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::addition_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(addition, ctx, jsValue);
                if (addition <= 254)
                {
                    el->SetAdditionEntryIndex(addition);
                }
            }
            else
            {
                el->SetAddition(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::additionStatus_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->HasAddition() && !el->IsQueue())
            return JS_NewUint32(ctx, el->GetAdditionStatus());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::additionStatus_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->AsPath();
            if (el != nullptr)
            {
                if (el->HasAddition() && !el->IsQueue())
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->SetAdditionStatus(value);
                    Invalidate(data);
                }
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isAdditionBroken_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->HasAddition())
            return JS_NewBool(ctx, el->IsBroken());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isAdditionBroken_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsBool(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->AsPath();
            if (el != nullptr)
            {
                JS_UNPACK_BOOL(value, ctx, jsValue);
                el->SetIsBroken(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isAdditionGhost_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsPath();
        if (el != nullptr && el->HasAddition())
            return JS_NewBool(ctx, el->AdditionIsGhost());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isAdditionGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsBool(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->AsPath();
            if (el != nullptr)
            {
                JS_UNPACK_BOOL(value, ctx, jsValue);
                el->SetAdditionIsGhost(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::footpathObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsEntrance();
        if (el != nullptr)
        {
            auto index = el->GetLegacyPathEntryIndex();
            if (index != kObjectEntryIndexNull)
            {
                return JS_NewUint32(ctx, index);
            }
        }
        return JS_NULL;
    }
    JSValue ScTileElement::footpathObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->AsEntrance();
            if (el != nullptr)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->SetLegacyPathEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::footpathSurfaceObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsEntrance();
        if (el != nullptr)
        {
            auto index = el->GetSurfaceEntryIndex();
            if (index != kObjectEntryIndexNull)
            {
                return JS_NewUint32(ctx, index);
            }
        }
        return JS_NULL;
    }

    JSValue ScTileElement::footpathSurfaceObject_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->AsEntrance();
            if (el != nullptr)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->SetSurfaceEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::direction_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                return JS_NewUint32(ctx, el->GetPosition());
            }
            case TileElementType::Path:
            case TileElementType::Surface:
            {
                return JS_NULL;
            }
            default:
            {
                return JS_NewUint32(ctx, element->GetDirection());
            }
        }
    }
    JSValue ScTileElement::direction_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->GetType())
        {
            case TileElementType::Banner:
            {
                auto* el = element->AsBanner();
                el->SetPosition(value);
                Invalidate(data);
                break;
            }
            case TileElementType::Path:
            case TileElementType::Surface:
            {
                break;
            }
            default:
            {
                element->SetDirection(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::bannerText_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        BannerIndex idx = data->element->GetBannerIndex();
        if (idx == BannerIndex::GetNull())
            return JS_NULL;
        else
            return JSFromStdString(ctx, GetBanner(idx)->getText());
    }
    JSValue ScTileElement::bannerText_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_STR(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        BannerIndex idx = element->GetBannerIndex();
        if (idx != BannerIndex::GetNull())
        {
            auto banner = GetBanner(idx);
            banner->text = value;
            if (element->GetType() != TileElementType::Banner)
            {
                if (value.empty())
                    banner->rideIndex = BannerGetClosestRideIndex({ banner->position.ToCoordsXY(), 16 });
                else
                    banner->rideIndex = RideId::GetNull();

                banner->flags.set(BannerFlag::linkedToRide, !banner->rideIndex.IsNull());
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isNoEntry_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsBanner();
        if (el != nullptr)
            return JS_NewBool(ctx, el->GetBanner()->flags.has(BannerFlag::noEntry));
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isNoEntry_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->AsBanner();
        if (el != nullptr)
        {
            el->GetBanner()->flags.set(BannerFlag::noEntry, value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    const LargeSceneryElement* ScTileElement::GetOtherLargeSceneryElement(
        const CoordsXY& loc, const LargeSceneryElement* const largeScenery)
    {
        const auto* const largeEntry = largeScenery->GetEntry();
        const auto direction = largeScenery->GetDirection();
        const auto sequenceIndex = largeScenery->GetSequenceIndex();
        const auto& tiles = largeEntry->tiles;
        const auto& initialTile = tiles[sequenceIndex];
        const auto rotatedFirstTile = CoordsXYZ{
            CoordsXY{ initialTile.offset }.Rotate(direction),
            initialTile.offset.z,
        };

        const auto firstTile = CoordsXYZ{ loc, largeScenery->GetBaseZ() } - rotatedFirstTile;
        for (auto& tile : tiles)
        {
            const auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(direction), tile.offset.z };

            const auto currentTile = firstTile + rotatedCurrentTile;

            const TileElement* tileElement = MapGetFirstElementAt(currentTile);
            if (tileElement != nullptr)
            {
                do
                {
                    if (tileElement->GetType() != TileElementType::LargeScenery)
                        continue;
                    if (tileElement->GetDirection() != direction)
                        continue;
                    if (tileElement->GetBaseZ() != currentTile.z)
                        continue;

                    if (tileElement->AsLargeScenery() == largeScenery)
                        continue;
                    if (tileElement->AsLargeScenery()->GetEntryIndex() != largeScenery->GetEntryIndex())
                        continue;
                    if (tileElement->AsLargeScenery()->GetSequenceIndex() != tile.index)
                        continue;

                    return tileElement->AsLargeScenery();
                } while (!(tileElement++)->IsLastForTile());
            }
        }
        return nullptr;
    }

    void ScTileElement::RemoveBannerEntryIfNeeded(TileElement* element, CoordsXY& coords)
    {
        // check if other element still uses the banner entry
        if (element->GetType() == TileElementType::LargeScenery
            && element->AsLargeScenery()->GetEntry()->scrolling_mode != kScrollingModeNone
            && GetOtherLargeSceneryElement(coords, element->AsLargeScenery()) != nullptr)
            return;
        // remove banner entry (if one exists)
        element->RemoveBannerEntry();
    }

    void ScTileElement::CreateBannerEntryIfNeeded(TileElement* element, CoordsXY& coords)
    {
        // check if creation is needed
        switch (element->GetType())
        {
            case TileElementType::Banner:
                break;
            case TileElementType::Wall:
            {
                auto wallEntry = element->AsWall()->GetEntry();
                if (wallEntry == nullptr || wallEntry->scrolling_mode == kScrollingModeNone)
                    return;
                break;
            }
            case TileElementType::LargeScenery:
            {
                auto largeScenery = element->AsLargeScenery();
                auto largeSceneryEntry = largeScenery->GetEntry();
                if (largeSceneryEntry == nullptr || largeSceneryEntry->scrolling_mode == kScrollingModeNone)
                    return;

                auto otherElement = GetOtherLargeSceneryElement(coords, largeScenery);
                if (otherElement != nullptr)
                {
                    largeScenery->SetBannerIndex(otherElement->GetBannerIndex());
                    return;
                }

                break;
            }
            default:
                return;
        }

        // create banner entry and initialise it
        auto* banner = CreateBanner();
        if (banner == nullptr)
            GetContext()->GetScriptEngine().LogPluginInfo("No free banners available.");
        else
        {
            banner->text = {};
            banner->colour = 0;
            banner->textColour = Drawing::TextColour::black;
            banner->flags = {};
            if (element->GetType() == TileElementType::Wall)
                banner->flags.set(BannerFlag::isWall);
            if (element->GetType() == TileElementType::LargeScenery)
                banner->flags.set(BannerFlag::isLargeScenery);
            banner->type = 0;
            banner->position = TileCoordsXY(coords);

            if (element->GetType() == TileElementType::Wall || element->GetType() == TileElementType::LargeScenery)
            {
                RideId rideIndex = BannerGetClosestRideIndex({ coords, element->BaseHeight });
                if (!rideIndex.IsNull())
                {
                    banner->rideIndex = rideIndex;
                    banner->flags.set(BannerFlag::linkedToRide);
                }
            }

            element->SetBannerIndex(banner->id);
        }
    }

    TileElement* ScTileElement::GetTileElement(JSValue thisValue)
    {
        return gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue)->element;
    }

    JSValue ScTileElement::New(JSContext* ctx, TileElement* element, CoordsXY& coords)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            // All
            JS_CGETSET_DEF("type", ScTileElement::type_get, ScTileElement::type_set),
            JS_CGETSET_DEF("baseHeight", ScTileElement::baseHeight_get, ScTileElement::baseHeight_set),
            JS_CGETSET_DEF("baseZ", ScTileElement::baseZ_get, ScTileElement::baseZ_set),
            JS_CGETSET_DEF("clearanceHeight", ScTileElement::clearanceHeight_get, ScTileElement::clearanceHeight_set),
            JS_CGETSET_DEF("clearanceZ", ScTileElement::clearanceZ_get, ScTileElement::clearanceZ_set),
            JS_CGETSET_DEF("occupiedQuadrants", ScTileElement::occupiedQuadrants_get, ScTileElement::occupiedQuadrants_set),
            JS_CGETSET_DEF("isGhost", ScTileElement::isGhost_get, ScTileElement::isGhost_set),
            JS_CGETSET_DEF("isHidden", ScTileElement::isHidden_get, ScTileElement::isHidden_set),

            // Track | Small Scenery | Wall | Entrance | Large Scenery | Banner
            JS_CGETSET_DEF("direction", ScTileElement::direction_get, ScTileElement::direction_set),

            // Path | Small Scenery | Wall | Entrance | Large Scenery | Banner
            JS_CGETSET_DEF("object", ScTileElement::object_get, ScTileElement::object_set),

            // Small Scenery | Wall | Large Scenery | Banner
            JS_CGETSET_DEF("primaryColour", ScTileElement::primaryColour_get, ScTileElement::primaryColour_set),
            JS_CGETSET_DEF("secondaryColour", ScTileElement::secondaryColour_get, ScTileElement::secondaryColour_set),

            // Small Scenery | Wall | Large Scenery
            JS_CGETSET_DEF("tertiaryColour", ScTileElement::tertiaryColour_get, ScTileElement::tertiaryColour_set),

            // Wall | Large Scenery | Banner
            JS_CGETSET_DEF("bannerText", ScTileElement::bannerText_get, ScTileElement::bannerText_set),
            JS_CGETSET_DEF("bannerIndex", ScTileElement::bannerIndex_get, ScTileElement::bannerIndex_set),

            // Path | Track | Entrance
            JS_CGETSET_DEF("ride", ScTileElement::ride_get, ScTileElement::ride_set),
            JS_CGETSET_DEF("station", ScTileElement::station_get, ScTileElement::station_set),

            // Track | Entrance | Large Scenery
            JS_CGETSET_DEF("sequence", ScTileElement::sequence_get, ScTileElement::sequence_set),

            // Surface | Wall
            JS_CGETSET_DEF("slope", ScTileElement::slope_get, ScTileElement::slope_set),

            // Surface only
            JS_CGETSET_DEF("waterHeight", ScTileElement::waterHeight_get, ScTileElement::waterHeight_set),
            JS_CGETSET_DEF("surfaceStyle", ScTileElement::surfaceStyle_get, ScTileElement::surfaceStyle_set),
            JS_CGETSET_DEF("edgeStyle", ScTileElement::edgeStyle_get, ScTileElement::edgeStyle_set),
            JS_CGETSET_DEF("grassLength", ScTileElement::grassLength_get, ScTileElement::grassLength_set),
            JS_CGETSET_DEF("hasOwnership", ScTileElement::hasOwnership_get, nullptr),
            JS_CGETSET_DEF("hasConstructionRights", ScTileElement::hasConstructionRights_get, nullptr),
            JS_CGETSET_DEF("ownership", ScTileElement::ownership_get, ScTileElement::ownership_set),
            JS_CGETSET_DEF("parkFences", ScTileElement::parkFences_get, ScTileElement::parkFences_set),

            // Footpath only
            JS_CGETSET_DEF("edgesAndCorners", ScTileElement::edgesAndCorners_get, ScTileElement::edgesAndCorners_set),
            JS_CGETSET_DEF("edges", ScTileElement::edges_get, ScTileElement::edges_set),
            JS_CGETSET_DEF("corners", ScTileElement::corners_get, ScTileElement::corners_set),
            JS_CGETSET_DEF("slopeDirection", ScTileElement::slopeDirection_get, ScTileElement::slopeDirection_set),
            JS_CGETSET_DEF("isQueue", ScTileElement::isQueue_get, ScTileElement::isQueue_set),
            JS_CGETSET_DEF(
                "queueBannerDirection", ScTileElement::queueBannerDirection_get, ScTileElement::queueBannerDirection_set),

            JS_CGETSET_DEF("isBlockedByVehicle", ScTileElement::isBlockedByVehicle_get, ScTileElement::isBlockedByVehicle_set),
            JS_CGETSET_DEF("isWide", ScTileElement::isWide_get, ScTileElement::isWide_set),

            JS_CGETSET_DEF("surfaceObject", ScTileElement::surfaceObject_get, ScTileElement::surfaceObject_set),
            JS_CGETSET_DEF("railingsObject", ScTileElement::railingsObject_get, ScTileElement::railingsObject_set),

            JS_CGETSET_DEF("addition", ScTileElement::addition_get, ScTileElement::addition_set),
            JS_CGETSET_DEF("additionStatus", ScTileElement::additionStatus_get, ScTileElement::additionStatus_set),
            JS_CGETSET_DEF("isAdditionBroken", ScTileElement::isAdditionBroken_get, ScTileElement::isAdditionBroken_set),
            JS_CGETSET_DEF("isAdditionGhost", ScTileElement::isAdditionGhost_get, ScTileElement::isAdditionGhost_set),

            // Track only
            JS_CGETSET_DEF("trackType", ScTileElement::trackType_get, ScTileElement::trackType_set),
            JS_CGETSET_DEF("rideType", ScTileElement::rideType_get, ScTileElement::rideType_set),
            JS_CGETSET_DEF("mazeEntry", ScTileElement::mazeEntry_get, ScTileElement::mazeEntry_set),
            JS_CGETSET_DEF("colourScheme", ScTileElement::colourScheme_get, ScTileElement::colourScheme_set),
            JS_CGETSET_DEF("seatRotation", ScTileElement::seatRotation_get, ScTileElement::seatRotation_set),
            JS_CGETSET_DEF("brakeBoosterSpeed", ScTileElement::brakeBoosterSpeed_get, ScTileElement::brakeBoosterSpeed_set),
            JS_CGETSET_DEF("hasChainLift", ScTileElement::hasChainLift_get, ScTileElement::hasChainLift_set),
            JS_CGETSET_DEF("isInverted", ScTileElement::isInverted_get, ScTileElement::isInverted_set),
            JS_CGETSET_DEF("hasCableLift", ScTileElement::hasCableLift_get, ScTileElement::hasCableLift_set),
            JS_CGETSET_DEF("isHighlighted", ScTileElement::isHighlighted_get, ScTileElement::isHighlighted_set),

            // Small Scenery only
            JS_CGETSET_DEF("age", ScTileElement::age_get, ScTileElement::age_set),
            JS_CGETSET_DEF("quadrant", ScTileElement::quadrant_get, ScTileElement::quadrant_set),

            // Entrance only
            JS_CGETSET_DEF("footpathObject", ScTileElement::footpathObject_get, ScTileElement::footpathObject_set),
            JS_CGETSET_DEF(
                "footpathSurfaceObject", ScTileElement::footpathSurfaceObject_get, ScTileElement::footpathSurfaceObject_set),

            // Banner only
            JS_CGETSET_DEF("isNoEntry", ScTileElement::isNoEntry_get, ScTileElement::isNoEntry_set)
        };
        return MakeWithOpaque(ctx, funcs, new OpaqueTileElementData{ element, coords });
    }

    void ScTileElement::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "TileElement", Finalize);
    }

    void ScTileElement::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueTileElementData* data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisVal);
        if (data)
            delete data;
    }

} // namespace OpenRCT2::Scripting

#endif
