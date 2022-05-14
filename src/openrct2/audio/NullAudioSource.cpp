/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioSource.h"
#include "audio.h"

namespace OpenRCT2::Audio
{
    /**
     * An audio source representing silence.
     */
    class NullAudioSource : public IAudioSource
    {
    public:
        void Release() override
        {
        }

        int32_t GetBytesPerSecond() const override
        {
            return 0;
        }

        uint64_t GetLength() const override
        {
            return 0;
        }

        size_t Read([[maybe_unused]] void* dst, [[maybe_unused]] uint64_t offset, [[maybe_unused]] size_t len) override
        {
            return 0;
        }
    };

    IAudioSource* AudioSource::CreateNull()
    {
        return new NullAudioSource();
    }
} // namespace OpenRCT2::Audio
