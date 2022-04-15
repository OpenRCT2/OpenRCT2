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
constexpr const RideTypeDescriptor MotionSimulatorRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_THRILL),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack2x2),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_motionsimulator),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS |
                     RIDE_TYPE_FLAG_HAS_NO_TRACK | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE |
                     RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT |
                     RIDE_TYPE_FLAG_SINGLE_SESSION | RIDE_TYPE_FLAG_SLIGHTLY_INTERESTING_TO_LOOK_AT | 
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::FilmAvengingAviators, RideMode::FilmThrillRiders)),
    SET_FIELD(DefaultMode, RideMode::FilmAvengingAviators),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_MOTION_SIMULATOR, STR_RIDE_DESCRIPTION_MOTION_SIMULATOR }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_MOTION_SIMULATOR)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 12, 64, 3, 2, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_motion_simulator),
    SET_FIELD(RatingsMultipliers, { 24, 20, 10 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 110.00_GBP, 1.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 20, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 45),
    SET_FIELD(ColourPresets, DEFAULT_FLAT_RIDE_COLOUR_PRESET),
    SET_FIELD(ColourPreview, { 0, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
};
// clang-format on
