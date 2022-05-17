/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioObject.h"

#include "../AssetPackManager.h"
#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../audio/AudioContext.h"
#include "../core/Json.hpp"
#include "../core/Path.hpp"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;

void AudioObject::Load()
{
    auto context = GetContext();
    auto assetManager = context->GetAssetPackManager();
    if (assetManager != nullptr)
    {
        assetManager->LoadSamplesForObject(GetIdentifier(), _sampleTable);
    }
    _sampleTable.Load();
    _sampleTable.LoadSamples();
}

void AudioObject::Unload()
{
    _sampleTable.Unload();
}

void AudioObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "BannerObject::ReadJson expects parameter root to be object");
    _sampleTable.ReadFromJson(context, root);
    PopulateTablesFromJson(context, root);
}

IAudioSource* AudioObject::GetSample(uint32_t index) const
{
    return _sampleTable.GetSample(index);
}

int32_t AudioObject::GetSampleModifier(uint32_t index) const
{
    return _sampleTable.GetSampleModifier(index);
}
