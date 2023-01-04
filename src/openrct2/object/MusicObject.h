/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "AudioSampleTable.h"
#include "Object.h"

#include <string>
#include <vector>

class MusicObjectTrack
{
public:
    std::string Name;
    std::string Composer;
    ObjectAsset Asset;

    /**
     * The number of PCM bytes to seek per game tick when the music is playing offscreen.
     */
    size_t BytesPerTick;

    /**
     * The length of the PCM track in bytes.
     */
    size_t Size;
};

enum class MusicNiceFactor : int8_t
{
    Overbearing = -1, // Drowns out other music
    Neutral = 0,
    Nice = 1,
};

class MusicObject final : public Object
{
private:
    std::vector<uint8_t> _rideTypes;
    std::vector<MusicObjectTrack> _tracks;
    std::optional<uint8_t> _originalStyleId;
    MusicNiceFactor _niceFactor;
    AudioSampleTable _sampleTable;
    AudioSampleTable _loadedSampleTable;
    bool _hasPreview{};
    uint32_t _previewImageId{};

public:
    StringId NameStringId{};

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;

    std::optional<uint8_t> GetOriginalStyleId() const;
    bool SupportsRideType(ride_type_t rideType);
    size_t GetTrackCount() const;
    const MusicObjectTrack* GetTrack(size_t trackIndex) const;
    OpenRCT2::Audio::IAudioSource* GetTrackSample(size_t trackIndex) const;
    constexpr MusicNiceFactor GetNiceFactor() const
    {
        return _niceFactor;
    }

private:
    void ParseRideTypes(const json_t& jRideTypes);
    void ParseTracks(IReadObjectContext& context, json_t& jTracks);
    static ObjectAsset GetAsset(IReadObjectContext& context, std::string_view path);
};
