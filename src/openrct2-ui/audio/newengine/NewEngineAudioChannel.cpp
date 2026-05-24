/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewEngineAudioChannel.h"

namespace OpenRCT2::Audio
{
    NewEngineAudioChannel::NewEngineAudioChannel(
        AudioEngine* engine, AudioHandle handle, MixerGroup group, uint8_t channels, uint64_t lengthInFrames,
        size_t sourceBytesPerFrame)
        : _engine(engine)
        , _handle(handle)
        , _group(group)
        , _channels(channels)
        , _lengthInFrames(lengthInFrames)
        , _sourceBytesPerFrame(sourceBytesPerFrame)
    {
    }

    NewEngineAudioChannel::~NewEngineAudioChannel()
    {
        if (_engine != nullptr && _handle.isValid() && !_done && !_stopping)
        {
            _engine->stop(_handle);
        }
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
        if (_engine != nullptr)
            _engine->setRate(_handle, static_cast<float>(rate));
    }

    uint64_t NewEngineAudioChannel::GetOffset() const
    {
        if (_engine == nullptr || !_handle.isValid())
            return 0;

        uint64_t positionInFrames = _engine->getOffset(_handle);
        return positionInFrames * _sourceBytesPerFrame;
    }

    bool NewEngineAudioChannel::SetOffset(uint64_t offset)
    {
        if (_engine == nullptr || !_handle.isValid())
            return false;

        uint64_t offsetInFrames = offset / _sourceBytesPerFrame;

        if (offsetInFrames >= _lengthInFrames)
            return false;

        _engine->setOffset(_handle, offsetInFrames);
        return true;
    }

    int32_t NewEngineAudioChannel::GetVolume() const
    {
        return _volume;
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
        if (_engine != nullptr)
            _engine->setPan(_handle, pan);
    }

    bool NewEngineAudioChannel::IsDone() const
    {
        if (_done)
            return true;
        if (_engine != nullptr && _handle.isValid())
        {
            bool active = _engine->isHandleActive(_handle);
            if (active)
                _pendingActivation = false;
            return !active && !_pendingActivation;
        }
        return true;
    }

    bool NewEngineAudioChannel::IsPlaying() const
    {
        return !IsDone();
    }

    void NewEngineAudioChannel::Stop()
    {
        _stopping = true;
        // Aaa... Don't set _done = true here. The engine performs a 5ms fade-out,
        // and IsDone() correctly checks isHandleActive() to detect when the
        // voice has actually finished. Setting _done prematurely caused the game
        // code to destroy resources while audio was still playing. NOT GOOD.
        if (_engine != nullptr)
            _engine->stop(_handle);
    }

    // -----------
    // TODO: Remove. Legacy mixer interface stubs that aren't called by game code...
    // The new engine handles these differently (voice pool, internal ramping,
    // push-based mixing). Kept for IAudioChannel interface compat, but these should
    // all be removed when the Legacy engine is yeeted.

    IAudioSource* NewEngineAudioChannel::GetSource() const
    {
        return nullptr;
    }

    int32_t NewEngineAudioChannel::GetLoop() const
    {
        return 0;
    }

    void NewEngineAudioChannel::SetLoop(int32_t /*value*/)
    {
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

    bool NewEngineAudioChannel::IsStopping() const
    {
        return _stopping;
    }

    void NewEngineAudioChannel::SetStopping(bool value)
    {
        _stopping = value;
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

    void NewEngineAudioChannel::Play(IAudioSource* /*source*/, int32_t /*loop*/)
    {
        _done = false;
        _stopping = false;
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
