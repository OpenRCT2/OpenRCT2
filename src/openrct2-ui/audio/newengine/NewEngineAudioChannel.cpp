/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewEngineAudioChannel.h"

#include <cmath>

namespace OpenRCT2::Audio
{
    NewEngineAudioChannel::NewEngineAudioChannel(AudioEngine* engine, AudioHandle handle, MixerGroup group)
        : _engine(engine)
        , _handle(handle)
        , _group(group)
    {
    }

    NewEngineAudioChannel::~NewEngineAudioChannel()
    {
        if (_engine != nullptr && _handle.isValid() && !_done && !_stopping)
        {
            _engine->stop(_handle);
        }
    }

    IAudioSource* NewEngineAudioChannel::GetSource() const
    {
        return nullptr;
    }

    MixerGroup NewEngineAudioChannel::GetGroup() const
    {
        return _group;
    }

    void NewEngineAudioChannel::SetGroup(MixerGroup group)
    {
        _group = group;
    }

    double NewEngineAudioChannel::GetRate() const
    {
        return _rate;
    }

    void NewEngineAudioChannel::SetRate(double rate)
    {
        _rate = rate;
    }

    uint64_t NewEngineAudioChannel::GetOffset() const
    {
        return 0;
    }

    bool NewEngineAudioChannel::SetOffset(uint64_t /*offset*/)
    {
        return false;
    }

    int32_t NewEngineAudioChannel::GetLoop() const
    {
        return 0;
    }

    void NewEngineAudioChannel::SetLoop(int32_t /*value*/)
    {
    }

    int32_t NewEngineAudioChannel::GetVolume() const
    {
        return _volume;
    }

    float NewEngineAudioChannel::GetVolumeL() const
    {
        return _volumeL;
    }

    float NewEngineAudioChannel::GetVolumeR() const
    {
        return _volumeR;
    }

    float NewEngineAudioChannel::GetOldVolumeL() const
    {
        return _oldVolumeL;
    }

    float NewEngineAudioChannel::GetOldVolumeR() const
    {
        return _oldVolumeR;
    }

    int32_t NewEngineAudioChannel::GetOldVolume() const
    {
        return _oldVolume;
    }

    void NewEngineAudioChannel::SetVolume(int32_t volume)
    {
        _volume = volume;
        float normalized = static_cast<float>(volume) / static_cast<float>(kMixerVolumeMax);
        if (_engine != nullptr)
            _engine->setVolume(_handle, normalized);
    }

    float NewEngineAudioChannel::GetPan() const
    {
        return _pan;
    }

    void NewEngineAudioChannel::SetPan(float pan)
    {
        _pan = pan;
    }

    bool NewEngineAudioChannel::IsStopping() const
    {
        return _stopping;
    }

    void NewEngineAudioChannel::SetStopping(bool value)
    {
        _stopping = value;
    }

    bool NewEngineAudioChannel::IsDone() const
    {
        return _done;
    }

    void NewEngineAudioChannel::SetDone(bool value)
    {
        _done = value;
    }

    bool NewEngineAudioChannel::DeleteOnDone() const
    {
        return _deleteOnDone;
    }

    void NewEngineAudioChannel::SetDeleteOnDone(bool value)
    {
        _deleteOnDone = value;
    }

    bool NewEngineAudioChannel::IsPlaying() const
    {
        return !IsDone();
    }

    void NewEngineAudioChannel::Play(IAudioSource* /*source*/, int32_t /*loop*/)
    {
        _done = false;
        _stopping = false;
    }

    void NewEngineAudioChannel::Stop()
    {
        _stopping = true;
        _done = true;
        if (_engine != nullptr)
            _engine->stop(_handle);
    }

    void NewEngineAudioChannel::UpdateOldVolume()
    {
        _oldVolume = _volume;
        _oldVolumeL = _volumeL;
        _oldVolumeR = _volumeR;
    }

    size_t NewEngineAudioChannel::Read(void* /*dst*/, size_t /*len*/)
    {
        return 0;
    }

} // namespace OpenRCT2::Audio
