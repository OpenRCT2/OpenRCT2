/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioSampleTable.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../audio/AudioContext.h"
#include "../core/File.h"
#include "../core/Json.hpp"
#include "../core/Path.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

struct SourceInfo
{
    std::string Path;
    std::vector<int32_t> Range{};
};

static std::vector<int32_t> ParseRange(std::string_view s)
{
    // Currently only supports [###] or [###..###]
    std::vector<int32_t> result = {};
    if (s.length() >= 3 && s[0] == '[' && s[s.length() - 1] == ']')
    {
        s = s.substr(1, s.length() - 2);
        auto parts = String::Split(s, "..");
        if (parts.size() == 1)
        {
            result.push_back(std::stoi(parts[0]));
        }
        else
        {
            auto left = std::stoi(parts[0]);
            auto right = std::stoi(parts[1]);
            if (left <= right)
            {
                for (auto i = left; i <= right; i++)
                {
                    result.push_back(i);
                }
            }
            else
            {
                for (auto i = right; i >= left; i--)
                {
                    result.push_back(i);
                }
            }
        }
    }
    return result;
}

static SourceInfo ParseSource(std::string_view source)
{
    SourceInfo info;
    if (String::StartsWith(source, "$RCT1:DATA/"))
    {
        auto name = source.substr(11);
        auto rangeStart = name.find('[');
        if (rangeStart != std::string::npos)
        {
            info.Range = ParseRange(name.substr(rangeStart));
            name = name.substr(0, rangeStart);
        }

        auto env = GetContext()->GetPlatformEnvironment();
        info.Path = env->FindFile(DIRBASE::RCT1, DIRID::DATA, name);
    }
    else if (String::StartsWith(source, "$RCT2:DATA/"))
    {
        auto name = source.substr(11);
        auto rangeStart = name.find('[');
        if (rangeStart != std::string::npos)
        {
            info.Range = ParseRange(name.substr(rangeStart));
            name = name.substr(0, rangeStart);
        }

        auto env = GetContext()->GetPlatformEnvironment();
        info.Path = env->FindFile(DIRBASE::RCT2, DIRID::DATA, name);
    }
    else if (String::StartsWith(source, "$["))
    {
        info.Range = ParseRange(source.substr(1));
    }
    else
    {
        info.Path = source;
    }
    return info;
}

void AudioSampleTable::ReadFromJson(IReadObjectContext* context, const json_t& root)
{
    json_t jSamples = root["samples"];
    if (jSamples.is_array())
    {
        for (auto& jSample : jSamples)
        {
            SourceInfo sourceInfo;
            int32_t modifier{};
            if (jSample.is_string())
            {
                sourceInfo = ParseSource(jSample.get<std::string>());
            }
            else if (jSample.is_object())
            {
                sourceInfo = ParseSource(jSample.at("source").get<std::string>());
                if (jSample.contains("modifier"))
                {
                    modifier = jSample.at("modifier").get<int32_t>();
                }
            }

            auto asset = context->GetAsset(sourceInfo.Path);
            if (sourceInfo.Range.empty())
            {
                auto& entry = _entries.emplace_back();
                entry.Asset = asset;
                entry.Modifier = modifier;
            }
            else
            {
                for (auto index : sourceInfo.Range)
                {
                    auto& entry = _entries.emplace_back();
                    entry.Asset = asset;
                    entry.PathIndex = index;
                    entry.Modifier = modifier;
                }
            }
        }
    }
}

void AudioSampleTable::LoadFrom(const AudioSampleTable& table, size_t index, size_t length)
{
    auto audioContext = GetContext()->GetAudioContext();
    auto numEntries = std::min(_entries.size(), length);
    for (size_t i = 0; i < numEntries; i++)
    {
        auto& entry = _entries[i];
        if (entry.Source != nullptr)
        {
            continue;
        }

        auto sourceIndex = index + i;
        if (sourceIndex >= table._entries.size())
        {
            continue;
        }

        const auto& sourceEntry = table._entries[sourceIndex];
        if (sourceEntry.Asset)
        {
            auto stream = sourceEntry.Asset->GetStream();
            if (stream != nullptr)
            {
                if (sourceEntry.PathIndex)
                {
                    entry.Source = audioContext->CreateStreamFromCSS(std::move(stream), *sourceEntry.PathIndex);
                }
                else
                {
                    entry.Source = audioContext->CreateStreamFromWAV(std::move(stream));
                }
                entry.Modifier = sourceEntry.Modifier;
            }
        }
    }
}

void AudioSampleTable::Load()
{
    LoadFrom(*this, 0, _entries.size());
}

void AudioSampleTable::Unload()
{
    for (auto& entry : _entries)
    {
        if (entry.Source != nullptr)
        {
            entry.Source->Release();
            entry.Source = nullptr;
        }
    }
}

size_t AudioSampleTable::GetCount() const
{
    return _entries.size();
}

IAudioSource* AudioSampleTable::GetSample(uint32_t index) const
{
    if (index < _entries.size())
    {
        return _entries[index].Source;
    }
    return nullptr;
}

int32_t AudioSampleTable::GetSampleModifier(uint32_t index) const
{
    if (index < _entries.size())
    {
        return _entries[index].Modifier;
    }
    return 0;
}
