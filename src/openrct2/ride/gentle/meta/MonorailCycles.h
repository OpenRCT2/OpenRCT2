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
constexpr const RideTypeDescriptor MonorailCyclesRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_GENTLE),
    SET_FIELD(EnabledTrackPieces, {TRACK_STRAIGHT, TRACK_STATION_END, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, GetTrackPaintFunctionMonorailCycles),
    SET_FIELD(Flags, { TrackHasThreeColours, {
        RideTypeFlags::CanSynchroniseAdjacentStations,
        RideTypeFlags::HasDataLogging,
        RideTypeFlags::NoTestMode,
        RideTypeFlags::HasLoadOptions,
        RideTypeFlags::PeepWillRideAgain,
        RideTypeFlags::HasVehicleColours,
        RideTypeFlags::HasTrack,
        RideTypeFlags::SupportsMultipleTrackColour,
        RideTypeFlags::AllowMusic,
        RideTypeFlags::HasEntranceExit,
        RideTypeFlags::SlightlyInterestingToLookAt } } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_MONORAIL_CYCLES, STR_RIDE_DESCRIPTION_MONORAIL_CYCLES }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_MONORAIL_CYCLES)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, { 5, 24, 8, 7, }),
    SET_FIELD(MaxMass, 2),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_monorail_cycles),
    SET_FIELD(RatingsMultipliers, { 50, 10, 10 }),
    SET_FIELD(UpkeepCosts, { 47, 20, 0, 8, 3, 5 }),
    SET_FIELD(BuildCosts, { 10.00_GBP, 2.50_GBP, 30, }),
    SET_FIELD(DefaultPrices, { 15, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_SUMMER),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 45),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_GREY, COLOUR_DARK_BROWN },
        { COLOUR_GREY, COLOUR_BLACK, COLOUR_GREY },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_TRACK, SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "monorail_cycles"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, LightFxAddLightsMagicVehicle_BoatHire),
};
// clang-format on
