/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../audio/AudioSource.h"
#include "../core/IStream.hpp"
#include "AudioSampleTable.h"
#include "Object.h"

#include <optional>

class AudioObject final : public Object
{
private:
    AudioSampleTable _sampleTable;

public:
    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    OpenRCT2::Audio::IAudioSource* GetSample(uint32_t index) const;
    int32_t GetSampleModifier(uint32_t index) const;
};
