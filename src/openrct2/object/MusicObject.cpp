/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MusicObject.h"

#include "../AssetPackManager.h"
#include "../Context.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../audio/AudioContext.h"
#include "../audio/AudioSource.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../drawing/Image.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "RideObject.h"

#include <memory>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

constexpr size_t kDefaultBytesPerTick = 1378;

void MusicObject::Load()
{
    GetStringTable().Sort();
    NameStringId = LanguageAllocateObjectString(GetName());

    // Start with base images
    _loadedSampleTable.LoadFrom(_sampleTable, 0, _sampleTable.GetCount());

    // Override samples from asset packs
    auto context = GetContext();
    auto assetManager = context->GetAssetPackManager();
    if (assetManager != nullptr)
    {
        assetManager->LoadSamplesForObject(GetIdentifier(), _loadedSampleTable);
    }

    // Load metadata of samples
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
                track.BytesPerTick = kDefaultBytesPerTick;
                track.Size = track.Asset.GetSize();
            }
        }
        else
        {
            track.BytesPerTick = kDefaultBytesPerTick;
            track.Size = track.Asset.GetSize();
        }
    }

    _hasPreview = !!GetImageTable().GetCount();
    _previewImageId = LoadImages();
}

void MusicObject::Unload()
{
    LanguageFreeObjectString(NameStringId);
    UnloadImages();

    _hasPreview = false;
    _previewImageId = 0;
    NameStringId = 0;
}

void MusicObject::DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const
{
    // Write (no image)
    int32_t x = width / 2;
    int32_t y = height / 2;
    if (_hasPreview)
        GfxDrawSprite(dpi, ImageId(_previewImageId), { 0, 0 });
    else
        DrawTextBasic(dpi, { x, y }, STR_WINDOW_NO_IMAGE, {}, { TextAlignment::CENTRE });
}

bool MusicObject::HasPreview() const
{
    return _hasPreview;
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
    auto& entries = _sampleTable.GetEntries();
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
                auto asset = GetAsset(context, source);

                auto& entry = entries.emplace_back();
                entry.Asset = asset;

                track.Asset = asset;
                _tracks.push_back(std::move(track));
            }
        }
    }
}

std::optional<uint8_t> MusicObject::GetOriginalStyleId() const
{
    return _originalStyleId;
}

bool MusicObject::SupportsRideType(ride_type_t rideType)
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

IAudioSource* MusicObject::GetTrackSample(size_t trackIndex) const
{
    return _loadedSampleTable.LoadSample(static_cast<uint32_t>(trackIndex));
}

ObjectAsset MusicObject::GetAsset(IReadObjectContext& context, std::string_view path)
{
    if (path.find("$RCT2:DATA/") == 0)
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto path2 = env->FindFile(DIRBASE::RCT2, DIRID::DATA, path.substr(11));
        return ObjectAsset(path2);
    }

    return context.GetAsset(path);
}
