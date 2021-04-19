/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

#include <string>
#include <vector>

class MusicObjectTrack
{
public:
    std::string Name;
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

class MusicObject final : public Object
{
private:
    std::vector<uint8_t> _rideTypes;
    std::vector<MusicObjectTrack> _tracks;
    std::optional<uint8_t> _originalStyleId;

public:
    rct_string_id NameStringId{};

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    void DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const override;

    std::optional<uint8_t> GetOriginalStyleId() const;
    bool SupportsRideType(uint8_t rideType);
    size_t GetTrackCount() const;
    const MusicObjectTrack* GetTrack(size_t trackIndex) const;

private:
    void ParseRideTypes(const json_t& jRideTypes);
    void ParseTracks(IReadObjectContext& context, json_t& jTracks);
    static ObjectAsset GetAsset(IReadObjectContext& context, std::string_view path);
};
