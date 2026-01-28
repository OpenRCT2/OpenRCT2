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
#include "../../../drawing/LightFX.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor CarRideRTD =
{
    .Category = RideCategory::gentle,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::carRide,
        .supportType = MetalSupportType::boxed,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::slope, TrackGroup::curveVerySmall, TrackGroup::curveSmall, TrackGroup::spinningTunnel},
        .extraTrackGroups = {TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::rapids},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::canSynchroniseWithAdjacentStations,
                     RtdFlag::hasDataLogging, RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain,
                     RtdFlag::hasVehicleColours, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowDoorsOnTrack, RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit,
                     RtdFlag::allowMoreVehiclesThanStationFits, RtdFlag::showInTrackDesigner, 
                     RtdFlag::slightlyInterestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit),
    .DefaultMode = RideMode::continuousCircuit,
    .Naming = { STR_RIDE_NAME_CAR_RIDE, STR_RIDE_DESCRIPTION_CAR_RIDE },
    .NameConvention = { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 6, 24, 4, 7, },
    .MaxMass = 2,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 70, 10, 10 },
    .UpkeepCosts = { 70, 20, 0, 8, 3, 5 },
    .BuildCosts = { 12.50_GBP, 2.50_GBP, 30, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectSummer,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 50,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::saturatedBrown, Drawing::Colour::saturatedBrown, Drawing::Colour::grey },
        { Drawing::Colour::lightPurple, Drawing::Colour::lightPurple, Drawing::Colour::white },
        { Drawing::Colour::bordeauxRed, Drawing::Colour::bordeauxRed, Drawing::Colour::oliveGreen },
        { Drawing::Colour::grey, Drawing::Colour::grey, Drawing::Colour::black },
        { Drawing::Colour::black, Drawing::Colour::black, Drawing::Colour::saturatedBrown },
        { Drawing::Colour::brightYellow, Drawing::Colour::brightYellow, Drawing::Colour::brightRed },
        { Drawing::Colour::lightWater, Drawing::Colour::lightWater, Drawing::Colour::grey },
        { Drawing::Colour::icyBlue, Drawing::Colour::icyBlue, Drawing::Colour::white },
        { Drawing::Colour::white, Drawing::Colour::white, Drawing::Colour::oliveGreen },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "car_ride",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RideRating::make(2, 00), RideRating::make(0, 50), RideRating::make(0, 00) },
        12,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RideRating::make(0, 15), RideRating::make(0, 00), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusTurns,            0,                14860, 0, 11437 },
            { RatingsModifierType::BonusDrops,            0,                8738, 0, 0 },
            { RatingsModifierType::BonusSheltered,        0,                12850, 6553, 4681 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                8366, 0, 0 },
            { RatingsModifierType::RequirementLength,     0xC80000,         8, 2, 2 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
} // namespace OpenRCT2
// clang-format on
