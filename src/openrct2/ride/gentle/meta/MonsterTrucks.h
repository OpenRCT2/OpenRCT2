/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor MonsterTrucksRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL, TRACK_RAPIDS}),
    SET_FIELD(ExtraTrackPieces, {TRACK_SPINNING_TUNNEL}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_car_ride),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS |
                     RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR |
                     RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS | RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | 
                     RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, (1ULL << static_cast<uint8_t>(RideMode::ContinuousCircuit))),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_MONSTER_TRUCKS, STR_RIDE_DESCRIPTION_MONSTER_TRUCKS_GROUP }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_MONSTER_TRUCKS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 18, 24, 4, 7, }),
    SET_FIELD(MaxMass, 2),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_car_ride),
    SET_FIELD(RatingsMultipliers, { 70, 10, 10 }),
    SET_FIELD(UpkeepCosts, { 70, 20, 0, 8, 3, 5 }),
    SET_FIELD(BuildCosts, { 12.50_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 50),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_GREY },
        { COLOUR_LIGHT_PURPLE, COLOUR_LIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(RCT2ToOpenRCT2ConvertFunction, RCT2ToOpenRCT2::NoConversion),
};
// clang-format on
