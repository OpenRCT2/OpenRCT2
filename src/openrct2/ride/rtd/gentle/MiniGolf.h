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
constexpr RideTypeDescriptor MiniGolfRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::miniGolf,
        .supportType = MetalSupportType::Boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::miniGolfHole},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::noTestMode, RtdFlag::hasTrack, RtdFlag::hasOneStation,
                     RtdFlag::supportsMultipleColourSchemes, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_MINI_GOLF, STR_RIDE_DESCRIPTION_MINI_GOLF },
    .NameConvention = { RideComponentType::Player, RideComponentType::Course, RideComponentType::Station },
    .AvailableBreakdowns = 0,
    .Heights = { 7, 32, 2, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 30, 60, 0, 0, 0, 0 },
    .BuildCosts = { 25.00_GBP, 3.50_GBP, 20, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 23,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_GREEN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_MOSS_GREEN, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_TRACK, SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "mini_golf",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(1, 50), MakeRideRating(0, 90), MakeRideRating(0, 00) },
        0,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,      6000, 873, 0, 0 },
            { RatingsModifierType::BonusTurns,       0,    14860, 0, 0 },
            { RatingsModifierType::BonusSheltered,   0,    5140, 6553, 4681 },
            { RatingsModifierType::BonusProximity,   0,    15657, 0, 0 },
            { RatingsModifierType::BonusScenery,     0,    27887, 0, 0 },
            { RatingsModifierType::BonusHoles,       0,    5, 0, 0 },
            { RatingsModifierType::RequirementHoles, 1,    8, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_MiniGolf,
    .specialType = RtdSpecialType::miniGolf,
};
// clang-format on
