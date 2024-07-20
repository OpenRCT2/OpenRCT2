/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
constexpr RideTypeDescriptor WaterCoasterRTD =
{
    .Category = RIDE_CATEGORY_ROLLERCOASTER,
    .StartTrackPiece = TrackElemType::EndStation,
    .TrackPaintFunctions = TrackDrawerDescriptor(
        {
            .Drawer = GetTrackPaintFunctionWaterRC,
            .EnabledTrackPieces = {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_SLOPE_CURVE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_CURVE_LARGE, TRACK_HELIX_DOWN_BANKED_HALF, TRACK_HELIX_UP_BANKED_HALF, TRACK_BRAKES, TRACK_BLOCK_BRAKES, TRACK_ON_RIDE_PHOTO, TRACK_BOOSTER, TRACK_DIAG_BRAKES, TRACK_DIAG_BLOCK_BRAKES},
            .ExtraTrackPieces = {TRACK_SLOPE_LONG},
        },
        {
            .Drawer = GetTrackPaintFunctionSplashBoats,
            .EnabledTrackPieces = {TrackElemType::Flat, TrackElemType::LeftQuarterTurn5Tiles, TrackElemType::RightQuarterTurn5Tiles, TrackElemType::SBendLeft, TrackElemType::SBendRight},
            .ExtraTrackPieces = {},
        }
    ),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT |
                     RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES,
    .RideModes = EnumsToFlags(RideMode::ContinuousCircuit, RideMode::ContinuousCircuitBlockSectioned),
    .DefaultMode = RideMode::ContinuousCircuit,
    .TrackSpeedSettings = { 30, 15 },
    .BoosterSettings = { 17, 16 },
    .LegacyBoosterSettings = { 17, 16, 1 },
    .Naming = { STR_RIDE_NAME_WATER_COASTER, STR_RIDE_DESCRIPTION_WATER_COASTER },
    .NameConvention = { RideComponentType::Boat, RideComponentType::Track, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_WATER_COASTER",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION) | (1 << BREAKDOWN_BRAKES_FAILURE),
    .Heights = { 18, 24, 4, 7, },
    .MaxMass = 13,
    .LiftData = { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 60, 20, 80, 8, 3, 10 },
    .BuildCosts = { 27.50_GBP, 2.00_GBP, 40, },
    .DefaultPrices = { 20, 20 },
    .DefaultMusic = MUSIC_OBJECT_WATER,
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
        { RIDE_RATING(2, 70), RIDE_RATING(2, 80), RIDE_RATING(2, 10) },
        14,
        -1,
        false,
        {
            { RatingsModifierType::BonusLength,           6000,             764, 0, 0 },
            { RatingsModifierType::BonusSynchronisation,  0,                RIDE_RATING(0, 40), RIDE_RATING(0, 05), 0 },
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
    .LightFXAddLightsMagicVehicle = LightFxAddLightsMagicVehicle_BoatHire,
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
