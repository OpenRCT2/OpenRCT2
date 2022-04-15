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
constexpr const RideTypeDescriptor SpiralSlideRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack2x2),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_spiral_slide),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_NO_VEHICLES | RIDE_TYPE_FLAG_HAS_NO_TRACK | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_ALLOW_MUSIC | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_INTERESTING_TO_LOOK_AT | 
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::SingleRidePerAdmission, RideMode::UnlimitedRidesPerAdmission)),
    SET_FIELD(DefaultMode, RideMode::SingleRidePerAdmission),
    SET_FIELD(OperatingSettings, { 1, 5, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_SPIRAL_SLIDE, STR_RIDE_DESCRIPTION_SPIRAL_SLIDE }),
    SET_FIELD(NameConvention, { RideComponentType::Train, RideComponentType::Building, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPIRAL_SLIDE)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 15, 128, 0, 2, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_spiral_slide),
    SET_FIELD(RatingsMultipliers, { 50, 10, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 82.50_GBP, 1.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 40),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_DARK_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_BRIGHT_PINK, COLOUR_LIGHT_PURPLE },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
};
// clang-format on
