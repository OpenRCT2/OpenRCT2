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
namespace OpenRCT2 {
constexpr RideTypeDescriptor MiniGolfRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::miniGolf,
        .supportType = MetalSupportType::boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::miniGolfHole},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::noTestMode, RtdFlag::hasTrack, RtdFlag::hasOneStation,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_MINI_GOLF, STR_RIDE_DESCRIPTION_MINI_GOLF },
    .NameConvention = { RideComponentType::Player, RideComponentType::Course, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 7, 32, 2, 2, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 30, 60, 0, 0, 0, 0 },
    .BuildCosts = { 25.00_GBP, 3.50_GBP, 20, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 23,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::saturatedGreen, Drawing::Colour::darkBrown, Drawing::Colour::black },
        { Drawing::Colour::mossGreen, Drawing::Colour::bordeauxRed, Drawing::Colour::darkBrown },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "mini_golf",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(1, 50), RideRating::make(0, 90), RideRating::make(0, 00) },
        0,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,      6000, 873, 0, 0 },
            { RatingsModifierType::BonusTurns,       0,    14860, 0, 0 },
            // The first six holes used to benefit from bonuses intended for inversions.
            { RatingsModifierType::BonusHoles,       6,    6, 0, 0 },
            { RatingsModifierType::BonusSheltered,   0,    5140, 6553, 4681 },
            { RatingsModifierType::BonusProximity,   0,    15657, 0, 0 },
            { RatingsModifierType::BonusScenery,     0,    27887, 0, 0 },
            { RatingsModifierType::BonusHoles,       31,   5, 0, 0 },
            { RatingsModifierType::RequirementHoles, 1,    8, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_MiniGolf,
    .specialType = RtdSpecialType::miniGolf,
};
} // namespace OpenRCT2
// clang-format on
