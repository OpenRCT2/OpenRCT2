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
constexpr const RideTypeDescriptor GoKartsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_THRILL),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_go_karts),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS | RIDE_TYPE_FLAG_NO_TEST_MODE |
                     RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS | RIDE_TYPE_FLAG_HAS_TRACK |
                     RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::Race, RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::Race),
    SET_FIELD(OperatingSettings, { 1, 10, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_GO_KARTS, STR_RIDE_DESCRIPTION_GO_KARTS }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_GO_KARTS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 7, 24, 2, 1, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_go_karts),
    SET_FIELD(RatingsMultipliers, { 120, 20, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 20, 0, 8, 0, 0 }),
    SET_FIELD(BuildCosts, { 31.00_GBP, 2.00_GBP, 20, }),
    SET_FIELD(DefaultPrices, { 20, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_TECHNO),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 55),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_DARK_GREEN },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK, SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(RCT2ToOpenRCT2ConvertFunction, RCT2ToOpenRCT2::NoConversion),
};
// clang-format on
