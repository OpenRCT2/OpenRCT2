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
    #include "../../../drawing/TextColour.h"
    #include "../../../object/PathAdditionEntry.h"
    #include "../../../object/WallSceneryEntry.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideData.h"
    #include "../../../world/Map.h"
    #include "../../../world/tile_element/BannerElement.h"
    #include "../../../world/tile_element/EntranceElement.h"
    #include "../../../world/tile_element/LargeSceneryElement.h"
    #include "../../../world/tile_element/PathElement.h"
    #include "../../../world/tile_element/SmallSceneryElement.h"
    #include "../../../world/tile_element/SurfaceElement.h"
    #include "../../../world/tile_element/TrackElement.h"
    #include "../../../world/tile_element/WallElement.h"

namespace OpenRCT2::Scripting
{
    static inline std::string TileElementTypeToString(const TileElement* element)
    {
        switch (element->getType())
        {
            case TileElementType::surface:
                return "surface";
            case TileElementType::path:
                return "footpath";
            case TileElementType::track:
                return "track";
            case TileElementType::smallScenery:
                return "small_scenery";
            case TileElementType::entrance:
                return "entrance";
            case TileElementType::wall:
                return "wall";
            case TileElementType::largeScenery:
                return "large_scenery";
            case TileElementType::banner:
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
            element->setType(TileElementType::surface);
        else if (value == "footpath")
            element->setType(TileElementType::path);
        else if (value == "track")
            element->setType(TileElementType::track);
        else if (value == "small_scenery")
            element->setType(TileElementType::smallScenery);
        else if (value == "entrance")
            element->setType(TileElementType::entrance);
        else if (value == "wall")
            element->setType(TileElementType::wall);
        else if (value == "large_scenery")
            element->setType(TileElementType::largeScenery);
        else if (value == "banner")
            element->setType(TileElementType::banner);
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
        return JS_NewUint32(ctx, element->baseHeight);
    }
    JSValue ScTileElement::baseHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(newBaseHeight, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->baseHeight = newBaseHeight;
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::baseZ_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->getBaseZ());
    }
    JSValue ScTileElement::baseZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->setBaseZ(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::clearanceHeight_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->clearanceHeight);
    }
    JSValue ScTileElement::clearanceHeight_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(newClearanceHeight, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->clearanceHeight = newClearanceHeight;
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::clearanceZ_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        return JS_NewUint32(ctx, element->getClearanceZ());
    }
    JSValue ScTileElement::clearanceZ_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->setClearanceZ(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::slope_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::surface:
            {
                auto* el = element->asSurface();
                return JS_NewUint32(ctx, el->getSlope());
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                return JS_NewUint32(ctx, el->getSlope());
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
        const auto type = element->getType();

        if (type == TileElementType::surface)
        {
            auto* el = element->asSurface();
            el->setSlope(value);
            Invalidate(data);
        }
        else if (type == TileElementType::wall)
        {
            auto* el = element->asWall();
            el->setSlope(value);
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
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewInt32(ctx, el->getWaterHeight());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'waterHeight' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->setWaterHeight(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::surfaceStyle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getSurfaceObjectIndex());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'surfaceStyle' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->setSurfaceObjectIndex(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::edgeStyle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getEdgeObjectIndex());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'edgeStyle' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->setEdgeObjectIndex(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::grassLength_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getGrassLength());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'grassLength' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
        el->setGrassLengthAndInvalidate(value, data->coords);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::hasOwnership_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->getOwnership() & OWNERSHIP_OWNED);
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
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            auto ownership = el->getOwnership();
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
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getOwnership());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'ownership' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->setOwnership(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::parkFences_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSurface();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getParkFences());
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
        auto* el = data->element->asSurface();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'parkFences' property, tile element is not a SurfaceElement.");
            return JS_UNDEFINED;
        }

        el->setParkFences(value);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::trackType_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, EnumValue(el->getTrackType()));
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'trackType' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->setTrackType(static_cast<TrackElemType>(value));
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::rideType_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el != nullptr)
        {
            return JS_NewUint32(ctx, el->getRideType());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'rideType' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->setRideType(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::sequence_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                return JS_NewUint32(ctx, el->getSequenceIndex());
            }
            case TileElementType::track:
            {
                auto* el = element->asTrack();
                auto* ride = GetRide(el->getRideIndex());

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

                return JS_NewUint32(ctx, el->getSequenceIndex());
            }
            case TileElementType::entrance:
            {
                auto* el = element->asEntrance();
                return JS_NewUint32(ctx, EnumValue(el->getSequenceIndex()));
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

        switch (element->getType())
        {
            case TileElementType::largeScenery:
            {
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->asLargeScenery();
                el->setSequenceIndex(value);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::track:
            {
                auto* el = element->asTrack();
                auto ride = GetRide(el->getRideIndex());

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

                el->setSequenceIndex(value);
                Invalidate(data);
                break;
            }
            case TileElementType::entrance:
            {
                auto* el = element->asEntrance();
                el->setSequenceIndex(static_cast<ParkEntranceSequence>(std::clamp<uint8_t>(value, 0, 2)));
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
        switch (element->getType())
        {
            case TileElementType::path:
            {
                auto* el = element->asPath();
                if (!el->isQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'ride' property, path is not a queue.");
                    return JS_NULL;
                }

                if (!el->getRideIndex().IsNull())
                    return JS_NewUint32(ctx, el->getRideIndex().ToUnderlying());

                return JS_NULL;
            }
            case TileElementType::track:
            {
                auto* el = element->asTrack();
                return JS_NewUint32(ctx, el->getRideIndex().ToUnderlying());
            }
            case TileElementType::entrance:
            {
                auto* el = element->asEntrance();
                return JS_NewUint32(ctx, el->getRideIndex().ToUnderlying());
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

        switch (element->getType())
        {
            case TileElementType::path:
            {
                auto* el = element->asPath();
                if (!el->isQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot set ride property, path is not a queue.");
                    return JS_UNDEFINED;
                }

                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setRideIndex(RideId::FromUnderlying(value));
                }
                else if (JS_IsNull(jsValue))
                {
                    el->setRideIndex(RideId::GetNull());
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
            case TileElementType::track:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'ride' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asTrack();
                el->setRideIndex(RideId::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            case TileElementType::entrance:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'ride' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asEntrance();
                el->setRideIndex(RideId::FromUnderlying(value));
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
        switch (element->getType())
        {
            case TileElementType::path:
            {
                auto* el = element->asPath();
                if (!el->isQueue())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, path is not a queue.");
                    return JS_NULL;
                }

                if (el->getRideIndex().IsNull())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, queue is not linked to a ride.");
                    return JS_NULL;
                }

                if (!el->getStationIndex().IsNull())
                    return JS_NewUint32(ctx, el->getStationIndex().ToUnderlying());

                return JS_NULL;
            }
            case TileElementType::track:
            {
                auto* el = element->asTrack();
                if (!el->isStation())
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("Cannot read 'station' property, track is not a station.");
                    return JS_NULL;
                }

                return JS_NewUint32(ctx, el->getStationIndex().ToUnderlying());
            }
            case TileElementType::entrance:
            {
                auto* el = element->asEntrance();
                return JS_NewUint32(ctx, el->getStationIndex().ToUnderlying());
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

        switch (element->getType())
        {
            case TileElementType::path:
            {
                auto* el = element->asPath();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setStationIndex(StationIndex::FromUnderlying(value));
                }
                else if (JS_IsNull(jsValue))
                {
                    el->setStationIndex(StationIndex::GetNull());
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
            case TileElementType::track:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'station' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asTrack();
                el->setStationIndex(StationIndex::FromUnderlying(value));
                Invalidate(data);
                break;
            }
            case TileElementType::entrance:
            {
                if (!JS_IsNumber(jsValue))
                {
                    auto& scriptEngine = GetContext()->GetScriptEngine();
                    scriptEngine.LogPluginInfo("'station' must be a number.");
                    return JS_UNDEFINED;
                }

                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asEntrance();
                el->setStationIndex(StationIndex::FromUnderlying(value));
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
        auto* el = data->element->asTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->hasChain());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasChainLift' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->setHasChain(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::mazeEntry_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'mazeEntry' property, element is not a TrackElement.");
            return JS_NULL;
        }

        Ride* ride = GetRide(el->getRideIndex());
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

        return JS_NewUint32(ctx, el->getMazeEntry());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'mazeEntry' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->getRideIndex());
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
        el->setMazeEntry(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::colourScheme_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'colourScheme' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        auto* ride = GetRide(el->getRideIndex());
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

        return JS_NewUint32(ctx, el->getColourScheme());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'colourScheme' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->getRideIndex());
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
        el->setColourScheme(static_cast<RideColourScheme>(value));
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::seatRotation_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'seatRotation' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        auto* ride = GetRide(el->getRideIndex());
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

        return JS_NewUint32(ctx, el->getSeatRotation());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'seatRotation' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        auto* ride = GetRide(el->getRideIndex());
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
        el->setSeatRotation(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::brakeBoosterSpeed_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'brakeBoosterSpeed' property, tile element is not a TrackElement.");
            return JS_NULL;
        }

        if (!trackTypeHasSpeedSetting(el->getTrackType()))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot read 'brakeBoosterSpeed' property, track element has no speed setting.");
            return JS_NULL;
        }

        return JS_NewUint32(ctx, el->getBrakeBoosterSpeed());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'brakeBoosterSpeed' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        if (!trackTypeHasSpeedSetting(el->getTrackType()))
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'brakeBoosterSpeed' property, track element has no speed setting.");
            return JS_UNDEFINED;
        }

        JS_UNPACK_UINT32(value, ctx, jsValue);
        el->setBrakeBoosterSpeed(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isInverted_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->isInverted());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'isInverted' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->setInverted(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::hasCableLift_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asTrack();
        if (el != nullptr)
        {
            return JS_NewBool(ctx, el->hasCableLift());
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
        auto* el = data->element->asTrack();
        if (el == nullptr)
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.LogPluginInfo("Cannot set 'hasCableLift' property, tile element is not a TrackElement.");
            return JS_UNDEFINED;
        }

        el->setHasCableLift(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isHighlighted_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto el = data->element->asTrack();
        if (el != nullptr)
            return JS_NewBool(ctx, el->isHighlighted());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isHighlighted_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto el = data->element->asTrack();
        if (el != nullptr)
        {
            el->setHighlight(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::object_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::path:
            {
                auto* el = element->asPath();
                auto index = el->getLegacyPathEntryIndex();
                if (index != kObjectEntryIndexNull)
                    return JS_NewUint32(ctx, index);

                return JS_NULL;
            }
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                return JS_NewUint32(ctx, el->getEntryIndex());
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                return JS_NewUint32(ctx, el->getEntryIndex());
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                return JS_NewUint32(ctx, el->getEntryIndex());
            }
            case TileElementType::entrance:
            {
                auto* el = element->asEntrance();
                return JS_NewUint32(ctx, EnumValue(el->getEntranceType()));
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                return JS_NewUint32(ctx, el->getBanner()->type);
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

        switch (element->getType())
        {
            case TileElementType::path:
            {
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(index, ctx, jsValue);
                    auto* el = element->asPath();
                    el->setLegacyPathEntryIndex(index);
                    Invalidate(data);
                }
                break;
            }
            case TileElementType::smallScenery:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->asSmallScenery();
                el->setEntryIndex(index);
                Invalidate(data);
                break;
            }
            case TileElementType::largeScenery:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->asLargeScenery();
                el->setEntryIndex(index);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::wall:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                RemoveBannerEntryIfNeeded(element, data->coords);
                auto* el = element->asWall();
                el->setEntryIndex(index);
                CreateBannerEntryIfNeeded(element, data->coords);
                Invalidate(data);
                break;
            }
            case TileElementType::entrance:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->asEntrance();
                index = std::clamp<uint32_t>(index, 0, 2);
                el->setEntranceType(static_cast<EntranceType>(index));
                Invalidate(data);
                break;
            }
            case TileElementType::banner:
            {
                JS_UNPACK_UINT32(index, ctx, jsValue);
                auto* el = element->asBanner();
                el->getBanner()->type = index;
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
        return JS_NewBool(ctx, data->element->isInvisible());
    }

    JSValue ScTileElement::isHidden_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(hide, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->setInvisible(hide);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::age_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSmallScenery();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->getAge());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::age_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSmallScenery();
        if (el != nullptr)
        {
            el->setAge(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::quadrant_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSmallScenery();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->getSceneryQuadrant());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::quadrant_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asSmallScenery();
        if (el != nullptr)
        {
            el->setSceneryQuadrant(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::occupiedQuadrants_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        return JS_NewUint32(ctx, data->element->getOccupiedQuadrants());
    }
    JSValue ScTileElement::occupiedQuadrants_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->setOccupiedQuadrants(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isGhost_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        return JS_NewBool(ctx, data->element->isGhost());
    }
    JSValue ScTileElement::isGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        data->element->setGhost(value);
        Invalidate(data);
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::primaryColour_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                return JS_NewUint32(ctx, EnumValue(el->getPrimaryColour()));
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                return JS_NewUint32(ctx, EnumValue(el->getPrimaryColour()));
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                return JS_NewUint32(ctx, EnumValue(el->getPrimaryColour()));
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                return JS_NewUint32(ctx, EnumValue(el->getBanner()->colour));
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
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                el->setPrimaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                el->setPrimaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                el->setPrimaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                el->getBanner()->colour = static_cast<Drawing::Colour>(value);
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
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                return JS_NewUint32(ctx, EnumValue(el->getSecondaryColour()));
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                return JS_NewUint32(ctx, EnumValue(el->getSecondaryColour()));
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                return JS_NewUint32(ctx, EnumValue(el->getSecondaryColour()));
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                return JS_NewUint32(ctx, EnumValue(el->getBanner()->textColour));
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
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                el->setSecondaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                el->setSecondaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                el->setSecondaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                el->getBanner()->textColour = static_cast<Drawing::TextColour>(value);
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
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                return JS_NewUint32(ctx, EnumValue(el->getTertiaryColour()));
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                return JS_NewUint32(ctx, EnumValue(el->getTertiaryColour()));
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                return JS_NewUint32(ctx, EnumValue(el->getTertiaryColour()));
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
        switch (element->getType())
        {
            case TileElementType::smallScenery:
            {
                auto* el = element->asSmallScenery();
                el->setTertiaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                el->setTertiaryColour(static_cast<Drawing::Colour>(value));
                Invalidate(data);
                break;
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                el->setTertiaryColour(static_cast<Drawing::Colour>(value));
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
        BannerIndex idx = data->element->getBannerIndex();
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
        switch (element->getType())
        {
            case TileElementType::largeScenery:
            {
                auto* el = element->asLargeScenery();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setBannerIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->setBannerIndex(BannerIndex::GetNull());
                Invalidate(data);
                break;
            }
            case TileElementType::wall:
            {
                auto* el = element->asWall();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setBannerIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->setBannerIndex(BannerIndex::GetNull());
                Invalidate(data);
                break;
            }
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                if (JS_IsNumber(jsValue))
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setIndex(BannerIndex::FromUnderlying(value));
                }
                else
                    el->setIndex(BannerIndex::GetNull());
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
        auto* el = data->element->asPath();
        return JS_NewUint32(ctx, el != nullptr ? el->getEdgesAndCorners() : 0);
    }
    JSValue ScTileElement::edgesAndCorners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setEdgesAndCorners(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::edges_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->getEdges());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::edges_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setEdges(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::corners_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
            return JS_NewUint32(ctx, el->getCorners());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::corners_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setCorners(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::slopeDirection_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->isSloped())
            return JS_NewUint32(ctx, el->getSlopeDirection());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::slopeDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->setSloped(true);
                el->setSlopeDirection(value);
            }
            else
            {
                el->setSloped(false);
                el->setSlopeDirection(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isQueue_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->isQueue());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isQueue_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setIsQueue(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::queueBannerDirection_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->hasQueueBanner())
            return JS_NewUint32(ctx, el->getQueueBannerDirection());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::queueBannerDirection_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->setHasQueueBanner(true);
                el->setQueueBannerDirection(value);
            }
            else
            {
                el->setHasQueueBanner(false);
                el->setQueueBannerDirection(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isBlockedByVehicle_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->isBlockedByVehicle());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isBlockedByVehicle_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setIsBlockedByVehicle(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isWide_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
            return JS_NewBool(ctx, el->isWide());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isWide_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            el->setWide(value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::surfaceObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        if (element->getType() == TileElementType::path)
        {
            auto* el = element->asPath();
            auto index = el->getSurfaceEntryIndex();
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
            if (element->getType() == TileElementType::path)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asPath();
                el->setSurfaceEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::railingsObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        if (element->getType() == TileElementType::path)
        {
            auto* el = element->asPath();
            auto index = el->getRailingsEntryIndex();
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
            if (element->getType() == TileElementType::path)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                auto* el = element->asPath();
                el->setRailingsEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::addition_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->hasAddition())
            return JS_NewUint32(ctx, el->getAdditionEntryIndex());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::addition_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr)
        {
            if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(addition, ctx, jsValue);
                if (addition <= 254)
                {
                    el->setAdditionEntryIndex(addition);
                }
            }
            else
            {
                el->setAddition(0);
            }
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::additionStatus_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->hasAddition() && !el->isQueue())
            return JS_NewUint32(ctx, el->getAdditionStatus());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::additionStatus_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsNumber(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->asPath();
            if (el != nullptr)
            {
                if (el->hasAddition() && !el->isQueue())
                {
                    JS_UNPACK_UINT32(value, ctx, jsValue);
                    el->setAdditionStatus(value);
                    Invalidate(data);
                }
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isAdditionBroken_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->hasAddition())
            return JS_NewBool(ctx, el->isBroken());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isAdditionBroken_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsBool(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->asPath();
            if (el != nullptr)
            {
                JS_UNPACK_BOOL(value, ctx, jsValue);
                el->setIsBroken(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::isAdditionFull_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el == nullptr || !el->hasAddition() || el->isQueue())
            return JS_NULL;

        const auto* additionEntry = el->getAdditionEntry();
        if (additionEntry == nullptr || !(additionEntry->flags & PATH_ADDITION_FLAG_IS_BIN))
            return JS_NULL;

        // Each path edge has a 2-bit slot (0 = full, 3 = empty); a bin is only "full" once a
        // slot on an open edge reaches 0. Matches Staff::updatePatrollingFindBin.
        uint8_t binEdges = el->getEdges();
        uint8_t binStatus = el->getAdditionStatus();
        for (int32_t i = 0; i < 4; ++i)
        {
            if (!(binEdges & 1) && !(binStatus & 3))
                return JS_NewBool(ctx, true);
            binEdges >>= 1;
            binStatus >>= 2;
        }
        return JS_NewBool(ctx, false);
    }

    JSValue ScTileElement::isAdditionGhost_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asPath();
        if (el != nullptr && el->hasAddition())
            return JS_NewBool(ctx, el->additionIsGhost());
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isAdditionGhost_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        if (JS_IsBool(jsValue))
        {
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
            auto* el = data->element->asPath();
            if (el != nullptr)
            {
                JS_UNPACK_BOOL(value, ctx, jsValue);
                el->setAdditionIsGhost(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::footpathObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asEntrance();
        if (el != nullptr)
        {
            auto index = el->getLegacyPathEntryIndex();
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
            auto* el = data->element->asEntrance();
            if (el != nullptr)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->setLegacyPathEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::footpathSurfaceObject_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asEntrance();
        if (el != nullptr)
        {
            auto index = el->getSurfaceEntryIndex();
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
            auto* el = data->element->asEntrance();
            if (el != nullptr)
            {
                JS_UNPACK_UINT32(value, ctx, jsValue);
                el->setSurfaceEntryIndex(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::direction_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                return JS_NewUint32(ctx, el->getPosition());
            }
            case TileElementType::path:
            case TileElementType::surface:
            {
                return JS_NULL;
            }
            default:
            {
                return JS_NewUint32(ctx, element->getDirection());
            }
        }
    }
    JSValue ScTileElement::direction_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto element = data->element;
        switch (element->getType())
        {
            case TileElementType::banner:
            {
                auto* el = element->asBanner();
                el->setPosition(value);
                Invalidate(data);
                break;
            }
            case TileElementType::path:
            case TileElementType::surface:
            {
                break;
            }
            default:
            {
                element->setDirection(value);
                Invalidate(data);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScTileElement::bannerText_get(JSContext* ctx, JSValue thisValue)
    {
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        BannerIndex idx = data->element->getBannerIndex();
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
        BannerIndex idx = element->getBannerIndex();
        if (idx != BannerIndex::GetNull())
        {
            auto banner = GetBanner(idx);
            banner->text = value;
            if (element->getType() != TileElementType::banner)
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
        auto* el = data->element->asBanner();
        if (el != nullptr)
            return JS_NewBool(ctx, el->getBanner()->flags.has(BannerFlag::noEntry));
        else
            return JS_NULL;
    }
    JSValue ScTileElement::isNoEntry_set(JSContext* ctx, JSValue thisValue, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue);
        auto* el = data->element->asBanner();
        if (el != nullptr)
        {
            el->getBanner()->flags.set(BannerFlag::noEntry, value);
            Invalidate(data);
        }
        return JS_UNDEFINED;
    }

    const LargeSceneryElement* ScTileElement::GetOtherLargeSceneryElement(
        const CoordsXY& loc, const LargeSceneryElement* const largeScenery)
    {
        const auto* const largeEntry = largeScenery->getEntry();
        const auto direction = largeScenery->getDirection();
        const auto sequenceIndex = largeScenery->getSequenceIndex();
        const auto& tiles = largeEntry->tiles;
        const auto& initialTile = tiles[sequenceIndex];
        const auto rotatedFirstTile = CoordsXYZ{
            CoordsXY{ initialTile.offset }.Rotate(direction),
            initialTile.offset.z,
        };

        const auto firstTile = CoordsXYZ{ loc, largeScenery->getBaseZ() } - rotatedFirstTile;
        for (auto& tile : tiles)
        {
            const auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(direction), tile.offset.z };

            const auto currentTile = firstTile + rotatedCurrentTile;

            const TileElement* tileElement = MapGetFirstElementAt(currentTile);
            if (tileElement != nullptr)
            {
                do
                {
                    if (tileElement->getType() != TileElementType::largeScenery)
                        continue;
                    if (tileElement->getDirection() != direction)
                        continue;
                    if (tileElement->getBaseZ() != currentTile.z)
                        continue;

                    if (tileElement->asLargeScenery() == largeScenery)
                        continue;
                    if (tileElement->asLargeScenery()->getEntryIndex() != largeScenery->getEntryIndex())
                        continue;
                    if (tileElement->asLargeScenery()->getSequenceIndex() != tile.index)
                        continue;

                    return tileElement->asLargeScenery();
                } while (!(tileElement++)->isLastForTile());
            }
        }
        return nullptr;
    }

    void ScTileElement::RemoveBannerEntryIfNeeded(TileElement* element, CoordsXY& coords)
    {
        // check if other element still uses the banner entry
        if (element->getType() == TileElementType::largeScenery
            && element->asLargeScenery()->getEntry()->scrolling_mode != kScrollingModeNone
            && GetOtherLargeSceneryElement(coords, element->asLargeScenery()) != nullptr)
            return;
        // remove banner entry (if one exists)
        element->removeBannerEntry();
    }

    void ScTileElement::CreateBannerEntryIfNeeded(TileElement* element, CoordsXY& coords)
    {
        // check if creation is needed
        switch (element->getType())
        {
            case TileElementType::banner:
                break;
            case TileElementType::wall:
            {
                auto wallEntry = element->asWall()->getEntry();
                if (wallEntry == nullptr || wallEntry->scrolling_mode == kScrollingModeNone)
                    return;
                break;
            }
            case TileElementType::largeScenery:
            {
                auto largeScenery = element->asLargeScenery();
                auto largeSceneryEntry = largeScenery->getEntry();
                if (largeSceneryEntry == nullptr || largeSceneryEntry->scrolling_mode == kScrollingModeNone)
                    return;

                auto otherElement = GetOtherLargeSceneryElement(coords, largeScenery);
                if (otherElement != nullptr)
                {
                    largeScenery->setBannerIndex(otherElement->getBannerIndex());
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
            banner->colour = Drawing::Colour::black;
            banner->textColour = Drawing::TextColour::black;
            banner->flags = {};
            if (element->getType() == TileElementType::wall)
                banner->flags.set(BannerFlag::isWall);
            if (element->getType() == TileElementType::largeScenery)
                banner->flags.set(BannerFlag::isLargeScenery);
            banner->type = 0;
            banner->position = TileCoordsXY(coords);

            if (element->getType() == TileElementType::wall || element->getType() == TileElementType::largeScenery)
            {
                RideId rideIndex = BannerGetClosestRideIndex({ coords, element->baseHeight });
                if (!rideIndex.IsNull())
                {
                    banner->rideIndex = rideIndex;
                    banner->flags.set(BannerFlag::linkedToRide);
                }
            }

            element->setBannerIndex(banner->id);
        }
    }

    TileElement* ScTileElement::GetTileElement(JSValue thisValue)
    {
        return gScTileElement.GetOpaque<OpaqueTileElementData*>(thisValue)->element;
    }

    JSValue ScTileElement::New(JSContext* ctx, TileElement* element, CoordsXY& coords)
    {
        return MakeWithOpaque(ctx, new OpaqueTileElementData{ element, coords });
    }

    void ScTileElement::Register(JSContext* ctx)
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
            JS_CGETSET_DEF("isAdditionFull", ScTileElement::isAdditionFull_get, nullptr),
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
        RegisterBase(ctx, "TileElement", Finalize, funcs);
    }

    void ScTileElement::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueTileElementData* data = gScTileElement.GetOpaque<OpaqueTileElementData*>(thisVal);
        if (data)
            delete data;
    }

} // namespace OpenRCT2::Scripting

#endif
