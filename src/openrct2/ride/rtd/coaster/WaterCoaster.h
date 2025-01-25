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
#include "../../../ride/RideStringIds.h"
#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor WaterCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = OpenRCT2::TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(
        {
            .trackStyle = TrackStyle::waterCoaster,
            .supportType = MetalSupportType::Fork,
            .enabledTrackGroups = {TrackGroup::straight, TrackGroup::stationEnd, TrackGroup::liftHill, TrackGroup::flatRollBanking, TrackGroup::slope, TrackGroup::slopeSteepUp, TrackGroup::slopeSteepDown, TrackGroup::slopeCurve, TrackGroup::sBend, TrackGroup::curveSmall, TrackGroup::curve, TrackGroup::curveLarge, TrackGroup::helixDownBankedHalf, TrackGroup::helixUpBankedHalf, TrackGroup::brakes, TrackGroup::blockBrakes, TrackGroup::onridePhoto, TrackGroup::booster, TrackGroup::diagBrakes, TrackGroup::diagBlockBrakes},
            .extraTrackGroups = {TrackGroup::flatToSteepSlope},
            .icon = SPR_RIDE_CONSTRUCTION_RC_TRACK,
            .tooltip = STR_RIDE_CONSTRUCTION_STANDARD_RC_TRACK_TIP,
        },
        {
            .trackStyle = TrackStyle::splashBoats,
            .supportType = WoodenSupportType::Truss,
            .enabledTrackGroups = { TrackGroup::straight, TrackGroup::curve, TrackGroup::sBend },
            .extraTrackGroups = {},
            .icon = SPR_RIDE_CONSTRUCTION_WATER_CHANNEL,
            .tooltip = STR_RIDE_CONSTRUCTION_WATER_CHANNEL_TIP,
        }
    ),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | kRtdFlagsCommonCoaster | kRtdFlagsCommonCoasterNonAlt | 
        EnumsToFlags(RtdFlag::hasLeaveWhenAnotherVehicleArrivesAtStation, RtdFlag::hasCoveredPieces, RtdFlag::checkGForces),
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .TrackSpeedSettings = { 30, 15 },
    .BoosterSettings = { 17, 16 },
    .LegacyBoosterSettings = { 17, 16, 1 },
    .Naming = { STR_RIDE_NAME_WATER_COASTER, STR_RIDE_DESCRIPTION_WATER_COASTER },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 18, 24, 4, 7, },
    .MaxMass = 13,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 60, 20, 80, 8, 3, 10 },
    .BuildCosts = { 27.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = kMusicObjectWater,
    .PhotoItem = ShopItem::Photo4,
    .BonusValue = 60,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_BLACK },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_DARK_PURPLE },
        { COLOUR_DULL_BROWN_LIGHT, COLOUR_DULL_BROWN_LIGHT, COLOUR_DARK_BROWN }, // De Vliegende Hollander
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "water_coaster",
    .RatingsData = 
    {
        RatingsCalculationType::Normal,
        { MakeRideRating(2, 70), MakeRideRating(2, 80), MakeRideRating(2, 10) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                MakeRideRating(0, 40), MakeRideRating(0, 05), 0 },
            { RatingsModifierType::BonusTrainLength,      0,                187245, 0, 0 },
            { RatingsModifierType::BonusMaxSpeed,         0,                44281, 88562, 35424 },
            { RatingsModifierType::BonusAverageSpeed,     0,                291271, 436906, 0 },
            { RatingsModifierType::BonusDuration,         150,              26214, 0, 0 },
            { RatingsModifierType::BonusGForces,          0,                20480, 23831, 49648 },
            { RatingsModifierType::BonusTurns,            0,                26749, 34767, 45749 },
            { RatingsModifierType::BonusDrops,            0,                29127, 46811, 49152 },
            { RatingsModifierType::BonusSheltered,        0,                25700, 30583, 35108 },
            { RatingsModifierType::BonusProximity,        0,                20130, 0, 0 },
            { RatingsModifierType::BonusScenery,          0,                9760, 0, 0 },
            { RatingsModifierType::RequirementDropHeight, 8,                2, 2, 2 },
            { RatingsModifierType::RequirementMaxSpeed,   0x70000,          2, 2, 2 },
            { RatingsModifierType::RequirementNumDrops,   1,                2, 2, 2 },
            { RatingsModifierType::RequirementSplashdown, 0,                8, 1, 1 },
            { RatingsModifierType::PenaltyLateralGs,      0,                20480, 23831, 49648 },
        },
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = OpenRCT2::Drawing::LightFx::AddLightsMagicVehicle_BoatHire,
    .StartRideMusic = OpenRCT2::RideAudio::DefaultStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = DefaultMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_WaterCoaster,
};
// clang-format on
