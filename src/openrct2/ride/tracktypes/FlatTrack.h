#pragma once

#include "../../localisation/StringIds.h"
#include "../Track.h"
#include "../TrackData.h"

// clang-format off

// note: TrackHeightMarkerPositions is a bitfield denoting
// which sequence pieces get a height marker flag above them
constexpr const TrackTypeDescriptor FlatTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice,65536),
    SET_FIELD(AlternateTrackType, TrackElemType::FlatCovered),
    SET_FIELD(MirroredTrackType, TrackElemType::Flat),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor EndStationTTD = {
    SET_FIELD(TrackName,STR_STATION_PLATFORM),
    SET_FIELD(TrackPrice,98304),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::EndStation),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor BeginStationTTD = {
    SET_FIELD(TrackName,STR_STATION_PLATFORM),
    SET_FIELD(TrackPrice,98304),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::BeginStation),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor MiddleStationTTD = {
    SET_FIELD(TrackName,STR_STATION_PLATFORM),
    SET_FIELD(TrackPrice,98304),
    SET_FIELD(AlternateTrackType, -1),
    SET_FIELD(MirroredTrackType, TrackElemType::MiddleStation),
    SET_FIELD(Length, 32),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0)),
};

constexpr const TrackTypeDescriptor LeftQuarterTurn5TilesTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice,257359),
    SET_FIELD(AlternateTrackType, TrackElemType::LeftQuarterTurn5TilesCovered),
    SET_FIELD(MirroredTrackType, TrackElemType::RightQuarterTurn5Tiles),
    SET_FIELD(Length, 124),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0) | (1 << 6)),
};

constexpr const TrackTypeDescriptor RightQuarterTurn5TilesTTD = {
    SET_FIELD(TrackName,STR_RIDE_COMPONENT_TRACK),
    SET_FIELD(TrackPrice,257359),
    SET_FIELD(AlternateTrackType, TrackElemType::RightQuarterTurn5TilesCovered),
    SET_FIELD(MirroredTrackType, TrackElemType::LeftQuarterTurn5Tiles),
    SET_FIELD(Length, 124),
    SET_FIELD(TrackHeightMarkerPositions, (1 << 0) | (1 << 6)),
};

// clang-format on
