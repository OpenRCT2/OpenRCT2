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
#include "../../../ride/RideStringIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor DinghySlideRTD =
{
    .Category = RideCategory::water,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(
        {
            .trackStyle = TrackStyle::dinghySlide,
            .supportType = MetalSupportType::tubes,
            .enabledTrackGroups = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve },
            .extraTrackGroups = {},
            .icon = SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK,
            .tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP,
        }, 
        {
            .trackStyle = TrackStyle::dinghySlideCovered,
            .supportType = MetalSupportType::tubes,
            .enabledTrackGroups = { TrackGroup::straight, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::curve, TrackGroup::sBend, TrackGroup::curveSmall },
            .extraTrackGroups = {},
            .icon = SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK,
            .tooltip = STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP,
        }
    ),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, 
                     RtdFlag::canSynchroniseWithAdjacentStations, RtdFlag::hasGForces, 
                     RtdFlag::hasDataLogging, RtdFlag::hasDrops, RtdFlag::hasCoveredPieces, 
                     RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain, RtdFlag::hasVehicleColours, 
                     RtdFlag::checkForStalling, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes, 
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::hasAirTime, 
                     RtdFlag::showInTrackDesigner, RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_DINGHY_SLIDE, STR_RIDE_DESCRIPTION_DINGHY_SLIDE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 24, 5, 7, },
    .MaxMass = 5,
    .LiftData = { Audio::SoundId::liftFrictionWheels, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 4, 3, 10 },
    .BuildCosts = { 20.00_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::photo4,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkGreen, Drawing::Colour::saturatedBrown, Drawing::Colour::darkGreen },
        { Drawing::Colour::darkWater, Drawing::Colour::bordeauxRed, Drawing::Colour::darkWater },
        { Drawing::Colour::brightRed, Drawing::Colour::grey, Drawing::Colour::saturatedRed },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "dinghy_slide",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(2, 70), RideRating::make(2, 00), RideRating::make(1, 50) },
        13,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 50), RideRating::make(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                65536, 29789, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                5577, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 12,               2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementLength,     0x8C0000,         2, 2, 2 },
            { RatingsModifierType::PenaltyLateralGs,      0,                65536, 29789, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
