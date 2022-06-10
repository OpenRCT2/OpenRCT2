/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MusicObject.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../audio/AudioContext.h"
#include "../audio/AudioSource.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "RideObject.h"

#include <memory>

using namespace OpenRCT2;

constexpr size_t DEFAULT_BYTES_PER_TICK = 1378;

void MusicObject::Load()
{
    GetStringTable().Sort();
    NameStringId = language_allocate_object_string(GetName());

    auto audioContext = GetContext()->GetAudioContext();
    for (auto& track : _tracks)
    {
        auto stream = track.Asset.GetStream();
        if (stream != nullptr)
        {
            auto source = audioContext->CreateStreamFromWAV(std::move(stream));
            if (source != nullptr)
            {
                track.BytesPerTick = source->GetBytesPerSecond() / 40;
                track.Size = source->GetLength();
                source->Release();
            }
            else
            {
                track.BytesPerTick = DEFAULT_BYTES_PER_TICK;
                track.Size = track.Asset.GetSize();
            }
        }
        else
        {
            track.BytesPerTick = DEFAULT_BYTES_PER_TICK;
            track.Size = track.Asset.GetSize();
        }
    }
}

void MusicObject::Unload()
{
    language_free_object_string(NameStringId);
    NameStringId = 0;
}

void MusicObject::DrawPreview(rct_drawpixelinfo* dpi, int32_t width, int32_t height) const
{
    // Write (no image)
    int32_t x = width / 2;
    int32_t y = height / 2;
    DrawTextBasic(dpi, { x, y }, STR_WINDOW_NO_IMAGE, {}, { TextAlignment::CENTRE });
}

void MusicObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    _originalStyleId = {};
    _rideTypes.clear();
    _tracks.clear();
    _niceFactor = MusicNiceFactor::Neutral;

    auto& properties = root["properties"];
    if (properties != nullptr)
    {
        const auto& originalStyleId = properties["originalStyleId"];
        if (originalStyleId.is_number_integer())
        {
            _originalStyleId = originalStyleId.get<uint8_t>();
        }

        const auto& niceFactor = properties["niceFactor"];
        if (niceFactor.is_number_integer())
        {
            _niceFactor = static_cast<MusicNiceFactor>(std::clamp<int8_t>(niceFactor.get<int8_t>(), -1, 1));
        }

        const auto& jRideTypes = properties["rideTypes"];
        if (jRideTypes.is_array())
        {
            ParseRideTypes(jRideTypes);
        }

        auto& jTracks = properties["tracks"];
        if (jTracks.is_array())
        {
            ParseTracks(*context, jTracks);
        }
    }

    PopulateTablesFromJson(context, root);
}

void MusicObject::ParseRideTypes(const json_t& jRideTypes)
{
    for (const auto& jRideType : jRideTypes)
    {
        auto szRideType = Json::GetString(jRideType);
        if (!szRideType.empty())
        {
            auto rideType = RideObject::ParseRideType(szRideType);
            if (rideType != RIDE_TYPE_NULL)
            {
                _rideTypes.push_back(rideType);
            }
        }
    }
}

void MusicObject::ParseTracks(IReadObjectContext& context, json_t& jTracks)
{
    for (auto& jTrack : jTracks)
    {
        if (jTrack.is_object())
        {
            MusicObjectTrack track;
            track.Name = Json::GetString(jTrack["name"]);
            track.Composer = Json::GetString(jTrack["composer"]);
            auto source = Json::GetString(jTrack["source"]);
            if (source.empty())
            {
                context.LogError(ObjectError::InvalidProperty, "Invalid audio track definition.");
            }
            else
            {
                track.Asset = GetAsset(context, source);
                _tracks.push_back(std::move(track));
            }
        }
    }
}

std::optional<uint8_t> MusicObject::GetOriginalStyleId() const
{
    return _originalStyleId;
}

bool MusicObject::SupportsRideType(uint8_t rideType)
{
    if (_rideTypes.size() == 0)
    {
        // Default behaviour for music is to only exclude from merry-go-round
        return rideType != RIDE_TYPE_MERRY_GO_ROUND;
    }

    auto it = std::find(_rideTypes.begin(), _rideTypes.end(), rideType);
    return it != _rideTypes.end();
}

size_t MusicObject::GetTrackCount() const
{
    return _tracks.size();
}

const MusicObjectTrack* MusicObject::GetTrack(size_t trackIndex) const
{
    if (_tracks.size() > trackIndex)
    {
        return &_tracks[trackIndex];
    }
    return {};
}

ObjectAsset MusicObject::GetAsset(IReadObjectContext& context, std::string_view path)
{
    if (path.find("$RCT2:DATA/") == 0)
    {
        auto platformEnvironment = GetContext()->GetPlatformEnvironment();
        auto dir = platformEnvironment->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
        auto path2 = Path::Combine(dir, std::string(path.substr(11)));
        return ObjectAsset(path2);
    }

    return context.GetAsset(path);
}
