/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
constexpr RideTypeDescriptor RotoDropRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = OpenRCT2::TrackElemType::TowerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::rotoDrop,
        .enabledTrackGroups = {TrackGroup::tower},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::noWallsAroundTrack,
                     RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::singleSession, RtdFlag::showInTrackDesigner, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::FreefallDrop),
    .DefaultMode = RideMode::FreefallDrop,
    .Naming = { STR_RIDE_NAME_ROTO_DROP, STR_RIDE_DESCRIPTION_ROTO_DROP },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 50, 10 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 22.50_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_TRACK, SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "roto_drop",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 80), MakeRideRating(3, 50), MakeRideRating(3, 50) },
        24,
        -1,
        false,
        {
            { RatingsModifierType::BonusProximity,   0, 11183, 0, 0 },
            { RatingsModifierType::BonusScenery,     0, 25098, 0, 0 },
            { RatingsModifierType::BonusRotoDrop,    0, 0, 0, 0 },
        },
    },
};
// clang-format on
