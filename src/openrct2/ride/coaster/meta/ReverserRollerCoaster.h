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
constexpr const RideTypeDescriptor ReverserRollerCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_SLOPE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_BRAKES, TRACK_REVERSER}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_reverser_rc),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_REVERSER_ROLLER_COASTER, STR_RIDE_DESCRIPTION_REVERSER_ROLLER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_REVERSER_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE)),
    SET_FIELD(Heights, { 18, 24, 8, 11, }),
    SET_FIELD(MaxMass, 15),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftWood, 3, 4 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_reverser_roller_coaster),
    SET_FIELD(RatingsMultipliers, { 48, 28, 7 }),
    SET_FIELD(UpkeepCosts, { 39, 20, 80, 10, 3, 10 }),
    SET_FIELD(BuildCosts, { 27.50_GBP, 3.00_GBP, 37, }),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_TECHNO),
    SET_FIELD(PhotoItem, ShopItem::Photo3),
    SET_FIELD(BonusValue, 65),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_LIGHT_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_SATURATED_BROWN, COLOUR_BORDEAUX_RED, COLOUR_SATURATED_BROWN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
};
// clang-format on
