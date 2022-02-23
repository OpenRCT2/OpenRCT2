/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackP.h"

#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../world/TileElementsView.h"
#include "Map.h"

static std::vector<Track> _tracks;

void track_init()
{
    _tracks.clear();
}

void track_reset_broken_index()
{
    for (TrackIndex::UnderlyingType index = 0; index < _tracks.size(); index++)
    {
        const auto trackId = TrackIndex::FromUnderlying(index);
        auto tileElement = track_get_tile_element(trackId);
        if (tileElement == nullptr)
        {
            auto track = GetTrack(trackId);
            if (track != nullptr)
            {
                track->RideIndex = RideId::GetNull();
            }
        }
    }
}

void FixDuplicatedTracks()
{
    // For each track in the map, check if the track index is in use already, and if so, create a new entry for it
    std::vector<bool> activeTracks;
    activeTracks.resize(MAX_TRACKS);

    for (int y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; y++)
    {
        for (int x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; x++)
        {
            const auto trackPos = TileCoordsXY{ x, y }.ToCoordsXY();
            for (auto* trackElement : OpenRCT2::TileElementsView<TrackElement>(trackPos))
            {
                auto trackIndex = trackElement->GetIndex();
                if (trackIndex == TrackIndex::GetNull())
                    continue;

                const auto index = trackIndex.ToUnderlying();
                if (activeTracks[index])
                {
                    log_info(
                        "Duplicated track with index %d found at x = %d, y = %d and z = %d.", index, x, y,
                        trackElement->base_height);

                    // Banner index is already in use by another track, so duplicate it
                    auto newTrack = CreateTrack();
                    if (newTrack == nullptr)
                    {
                        log_error("Failed to create new track element.");
                        continue;
                    }
                    Guard::Assert(!activeTracks[newTrack->id.ToUnderlying()]);

                    // Copy over the original track, but update the location
                    const auto* oldTrack = GetTrack(trackIndex);
                    if (oldTrack != nullptr)
                    {
                        auto newTrackId = newTrack->id;

                        *newTrack = *oldTrack;
                        newTrack->id = newTrackId;
                        // newTrack->position = { x, y };
                    }

                    trackElement->SetIndex(newTrack->id);
                }

                // Mark track index as in-use
                activeTracks[index] = true;
            }
        }
    }
}

Track* GetTrack(TrackIndex id)
{
    const auto index = id.ToUnderlying();
    if (index < _tracks.size())
    {
        auto track = &_tracks[index];
        if (track != nullptr && !track->IsNull())
        {
            return track;
        }
    }
    return nullptr;
}

TileElement* track_get_tile_element(TrackIndex trackIndex)
{
    return nullptr;
}

Track* GetOrCreateTrack(TrackIndex id)
{
    const auto index = id.ToUnderlying();
    if (index < MAX_TRACKS)
    {
        if (index >= _tracks.size())
        {
            _tracks.resize(index + 1);
        }
        // Create the track
        auto& track = _tracks[index];
        track.id = id;
        return &track;
    }
    return nullptr;
}

static TrackIndex TrackGetNewIndex()
{
    for (TrackIndex::UnderlyingType trackIndex = 0; trackIndex < MAX_TRACKS; trackIndex++)
    {
        if (trackIndex < _tracks.size())
        {
            if (_tracks[trackIndex].IsNull())
            {
                return TrackIndex::FromUnderlying(trackIndex);
            }
        }
        else
        {
            _tracks.emplace_back();
            return TrackIndex::FromUnderlying(trackIndex);
        }
    }
    return TrackIndex::GetNull();
}

Track* CreateTrack()
{
    auto trackIndex = TrackGetNewIndex();
    auto track = GetOrCreateTrack(trackIndex);
    if (track != nullptr)
    {
        track->id = trackIndex;
    }
    return track;
}

void DeleteTrack(TrackIndex id)
{
    auto* const banner = GetTrack(id);
    if (banner != nullptr)
    {
        *banner = {};
    }
}

void RemoveAllTracksForRide(RideId rideId)
{
    for (auto& track : _tracks)
    {
        if (!track.IsNull() && track.RideIndex == rideId)
        {
            *(&track) = {};
        }
    }
}

void TrimTracks()
{
    if (_tracks.size() > 0)
    {
        auto lastBannerId = _tracks.size() - 1;
        while (lastBannerId != std::numeric_limits<size_t>::max() && _tracks[lastBannerId].IsNull())
        {
            lastBannerId--;
        }
        _tracks.resize(lastBannerId + 1);
        _tracks.shrink_to_fit();
    }
}

size_t GetNumTracks()
{
    size_t count = 0;
    for (const auto& track : _tracks)
    {
        if (!track.IsNull())
        {
            count++;
        }
    }
    return count;
}

bool HasReachedTrackLimit()
{
    auto numTracks = GetNumBanners();
    return numTracks >= MAX_TRACKS;
}
