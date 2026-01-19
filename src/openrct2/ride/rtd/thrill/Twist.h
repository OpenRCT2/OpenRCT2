/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
namespace OpenRCT2
{
constexpr RideTypeDescriptor TwistRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::twist,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps, RtdFlag::hasLoadOptions,
                     RtdFlag::vehicleIsIntegral, RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide,
                     RtdFlag::hasVehicleColours, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::singleSession, RtdFlag::interestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::rotation),
    .DefaultMode = RideMode::rotation,
    .OperatingSettings = { 3, 6, 3 },
    .Naming = { STR_RIDE_NAME_TWIST, STR_RIDE_DESCRIPTION_TWIST },
    .NameConvention = { RideComponentType::Structure, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 64, 3, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 40, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 45.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectGentle,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 40,
    .ColourPresets = kDefaultFlatRideColourPreset,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "twist",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(1, 13), RideRating::make(0, 97), RideRating::make(1, 90) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusRotations,        0,                20, 20, 20 },
            { RatingsModifierType::BonusScenery,          0,                13943, 0, 0 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
