/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor LaunchedFreefallRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::towerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::launchedFreefall,
        .enabledTrackGroups = {TrackGroup::tower},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::noWallsAroundTrack, RtdFlag::guestsWillRideAgain,
                     RtdFlag::hasVehicleColours, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::showInTrackDesigner, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::upwardLaunch, RideMode::downwardLaunch),
    .DefaultMode = RideMode::upwardLaunch,
    .OperatingSettings = { 10, 40 },
    .Naming = { STR_RIDE_NAME_LAUNCHED_FREEFALL, STR_RIDE_DESCRIPTION_LAUNCHED_FREEFALL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 50, 10 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 25.00_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 65,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::white, Drawing::Colour::brightRed, Drawing::Colour::lightPurple },
        { Drawing::Colour::darkGreen, Drawing::Colour::yellow, Drawing::Colour::white },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_TRACK, SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "launched_freefall",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(2, 70), RideRating::make(3, 00), RideRating::make(3, 50) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusDownwardLaunch,          0, RideRating::make(0, 30), RideRating::make(0, 65), RideRating::make(0, 45) },
            { RatingsModifierType::BonusLaunchedFreefallSpecial, 0, 0, 1355917, 451972 },
            { RatingsModifierType::BonusProximity,               0, 20130, 0, 0 },
            { RatingsModifierType::BonusScenery,                 0, 25098, 0, 0 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
