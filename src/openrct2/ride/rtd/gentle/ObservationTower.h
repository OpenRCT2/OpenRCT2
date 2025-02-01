/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../drawing/LightFX.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor ObservationTowerRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::TowerBase,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::observationTower,
        .enabledTrackGroups = {TrackGroup::tower},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation, RtdFlag::cannotHaveGaps,
                     RtdFlag::hasLoadOptions, RtdFlag::noWallsAroundTrack, RtdFlag::hasVehicleColours, RtdFlag::hasTrack,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::showInTrackDesigner, RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::RotatingLift),
    .DefaultMode = RideMode::RotatingLift,
    .Naming = { STR_RIDE_NAME_OBSERVATION_TOWER, STR_RIDE_DESCRIPTION_OBSERVATION_TOWER },
    .NameConvention = { RideComponentType::Cabin, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 32, 3, 2, },
    .MaxMass = 15,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 80, 10, 0 },
    .UpkeepCosts = { 50, 20, 0, 10, 0, 0 },
    .BuildCosts = { 18.50_GBP, 0.00_GBP, 4, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 45,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
        { COLOUR_LIGHT_BLUE, COLOUR_YELLOW, COLOUR_GREY },
        { COLOUR_WHITE, COLOUR_YELLOW, COLOUR_GREY },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_TRACK, SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "observation_tower",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 50), MakeRideRating(0, 00), MakeRideRating(0, 10) },
        15,
        7,
        false,
        {
            { RatingsModifierType::BonusProximity,         0, 20130, 0, 0 },
            { RatingsModifierType::BonusScenery,           0, 83662, 0, 0 },
            { RatingsModifierType::BonusTowerRide,       0, 45875, 0, 26214 },
            { RatingsModifierType::RequirementUnsheltered, 5, 4, 1, 1 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_ObservationTower,
};
// clang-format on
