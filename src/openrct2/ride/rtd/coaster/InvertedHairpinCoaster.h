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
constexpr RideTypeDescriptor InvertedHairpinCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::invertedHairpinCoaster,
        .supportType = MetalSupportType::TubesInverted,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::brakes, TrackGroup::blockBrakes},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::isSuspended),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_INVERTED_HAIRPIN_COASTER, STR_RIDE_DESCRIPTION_INVERTED_HAIRPIN_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 16, 24, 24, 7, },
    .MaxMass = 4,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftWildMouse, 4, 6 },
    .RatingsMultipliers = { 50, 30, 30 },
    .UpkeepCosts = { 40, 20, 80, 9, 3, 10 },
    .BuildCosts = { 33.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock2,
    .PhotoItem = ShopItem::Photo2,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_RED },
        { COLOUR_BLACK, COLOUR_ICY_BLUE, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_YELLOW },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "inverted_hairpin_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 00), MakeRideRating(2, 65), MakeRideRating(2, 25) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             873, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 8), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                364088, 655360, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                102400, 35746, 49648 },
            { RatingsModifierType::BonusTurns,            0,                29721, 43458, 45749 },
            { RatingsModifierType::BonusDrops,            0,                40777, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                16705, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                17893, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementNegativeGs, MakeFixed16_2dp(0, 10), 2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  MakeFixed16_2dp(1, 50), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0xAA0000,         2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   3,                2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                102400, 35746, 49648 },
        },
    },
};
// clang-format on
