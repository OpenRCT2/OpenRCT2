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
constexpr const RideTypeDescriptor SpaceRingsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack3x3),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionSpaceRings),
    SET_FIELD(Flags, { {
        RideTypeFlags::HasSinglePieceStation,
        RideTypeFlags::CannotHaveGaps,
        RideTypeFlags::NoTestMode,
        RideTypeFlags::HasNoTrack,
        RideTypeFlags::TrackNoWalls,
        RideTypeFlags::FlatRide,
        RideTypeFlags::HasVehicleColours,
        RideTypeFlags::AllowMusic,
        RideTypeFlags::HasEntranceExit,
        RideTypeFlags::SingleSession,
        RideTypeFlags::SlightlyInterestingToLookAt,
        RideTypeFlags::ListVehiclesSeparately } } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::SpaceRings)),
    SET_FIELD(DefaultMode, RideMode::SpaceRings),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_SPACE_RINGS, STR_RIDE_DESCRIPTION_SPACE_RINGS }),
    SET_FIELD(NameConvention, { RideComponentType::Ring, RideComponentType::Structure, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_SPACE_RINGS)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT)),
    SET_FIELD(Heights, { 16, 48, 3, 2, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_space_rings),
    SET_FIELD(RatingsMultipliers, { 12, 4, 4 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 36.00_GBP, 1.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 5, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_GENTLE),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 30),
    SET_FIELD(ColourPresets, DEFAULT_FLAT_RIDE_COLOUR_PRESET),
    SET_FIELD(ColourPreview, { 0, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "space_rings"),
};
// clang-format on
