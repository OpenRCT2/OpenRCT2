/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor FlyingSaucersRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack4x4),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionFlyingSaucers),
    SET_FIELD(Flags, { {
            RideTypeFlags::HasTrackColourMain,
            RideTypeFlags::HasTrackColourAdditional,
            RideTypeFlags::HasSinglePieceStation,
            RideTypeFlags::CannotHaveGaps,
            RideTypeFlags::NoTestMode,
            RideTypeFlags::HasNoTrack,
            RideTypeFlags::TrackNoWalls,
            RideTypeFlags::FlatRide,
            RideTypeFlags::HasVehicleColours,
            RideTypeFlags::MusicOnDefault,
            RideTypeFlags::AllowMusic,
            RideTypeFlags::HasEntranceExit,
            RideTypeFlags::SingleSession,
            RideTypeFlags::InterestingToLookAt,
            RideTypeFlags::ListVehiclesSeparately } } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::Dodgems)),
    SET_FIELD(DefaultMode, RideMode::Dodgems),
    SET_FIELD(OperatingSettings, { 20, 180, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_FLYING_SAUCERS, STR_RIDE_DESCRIPTION_FLYING_SAUCERS }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_FLYING_SAUCERS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 9, 48, 2, 2, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_flying_saucers),
    SET_FIELD(RatingsMultipliers, { 50, 25, 0 }),
    SET_FIELD(UpkeepCosts, { 90, 1, 0, 5, 0, 0 }),
    SET_FIELD(BuildCosts, { 35.00_GBP, 2.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_ROCK_1),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 35),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_PURPLE, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_SATURATED_RED, COLOUR_BLACK },
        { COLOUR_ICY_BLUE, COLOUR_WHITE, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_FLYING_SAUCERS_TRACK, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "flying_saucers"),
};
// clang-format on
