/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/RideTypes.h"
#include "AudioSampleTable.h"
#include "Object.h"

#include <string>
#include <vector>

namespace OpenRCT2
{
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
        static constexpr ObjectType kObjectType = ObjectType::music;

        StringId NameStringId{};

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;
        bool HasPreview() const;

        std::optional<uint8_t> GetOriginalStyleId() const;
        /**
         *
         * @param rideType
         * @param onlyAllowIfExplicitlyListed If a music object does not provide a list of ride types,
         *  it will be enabled for all ride types, unless this parameter is set to true.
         * @return
         */
        bool SupportsRideType(ride_type_t rideType, bool onlyAllowIfExplicitlyListed);
        size_t GetTrackCount() const;
        const MusicObjectTrack* GetTrack(size_t trackIndex) const;
        Audio::IAudioSource* GetTrackSample(size_t trackIndex) const;
        constexpr MusicNiceFactor GetNiceFactor() const
        {
            return _niceFactor;
        }

    private:
        void ParseRideTypes(const json_t& jRideTypes);
        void ParseTracks(IReadObjectContext& context, json_t& jTracks);
        static ObjectAsset GetAsset(IReadObjectContext& context, std::string_view path);
    };
} // namespace OpenRCT2
