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
#include "../../../paint/support/MetalSupports.h"
#include "../../RideData.h"
#include "../../RideStringIds.h"
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor VerticalDropCoasterRTD =
{
    .Category = RideCategory::rollerCoaster,
    .StartTrackPiece = TrackElemType::endStation,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::twisterRollerCoaster,
        .supportType = MetalSupportType::boxed,
        .enabledTrackGroups = { TrackGroup::flat, TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::liftHillSteep, TrackGroup::flatRollBanking, TrackGroup::verticalLoop, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::flatToSteepSlope, TrackGroup::slopeCurve, TrackGroup::slopeCurveSteep, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::onridePhoto, TrackGroup::slopeVertical, TrackGroup::slopeCurveBanked, TrackGroup::blockBrakes, TrackGroup::slopeRollBanking, TrackGroup::curveVertical, TrackGroup::halfLoopLarge, TrackGroup::brakeForDrop, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes, TrackGroup::slopeCurveLarge, TrackGroup::halfLoopMedium, TrackGroup::zeroGRoll, TrackGroup::zeroGRollLarge, TrackGroup::corkscrew, TrackGroup::corkscrewLarge, TrackGroup::diveLoop, TrackGroup::diagSlope, TrackGroup::diagSlopeSteepUp, TrackGroup::diagSlopeSteepDown, TrackGroup::helixUpUnbankedQuarter, TrackGroup::helixDownUnbankedQuarter, TrackGroup::helixUpBankedQuarter, TrackGroup::helixDownBankedQuarter },
        .extraTrackGroups = {TrackGroup::halfLoop, TrackGroup::barrelRoll, TrackGroup::poweredLift, TrackGroup::halfLoopLarge, TrackGroup::quarterLoop, TrackGroup::booster, TrackGroup::twist, TrackGroup::slopeSteepLong, TrackGroup::diagSlopeSteepLong},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt |
        RtdFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::continuousCircuit, RideMode::continuousCircuitBlockSectioned),
    .DefaultMode = RideMode::continuousCircuit,
    .OperatingSettings = { 10, 27 },
    .BoosterSettings = { 17, 68 },
    .LegacyBoosterSettings = { 17, 68 },
    .Naming = { STR_RIDE_NAME_VERTICAL_DROP_ROLLER_COASTER, STR_RIDE_DESCRIPTION_VERTICAL_DROP_ROLLER_COASTER },
    .NameConvention = { RideComponentType::train, RideComponentType::track, RideComponentType::station },
    .availableBreakdowns = { Breakdown::safetyCutOut, Breakdown::restraintsStuckClosed, Breakdown::restraintsStuckOpen, Breakdown::vehicleMalfunction, Breakdown::brakesFailure },
    .Heights = { 55, 24, 8, 11, },
    .MaxMass = 25,
    .LiftData = { Audio::SoundId::liftBM, 4, 5 },
    .RatingsMultipliers = { 52, 38, 10 },
    .UpkeepCosts = { 44, 20, 80, 11, 3, 10 },
    .BuildCosts = { 60.00_GBP, 2.50_GBP, 42, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectRock1,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 95,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::black, Drawing::Colour::grey, Drawing::Colour::black },
        { Drawing::Colour::darkGreen, Drawing::Colour::darkGreen, Drawing::Colour::grey },
        { Drawing::Colour::darkOrange, Drawing::Colour::darkOrange, Drawing::Colour::viridian }, // Yukon Striker
        { Drawing::Colour::lightOrange, Drawing::Colour::darkOrange, Drawing::Colour::darkBlue }, // Valravn
        { Drawing::Colour::grey, Drawing::Colour::brightRed, Drawing::Colour::yellow }, // Dr. Diabolical's Cliffhanger
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::ride,
    .Name = "vertical_drop_rc",
    .RatingsData =
    {
        RatingsCalculationType::normal,
        { RideRating::make(3, 20), RideRating::make(0, 80), RideRating::make(0, 30) },
        16,
        kDynamicRideShelterRating,
        false,
        {
            { RatingsModifierType::bonusLength,           4000,             1146, 0, 0 },
            { RatingsModifierType::bonusSynchronisation,  0,                RideRating::make(0, 40), RideRating::make(0, 05), 0 },
            { RatingsModifierType::bonusMaxSpeed,         0,                97418, 141699, 70849 },
            { RatingsModifierType::bonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::bonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::bonusGForces,          0,                40960, 35746, 49648 },
            { RatingsModifierType::bonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::bonusDrops,            0,                58254, 46811, 49152 },
            { RatingsModifierType::bonusSheltered,        0,                15420, 32768, 35108 },
            { RatingsModifierType::bonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::bonusScenery,          0,                6693, 0, 0 },
            { RatingsModifierType::requirementDropHeight, 20,               2, 2, 2 },
            { RatingsModifierType::requirementMaxSpeed,   0xA0000,          2, 2, 2 },
            { RatingsModifierType::requirementNegativeGs, MakeFixed16_2dp(0, 10), 2, 2, 2 },
            { RatingsModifierType::requirementNumDrops,   1,                2, 2, 2 },
            { RatingsModifierType::penaltyLateralGs,      0,                40960, 35746, 49648 },
        },
    },
};
} // namespace OpenRCT2
// clang-format on
