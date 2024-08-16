/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../ride/RideStringIds.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor DinghySlideRTD =
{
    .Category = RIDE_CATEGORY_WATER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(
        {
            .Drawer = GetTrackPaintFunctionDinghySlide,
            .supportType = MetalSupportType::Tubes,
            .EnabledTrackPieces = { TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve },
            .ExtraTrackPieces = {},
            .icon = SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK,
            .tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP,
        }, 
        {
            .Drawer = GetTrackPaintFunctionDinghySlideCovered,
            .supportType = MetalSupportType::Tubes,
            .EnabledTrackPieces = {OpenRCT2::TrackElemType::Flat, OpenRCT2::TrackElemType::Up25, OpenRCT2::TrackElemType::Up60, OpenRCT2::TrackElemType::FlatToUp25, OpenRCT2::TrackElemType::Up25ToUp60, OpenRCT2::TrackElemType::Up60ToUp25, OpenRCT2::TrackElemType::Up25ToFlat, OpenRCT2::TrackElemType::Down25, OpenRCT2::TrackElemType::Down60, OpenRCT2::TrackElemType::FlatToDown25, OpenRCT2::TrackElemType::Down25ToDown60, OpenRCT2::TrackElemType::Down60ToDown25, OpenRCT2::TrackElemType::Down25ToFlat, OpenRCT2::TrackElemType::LeftQuarterTurn5Tiles, OpenRCT2::TrackElemType::RightQuarterTurn5Tiles, OpenRCT2::TrackElemType::SBendLeft, OpenRCT2::TrackElemType::SBendRight, OpenRCT2::TrackElemType::LeftQuarterTurn3Tiles,OpenRCT2::TrackElemType::RightQuarterTurn3Tiles},
            .ExtraTrackPieces = {},
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
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit),
    .DefaultMode = RideMode::ContinuousCircuit,
    .Naming = { STR_RIDE_NAME_DINGHY_SLIDE, STR_RIDE_DESCRIPTION_DINGHY_SLIDE },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_DINGHY_SLIDE",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 15, 24, 5, 7, },
    .MaxMass = 5,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftFrictionWheels, 4, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 40, 20, 80, 4, 3, 10 },
    .BuildCosts = { 20.00_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_WATER,
    .PhotoItem = ShopItem::Photo4,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_SATURATED_BROWN, COLOUR_DARK_GREEN },
        { COLOUR_TEAL, COLOUR_BORDEAUX_RED, COLOUR_TEAL },
        { COLOUR_BRIGHT_RED, COLOUR_GREY, COLOUR_SATURATED_RED },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_TRACK, SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "dinghy_slide",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { RIDE_RATING(2, 70), RIDE_RATING(2, 00), RIDE_RATING(1, 50) },
        13,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 50), RIDE_RATING(0, 05), 0 },
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
// clang-format on
