/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"

constexpr size_t MAX_TRACKS = 15775215;

struct TileElement;
struct TrackElement;
class DataSerialiser;

struct Track
{
    TrackIndex id = TrackIndex::GetNull();

    RideId RideIndex = RideId::GetNull();
    uint8_t BrakeBoosterSpeed{};
    uint8_t SeatRotation{};
    uint16_t Flags3{};
    StationIndex stationIndex{};
    uint8_t pad[5]{};

    bool IsNull() const
    {
        return id == TrackIndex::GetNull();
    }
    // Copy all properties except id
    void Clone(Track& src);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newIndex);

    uint8_t GetBrakeBoosterSpeed();
    void SetBrakeBoosterSpeed(uint8_t newSpeed);

    bool GetIsIndestructible() const;
    void SetIsIndestrutible(bool newIsIndestructible);

    StationIndex GetStationIndex() const;
    void SetStationIndex(StationIndex newStationIndex);

    void Serialise(DataSerialiser& stream);
};
assert_struct_size(Track, 16);

void track_init();
TileElement* track_get_tile_element(TrackIndex trackIndex);
void track_reset_broken_index();
void FixDuplicatedTracks();
void RemoveAllTracksForRide(RideId rideId);
Track* GetTrack(TrackIndex id);
Track* GetOrCreateTrack(TrackIndex id);
Track* CreateTrack();
void DeleteTrack(TrackIndex id);
void TrimTracks();
size_t GetNumTracks();
bool HasReachedTrackLimit();
