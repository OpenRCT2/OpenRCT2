/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor RiverRaftsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_WATER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_S_BEND, TRACK_CURVE}),
    SET_FIELD(ExtraTrackPieces, {TRACK_SLOPE, TRACK_SLOPE_STEEP_DOWN, TRACK_ON_RIDE_PHOTO}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_splash_boats),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS | RIDE_TYPE_FLAG_HAS_DATA_LOGGING |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC |
                     RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS |
                     RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_RIVER_RAFTS, STR_RIDE_DESCRIPTION_RIVER_RAFTS }),
    SET_FIELD(NameConvention, { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_RIVER_RAFTS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 12, 24, 7, 11, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_river_rafts),
    SET_FIELD(RatingsMultipliers, { 80, 34, 6 }),
    SET_FIELD(UpkeepCosts, { 50, 20, 0, 9, 0, 10 }),
    SET_FIELD(BuildCosts, { 20.00_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 10, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_GENTLE),
    SET_FIELD(PhotoItem, ShopItem::Photo4),
    SET_FIELD(BonusValue, 65),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "river_rafts"),
};
// clang-format on
