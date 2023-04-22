/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor HeartlineTwisterCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_LIFT_HILL_STEEP, TRACK_SLOPE, TRACK_SLOPE_STEEP_UP, TRACK_SLOPE_STEEP_DOWN, TRACK_HEARTLINE_ROLL, TRACK_HEARTLINE_TRANSFER}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionHeartlineTwisterRC),
    SET_FIELD(Flags, RIDE_TYPE_FLAGS_TRACK_HAS_3_COLOURS | RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION |
                     RIDE_TYPE_FLAGS_COMMON_COASTER | RIDE_TYPE_FLAGS_COMMON_COASTER_NON_ALT | RIDE_TYPE_FLAG_PEEP_CHECK_GFORCES |
                     RIDE_TYPE_FLAG_ALLOW_REVERSED_TRAINS),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 10, 27, 30, 25, 25, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_HEARTLINE_TWISTER_COASTER, STR_RIDE_DESCRIPTION_HEARTLINE_TWISTER_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_HEARTLINE_TWISTER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_RESTRAINTS_STUCK_CLOSED) | (1 << BREAKDOWN_RESTRAINTS_STUCK_OPEN) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 22, 24, 15, 9, }),
    SET_FIELD(MaxMass, 18),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::LiftClassic, 4, 6 }),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 47, 20, 80, 11, 3, 10 }),
    SET_FIELD(BuildCosts, { 72.50_GBP, 3.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 15, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_TECHNO),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 70),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "heartline_twister_rc"),
    SET_FIELD(RatingsData,
    {
        SET_FIELD(Type, RatingsCalculationType::Normal),
#ifdef ORIGINAL_RATINGS
        SET_FIELD(BaseRatings, { RIDE_RATING(1, 40), RIDE_RATING(1, 70), RIDE_RATING(1, 65) }),
#else
        SET_FIELD(BaseRatings, { RIDE_RATING(3, 00), RIDE_RATING(1, 70), RIDE_RATING(1, 65) }),
#endif
        SET_FIELD(Unreliability, 18),
        SET_FIELD(RideShelter, -1),
        SET_FIELD(RelaxRequirementsIfInversions, false),
        SET_FIELD(Modifiers, {
            { RatingsModifierType::BonusLength,           6000, 764, 0, 0 },
            { RatingsModifierType::BonusSynchronization,  0,    RIDE_RATING(0, 20), RIDE_RATING(0, 04), 0 },
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
        }),
    }),
};
// clang-format on
