/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScRideTypeDescriptor.h"

    #include "../../../core/EnumUtils.hpp"
    #include "../../../ride/RideData.h"
    #include "../../../ride/ted/TrackElementDescriptor.h"
    #include "../../ScriptUtil.hpp"
    #include "ScTrackSegment.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::TrackMetadata;

namespace
{
    const RideTypeDescriptor& GetDescriptor(ride_type_t rideType)
    {
        return GetRideTypeDescriptor(rideType);
    }

    const TrackDrawerEntry& GetRegularTrackDrawerEntry(ride_type_t rideType)
    {
        return GetDescriptor(rideType).TrackPaintFunctions.Regular;
    }

    JSValue TrackGroupSetToArray(JSContext* ctx, const RideTrackGroups& groups)
    {
        auto result = JS_NewArray(ctx);
        int64_t index = 0;
        for (uint8_t i = 0; i < EnumValue(TrackGroup::count); i++)
        {
            if (groups.get(i))
            {
                JS_SetPropertyInt64(ctx, result, index++, JS_NewInt32(ctx, i));
            }
        }
        return result;
    }

    JSValue TrackSegmentsForGroupsToArray(JSContext* ctx, const RideTrackGroups& groups)
    {
        auto result = JS_NewArray(ctx);
        int64_t index = 0;
        for (uint16_t type = 0; type < EnumValue(TrackElemType::count); type++)
        {
            auto trackType = static_cast<TrackElemType>(type);
            const auto& ted = GetTrackElementDescriptor(trackType);
            if (groups.get(EnumValue(ted.definition.group)))
            {
                JS_SetPropertyInt64(ctx, result, index++, gScTrackSegment.New(ctx, trackType));
            }
        }
        return result;
    }

    std::string_view RideCategoryToString(RideCategory category)
    {
        switch (category)
        {
            case RideCategory::transport:
                return "transport";
            case RideCategory::gentle:
                return "gentle";
            case RideCategory::rollerCoaster:
                return "rollercoaster";
            case RideCategory::thrill:
                return "thrill";
            case RideCategory::water:
                return "water";
            case RideCategory::shop:
                return "shop";
            case RideCategory::none:
                return "none";
        }
        return "none";
    }
} // namespace

void ScRideTypeDescriptor::Register(JSContext* ctx)
{
    static constexpr JSCFunctionListEntry funcs[] = {
        JS_CGETSET_DEF("rideType", ScRideTypeDescriptor::rideType_get, nullptr),
        JS_CGETSET_DEF("name", ScRideTypeDescriptor::name_get, nullptr),
        JS_CGETSET_DEF("category", ScRideTypeDescriptor::category_get, nullptr),
        JS_CGETSET_DEF("startTrackSegment", ScRideTypeDescriptor::startTrackSegment_get, nullptr),
        JS_CGETSET_DEF("standardTrackSegments", ScRideTypeDescriptor::standardTrackSegments_get, nullptr),
        JS_CGETSET_DEF("extraTrackSegments", ScRideTypeDescriptor::extraTrackSegments_get, nullptr),
        JS_CGETSET_DEF("standardTrackGroups", ScRideTypeDescriptor::standardTrackGroups_get, nullptr),
        JS_CGETSET_DEF("extraTrackGroups", ScRideTypeDescriptor::extraTrackGroups_get, nullptr),
    };
    RegisterBase(ctx, "RideTypeDescriptor", Finalize, funcs);
}

JSValue ScRideTypeDescriptor::New(JSContext* ctx, ride_type_t rideType)
{
    return MakeWithOpaque(ctx, new RideTypeDescriptorData{ rideType });
}

void ScRideTypeDescriptor::Finalize(JSRuntime* rt, JSValue thisVal)
{
    RideTypeDescriptorData* data = GetRideTypeDescriptorData(thisVal);
    if (data != nullptr)
    {
        delete data;
    }
}

ScRideTypeDescriptor::RideTypeDescriptorData* ScRideTypeDescriptor::GetRideTypeDescriptorData(JSValue thisVal)
{
    return gScRideTypeDescriptor.GetOpaque<RideTypeDescriptorData*>(thisVal);
}

JSValue ScRideTypeDescriptor::rideType_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return JS_NewInt32(ctx, data->_rideType);
}

JSValue ScRideTypeDescriptor::name_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return JSFromStdString(ctx, GetDescriptor(data->_rideType).Name);
}

// is this duplicative? i feel like we can already get ride categories some other way?
// i see that it can return "none". i wonder which ones do that. do we need to cover this, or can we fall back to null?
JSValue ScRideTypeDescriptor::category_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return JSFromStdString(ctx, RideCategoryToString(GetDescriptor(data->_rideType).Category));
}

JSValue ScRideTypeDescriptor::startTrackSegment_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    const auto startTrackPiece = GetDescriptor(data->_rideType).StartTrackPiece;
    if (startTrackPiece >= TrackElemType::count)
    {
        return JS_NULL;
    }
    return gScTrackSegment.New(ctx, startTrackPiece);
}

JSValue ScRideTypeDescriptor::standardTrackSegments_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return TrackSegmentsForGroupsToArray(ctx, GetRegularTrackDrawerEntry(data->_rideType).enabledTrackGroups);
}

JSValue ScRideTypeDescriptor::extraTrackSegments_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return TrackSegmentsForGroupsToArray(ctx, GetRegularTrackDrawerEntry(data->_rideType).extraTrackGroups);
}

JSValue ScRideTypeDescriptor::standardTrackGroups_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return TrackGroupSetToArray(ctx, GetRegularTrackDrawerEntry(data->_rideType).enabledTrackGroups);
}

JSValue ScRideTypeDescriptor::extraTrackGroups_get(JSContext* ctx, JSValue thisVal)
{
    const auto* data = GetRideTypeDescriptorData(thisVal);
    return TrackGroupSetToArray(ctx, GetRegularTrackDrawerEntry(data->_rideType).extraTrackGroups);
}

#endif
