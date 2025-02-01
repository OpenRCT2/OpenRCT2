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
constexpr RideTypeDescriptor HeartlineTwisterCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::heartlineTwisterCoaster,
        .supportType = WoodenSupportType::Mine,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::heartlineRoll, TrackGroup::heartlineTransfer},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                              RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_HEARTLINE_TWISTER_COASTER, STR_RIDE_DESCRIPTION_HEARTLINE_TWISTER_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 22, 24, 15, 9, },
    .MaxMass = 18,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 47, 20, 80, 11, 3, 10 },
    .BuildCosts = { 72.50_GBP, 3.50_GBP, 30, },
    .DefaultPrices = { 15, 20 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "heartline_twister_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(3, 00), MakeRideRating(1, 70), MakeRideRating(1, 65) },
        18,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000, 764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,    MakeRideRating(0, 20), MakeRideRating(0, 04), 0 },
            { RatingsModifierType::BonusTrainLength,      0,    187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,    97418, 123987, 70849 },
            { RatingsModifierType::BonusAverageSpeed,     0,    291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,  26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,    24576, 44683, 89367 },
            { RatingsModifierType::BonusTurns,            0,    26749, 52150, 57186 },
            { RatingsModifierType::BonusDrops,            0,    29127, 53052, 55705 },
            { RatingsModifierType::BonusSheltered,        0,    15420, 34952, 35108 },
            { RatingsModifierType::BonusProximity,        0,    9841, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,    3904, 0, 0 },
            { RatingsModifierType::RequirementInversions, 1,    4, 1, 1 },
            { RatingsModifierType::RequirementNumDrops,   1,    4, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,    24576, 44683, 89367 },
        },
    },
};
// clang-format on
