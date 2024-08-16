/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor BobsleighCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionBobsleighRC,
        .supportType = MetalSupportType::Tubes,        
        .EnabledTrackPieces = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve,TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes,TrackGroup::blockBrakes, TrackGroup::onridePhoto},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                     RtdFlag::allowMultipleCircuits),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_BOBSLEIGH_COASTER, STR_RIDE_DESCRIPTION_BOBSLEIGH_COASTER },
    .NameConvention = { RideComponentType::Train, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_BOBSLEIGH_COASTER",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 19, 24, 5, 7, },
    .MaxMass = 25,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 10, 3, 10 },
    .BuildCosts = { 40.00_GBP, 2.50_GBP, 45, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_ROCK_1,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 75,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_TAN_DARK, COLOUR_DARK_ORANGE, COLOUR_DULL_BROWN_LIGHT }, // Reptilian
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "bobsleigh_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 80), RIDE_RATING(3, 20), RIDE_RATING(2, 50) },
        16,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 20), RIDE_RATING(0, 00), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                65536, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::RequirementMaxSpeed,   0xC0000,          2, 2, 2 },
            { RatingsModifierType::RequirementLateralGs,  FIXED_2DP(1, 20), 2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0x1720000,        2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                65536, 23831, 49648 },
        },
    },
};
// clang-format on
