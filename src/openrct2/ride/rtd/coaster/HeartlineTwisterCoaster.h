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
constexpr RideTypeDescriptor HeartlineTwisterCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::heartlineTwisterCoaster,
        .supportType = WoodenSupportType::mine,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::heartlineRoll, TrackGroup::heartlineTransfer},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
                 EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces,
                              RtdFlag::allowReversedTrains),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 25, 25 },
    .LegacyBoosterSettings = { 25, 25 },
    .Naming = { STR_RIDE_NAME_HEARTLINE_TWISTER_COASTER, STR_RIDE_DESCRIPTION_HEARTLINE_TWISTER_COASTER },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 22, 24, 15, 9, },
    .MaxMass = 18,
    .LiftData = { Audio::SoundId::liftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 47, 20, 80, 11, 3, 10 },
    .BuildCosts = { 72.50_GBP, 3.50_GBP, 30, },
    .DefaultPrices = { 15, 20 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 70,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::white, Drawing::Colour::lightBlue, Drawing::Colour::white },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::yellow, Drawing::Colour::black },
        { Drawing::Colour::darkPurple, Drawing::Colour::white, Drawing::Colour::darkGreen },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "heartline_twister_rc",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(3, 00), RideRating::make(1, 70), RideRating::make(1, 65) },
        18,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000, 764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,    RideRating::make(0, 20), RideRating::make(0, 04), 0 },
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
} // namespace OpenRCT2
// clang-format on
