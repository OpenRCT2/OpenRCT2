/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
constexpr const RideTypeDescriptor SplashBoatsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_WATER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_SLOPE_STEEP_DOWN, TRACK_S_BEND, TRACK_CURVE, TRACK_ON_RIDE_PHOTO}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_splash_boats),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS | RIDE_TYPE_FLAG_HAS_DATA_LOGGING | RIDE_TYPE_FLAG_HAS_DROPS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_HAS_TRACK | RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC |
                     RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_ALLOW_MORE_VEHICLES_THAN_STATION_FITS | RIDE_TYPE_FLAG_HAS_AIR_TIME |
                     RIDE_TYPE_FLAG_SHOW_IN_TRACK_DESIGNER | RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_SPLASH_BOATS, STR_RIDE_DESCRIPTION_SPLASH_BOATS }),
    SET_FIELD(NameConvention, { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPLASH_BOATS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_CONTROL_FAILURE)),
    SET_FIELD(Heights, { 16, 24, 7, 11, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_splash_boats),
    SET_FIELD(RatingsMultipliers, { 80, 34, 6 }),
    SET_FIELD(UpkeepCosts, { 70, 20, 0, 9, 0, 10 }),
    SET_FIELD(BuildCosts, { 28.50_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_WATER),
    SET_FIELD(PhotoItem, ShopItem::Photo4),
    SET_FIELD(BonusValue, 65),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_TRACK, SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
<<<<<<< HEAD
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(Name, "splash_boats"),
=======
>>>>>>> 2e1d52f37 (Default to DefaultStartRideMusicChannel)
};
// clang-format on
