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
constexpr RideTypeDescriptor SubmarineRideRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::submarineRide,
        .supportType = MetalSupportType::Stick,
        .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::curveVerySmall, TrackGroup::curveSmall},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation,
                     RtdFlag::trackMustBeOnWater, RtdFlag::hasLoadOptions, RtdFlag::guestsWillRideAgain,
                     RtdFlag::hasVehicleColours, RtdFlag::hasTrack, RtdFlag::supportsMultipleColourSchemes,
                     RtdFlag::allowMusic, RtdFlag::checkGForces, RtdFlag::hasEntranceAndExit, 
                     RtdFlag::allowMoreVehiclesThanStationFits),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_SUBMARINE_RIDE, STR_RIDE_DESCRIPTION_SUBMARINE_RIDE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::DockingPlatform },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION),
    .Heights = { 255, 16, 0, 3, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 70, 6, 0 },
    .UpkeepCosts = { 50, 1, 0, 4, 0, 0 },
    .BuildCosts = { 35.00_GBP, 2.50_GBP, 5, },
    .DefaultPrices = { 10, 0 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 40,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "submarine_ride",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 20), MakeRideRating(1, 80), MakeRideRating(1, 40) },
        7,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusProximity,        0,                11183, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                22310, 0, 0 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
};
// clang-format on
