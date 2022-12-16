/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
constexpr const RideTypeDescriptor GhostTrainRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_SLOPE, TRACK_CURVE_VERY_SMALL, TRACK_CURVE_SMALL, TRACK_BRAKES, TRACK_SPINNING_TUNNEL}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionGhostTrain),
    SET_FIELD(Flags, { TrackHasThreeColours, {
        RideTypeFlags::CanSynchroniseAdjacentStations,
        RideTypeFlags::HasGForces,
        RideTypeFlags::HasDataLogging,
        RideTypeFlags::HasDrops,
        RideTypeFlags::HasLoadOptions,
        RideTypeFlags::PeepWillRideAgain,
        RideTypeFlags::HasVehicleColours,
        RideTypeFlags::HasTrack,
        RideTypeFlags::SupportsMultipleTrackColour,
        RideTypeFlags::AllowDoorsOnTrack,
        RideTypeFlags::AllowMusic,
        RideTypeFlags::HasEntranceExit,
        RideTypeFlags::AllowMoreVehiclesThanStationFits,
        RideTypeFlags::HasAirTime,
        RideTypeFlags::ShowInTrackDesigner,
        RideTypeFlags::InterestingToLookAt,
        RideTypeFlags::HasLandscapeDoors } } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_GHOST_TRAIN, STR_RIDE_DESCRIPTION_GHOST_TRAIN }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_GHOST_TRAIN)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 8, 24, 6, 7, }),
    SET_FIELD(MaxMass, 2),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_ghost_train),
    SET_FIELD(RatingsMultipliers, { 70, 10, 10 }),
    SET_FIELD(UpkeepCosts, { 80, 20, 0, 8, 3, 5 }),
    SET_FIELD(BuildCosts, { 13.00_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_HORROR),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 50),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_TRACK, SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "ghost_train"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, LightFxAddLightsMagicVehicle_MineTrainCoaster),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
    SET_FIELD(MusicUpdateFunction, DefaultMusicUpdate),
    SET_FIELD(Classification, RideClassification::Ride),
    SET_FIELD(UpdateLeaveEntrance, PeepUpdateRideLeaveEntranceDefault),
    SET_FIELD(SpecialElementRatingAdjustment, SpecialTrackElementRatingsAjustment_GhostTrain),
};
// clang-format on
