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
constexpr RideTypeDescriptor LiftRTD =
{
    .Category = RideCategory::transport,
    .StartTrackPiece = TrackElemType::towerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::lift,
        .enabledTrackGroups = {TrackGroup::tower},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = EnumsToFlags(RtdFlag::hasTrackColourMain, RtdFlag::hasTrackColourSupports, RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::hasLoadOptions, RtdFlag::noWallsAroundTrack, RtdFlag::hasVehicleColours,
                     RtdFlag::hasTrack, RtdFlag::allowExtraTowerBases, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::isTransportRide, RtdFlag::showInTrackDesigner),
    .RideModes = EnumsToFlags(RideMode::shuttle),
    .DefaultMode = RideMode::shuttle,
    .Naming = { STR_RIDE_NAME_LIFT, STR_RIDE_DESCRIPTION_LIFT },
    .NameConvention = { RideComponentType::Cabin, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_DOORS_STUCK_CLOSED) | (1 << BREAKDOWN_DOORS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 80, 10, 0 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 19.50_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::grey },
        { Drawing::Colour::darkGreen, Drawing::Colour::darkGreen, Drawing::Colour::grey },
        { Drawing::Colour::black, Drawing::Colour::black, Drawing::Colour::grey },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_LIFT_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "lift",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(1, 11), RideRating::make(0, 35), RideRating::make(0, 30) },
        15,
        7,
        false,
        {
            { RatingsModifierType::BonusProximity,         0, 11183, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 83662, 0, 0 },
            { RatingsModifierType::BonusTowerRide,       0, 45875, 0, 26214 },
            { RatingsModifierType::RequirementUnsheltered, 5, 4, 1, 1 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
