#pragma once

#include "../../localisation/StringIds.h"
#include "../Track.h"
#include "../TrackData.h"

// clang-format off
constexpr const TrackTypeDescriptor FlatToLeftBankTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 69632),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::FlatToRightBank),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor FlatToRightBankTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 69632),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::FlatToLeftBank),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor LeftBankToFlatTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 69632),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::RightBankToFlat),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor RightBankToFlatTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 69632),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::LeftBankToFlat),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor BankedLeftQuarterTurn5TilesTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 273443),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::BankedRightQuarterTurn5Tiles),
    SET_FIELD(Length, 124),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0) | (1 << 6)),
};

constexpr const TrackTypeDescriptor BankedRightQuarterTurn5TilesTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice, 273443),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::BankedLeftQuarterTurn5Tiles),
    SET_FIELD(Length, 124),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0) | (1 << 6)),
};

// clang-format on
#pragma once
