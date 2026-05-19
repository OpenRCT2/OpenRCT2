/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioEngine.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <openrct2/Diagnostic.h>

namespace OpenRCT2::Audio
{
    AudioEngine::AudioEngine()
    {
    }

    AudioEngine::~AudioEngine()
    {
    }

    bool AudioEngine::submitCommand(const AudioCommand& cmd)
    {
        if (_commandBuffer.tryPush(cmd))
            return true;

        uint64_t count = _droppedCommands.fetch_add(1, std::memory_order_relaxed) + 1;
        if ((count & (count - 1)) == 0 || count <= 4)
            LOG_WARNING("Audio command buffer full, dropped command (total: %llu)", count);
        return false;
    }

    AudioHandle AudioEngine::playOneShot(
        const float* pcmData, uint64_t lengthInFrames, uint32_t sampleRate, uint8_t channels, float volume, float pan,
        AudioEngineGroup group)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::playOneShot;
        cmd.data.playOneShot.pcmData = pcmData;
        cmd.data.playOneShot.pcmLengthInFrames = lengthInFrames;
        cmd.data.playOneShot.sampleRate = sampleRate;
        cmd.data.playOneShot.channels = channels;
        cmd.data.playOneShot.volume = volume;
        cmd.data.playOneShot.pan = pan;
        cmd.data.playOneShot.group = group;

        submitCommand(cmd);
        return { AudioHandle::kInvalid };
    }

    AudioHandle AudioEngine::playTracked(
        const float* pcmData, uint64_t lengthInFrames, uint32_t sampleRate, uint8_t channels, float volume, float pan,
        float rate, AudioEngineGroup group, bool looping)
    {
        static std::atomic<uint32_t> sNextHandle{ 1 };
        uint32_t handleVal = sNextHandle.fetch_add(1, std::memory_order_relaxed);
        if (handleVal == AudioHandle::kInvalid)
            handleVal = sNextHandle.fetch_add(1, std::memory_order_relaxed);

        AudioHandle handle{ handleVal };

        AudioCommand cmd{};
        cmd.type = AudioCommandType::playLoop;
        cmd.data.playLoop.pcmData = pcmData;
        cmd.data.playLoop.pcmLengthInFrames = lengthInFrames;
        cmd.data.playLoop.sampleRate = sampleRate;
        cmd.data.playLoop.channels = channels;
        cmd.data.playLoop.volume = volume;
        cmd.data.playLoop.pan = pan;
        cmd.data.playLoop.rate = rate;
        cmd.data.playLoop.group = group;
        cmd.data.playLoop.handle = handle;
        cmd.data.playLoop.looping = looping;

        if (!submitCommand(cmd))
            return { AudioHandle::kInvalid };
        return handle;
    }

    void AudioEngine::stop(AudioHandle handle)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::stop;
        cmd.data.stop.handle = handle;
        submitCommand(cmd);
    }

    void AudioEngine::stopAll()
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::stopAll;
        submitCommand(cmd);
    }

    void AudioEngine::setVolume(AudioHandle handle, float volume)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setVolume;
        cmd.data.setVolume.handle = handle;
        cmd.data.setVolume.volume = volume;
        submitCommand(cmd);
    }

    void AudioEngine::setPan(AudioHandle handle, float pan)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setPan;
        cmd.data.setPan.handle = handle;
        cmd.data.setPan.pan = pan;
        submitCommand(cmd);
    }

    void AudioEngine::setRate(AudioHandle handle, float rate)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setRate;
        cmd.data.setRate.handle = handle;
        cmd.data.setRate.rate = rate;
        submitCommand(cmd);
    }

    void AudioEngine::setMasterVolume(float volume)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setMasterVolume;
        cmd.data.setMasterVolume.volume = volume;
        submitCommand(cmd);
    }

    void AudioEngine::setGroupVolume(AudioEngineGroup group, float volume)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setGroupVolume;
        cmd.data.setGroupVolume.group = group;
        cmd.data.setGroupVolume.volume = volume;
        submitCommand(cmd);
    }

    void AudioEngine::fadeOut(AudioHandle handle, float durationMs)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::fadeOut;
        cmd.data.fadeOut.handle = handle;
        cmd.data.fadeOut.durationMs = durationMs;
        submitCommand(cmd);
    }

    void AudioEngine::stopGroup(AudioEngineGroup group)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::stopGroup;
        cmd.data.stopGroup.group = group;
        submitCommand(cmd);
    }

    void AudioEngine::render(float* outputBuffer, size_t framesRequested, uint32_t outputSampleRate)
    {
        _outputSampleRate = outputSampleRate;

        if (framesRequested == 0)
            return;

        auto t0 = std::chrono::high_resolution_clock::now();

        std::memset(outputBuffer, 0, framesRequested * 2 * sizeof(float));

        processCommands();

        size_t culled = 0;
        mixAllVoices(outputBuffer, framesRequested, outputSampleRate, culled);

        // master volume + hard clip
        size_t totalSamples = framesRequested * 2;
        for (size_t i = 0; i < totalSamples; i++)
        {
            outputBuffer[i] *= _masterVolume;
            if (outputBuffer[i] > 1.0f)
                outputBuffer[i] = 1.0f;
            else if (outputBuffer[i] < -1.0f)
                outputBuffer[i] = -1.0f;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        float durationUs = std::chrono::duration<float, std::micro>(t1 - t0).count();

        updateGovernor(durationUs, framesRequested);

        _stats.activeVoices = _voicePool.activeCount();
        _stats.culledVoices = culled;
        _stats.voiceLimit = _voicePool.getVoiceLimit();
        _stats.lastCallbackDurationUs = durationUs;
    }

    void AudioEngine::convertToDevice(const float* mixBuffer, uint8_t* dst, size_t frames, AudioSampleFormat format)
    {
        size_t totalSamples = frames * 2;

        switch (format)
        {
            case AudioSampleFormat::s16:
            {
                auto* out = reinterpret_cast<int16_t*>(dst);
                for (size_t i = 0; i < totalSamples; i++)
                {
                    float s = mixBuffer[i] * 32767.0f;
                    if (s > 32767.0f)
                        s = 32767.0f;
                    if (s < -32768.0f)
                        s = -32768.0f;
                    out[i] = static_cast<int16_t>(s);
                }
                break;
            }
            case AudioSampleFormat::f32:
            {
                std::memcpy(dst, mixBuffer, totalSamples * sizeof(float));
                break;
            }
            default:
            {
                auto* out = reinterpret_cast<int16_t*>(dst);
                for (size_t i = 0; i < totalSamples; i++)
                {
                    float s = mixBuffer[i] * 32767.0f;
                    out[i] = static_cast<int16_t>(std::clamp(s, -32768.0f, 32767.0f));
                }
                break;
            }
        }
    }

    void AudioEngine::processCommands()
    {
        AudioCommand cmd{};
        while (_commandBuffer.tryPop(cmd))
        {
            switch (cmd.type)
            {
                case AudioCommandType::playOneShot:
                    processPlayOneShot(cmd);
                    break;
                case AudioCommandType::playLoop:
                    processPlayLoop(cmd);
                    break;
                case AudioCommandType::stop:
                    processStop(cmd);
                    break;
                case AudioCommandType::stopAll:
                    _voicePool.releaseAll();
                    for (size_t i = 0; i < kMaxVoices; i++)
                        markSlotInactive(i);
                    break;
                case AudioCommandType::setVolume:
                    processSetVolume(cmd);
                    break;
                case AudioCommandType::setMasterVolume:
                    processSetMasterVolume(cmd);
                    break;
                case AudioCommandType::stopGroup:
                    for (size_t i = 0; i < kMaxVoices; i++)
                    {
                        auto& v = _voicePool.getByIndex(i);
                        if (v.state != VoiceState::idle && v.group == cmd.data.stopGroup.group)
                            markSlotInactive(i);
                    }
                    _voicePool.releaseGroup(cmd.data.stopGroup.group);
                    break;
                case AudioCommandType::setPan:
                    processSetPan(cmd);
                    break;
                case AudioCommandType::setRate:
                    processSetRate(cmd);
                    break;
                case AudioCommandType::setGroupVolume:
                    processSetGroupVolume(cmd);
                    break;
                case AudioCommandType::fadeOut:
                    processFadeOut(cmd);
                    break;
            }
        }
    }

    void AudioEngine::processPlayOneShot(const AudioCommand& cmd)
    {
        auto handle = _voicePool.claim();
        if (!handle.isValid())
        {
            handle = _voicePool.stealQuietest();
            if (handle.isValid())
                markSlotInactive(handle.slotIndex());
        }
        if (!handle.isValid())
            return;

        auto* voice = _voicePool.get(handle);
        if (voice == nullptr)
            return;

        const auto& p = cmd.data.playOneShot;
        voice->pcmData = p.pcmData;
        voice->pcmLengthInFrames = p.pcmLengthInFrames;
        voice->sampleRate = p.sampleRate;
        voice->channels = p.channels;
        voice->volume = p.volume;
        voice->targetVolume = p.volume;
        voice->pan = p.pan;
        voice->rate = 1.0f;
        voice->group = p.group;
        voice->looping = false;
        voice->playbackPosition = 0.0;
        voice->fadePerSample = 0.0f;
    }

    void AudioEngine::processPlayLoop(const AudioCommand& cmd)
    {
        auto slotHandle = _voicePool.claim();
        if (!slotHandle.isValid())
        {
            slotHandle = _voicePool.stealQuietest();
            if (slotHandle.isValid())
            {
                markSlotInactive(slotHandle.slotIndex());
            }
        }
        if (!slotHandle.isValid())
            return;

        auto* voice = _voicePool.get(slotHandle);
        if (voice == nullptr)
            return;

        const auto& p = cmd.data.playLoop;
        voice->pcmData = p.pcmData;
        voice->pcmLengthInFrames = p.pcmLengthInFrames;
        voice->sampleRate = p.sampleRate;
        voice->channels = p.channels;
        voice->volume = p.volume;
        voice->targetVolume = p.volume;
        voice->pan = p.pan;
        voice->rate = p.rate;
        voice->group = p.group;
        voice->looping = p.looping;
        voice->playbackPosition = 0.0;
        voice->fadePerSample = 0.0f;
        voice->gameHandle = p.handle;

        if (p.handle.isValid())
            markSlotActive(slotHandle.slotIndex(), p.handle);
    }

    Voice* AudioEngine::resolveVoice(AudioHandle handle)
    {
        if (!handle.isValid())
            return nullptr;
        return _voicePool.getByGameHandle(handle);
    }

    size_t AudioEngine::getActiveVoiceCount() const
    {
        return _voicePool.activeCount();
    }

    AudioEngineStats AudioEngine::getDebugStats() const
    {
        AudioEngineStats stats = _stats;
        stats.droppedCommands = _droppedCommands.load(std::memory_order_relaxed);
        return stats;
    }

    void AudioEngine::processStop(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.stop.handle);
        if (voice != nullptr)
        {
            if (voice->volume <= 0.001f)
            {
                voice->state = VoiceState::idle;
                if (voice->gameHandle.isValid())
                    markSlotInactive(_voicePool.indexOf(voice));
                return;
            }
            voice->state = VoiceState::stopping;
            voice->targetVolume = 0.0f;
            if (_outputSampleRate > 0)
            {
                float durationSamples = (5.0f / 1000.0f) * static_cast<float>(_outputSampleRate);
                voice->fadePerSample = voice->volume / std::max(1.0f, durationSamples);
            }
            else
            {
                voice->fadePerSample = voice->volume;
            }
        }
    }

    void AudioEngine::processSetVolume(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.setVolume.handle);
        if (voice != nullptr)
        {
            voice->targetVolume = std::clamp(cmd.data.setVolume.volume, 0.0f, 1.0f);
        }
    }

    void AudioEngine::processSetPan(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.setPan.handle);
        if (voice != nullptr)
        {
            voice->pan = std::clamp(cmd.data.setPan.pan, 0.0f, 1.0f);
        }
    }

    void AudioEngine::processSetRate(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.setRate.handle);
        if (voice != nullptr)
        {
            voice->rate = std::max(0.001f, cmd.data.setRate.rate);
        }
    }

    void AudioEngine::processSetMasterVolume(const AudioCommand& cmd)
    {
        _masterVolume = std::clamp(cmd.data.setMasterVolume.volume, 0.0f, 1.0f);
    }

    void AudioEngine::processSetGroupVolume(const AudioCommand& cmd)
    {
        float vol = std::clamp(cmd.data.setGroupVolume.volume, 0.0f, 1.0f);
        switch (cmd.data.setGroupVolume.group)
        {
            case AudioEngineGroup::sound:
                _soundVolume = vol;
                break;
            case AudioEngineGroup::rideMusic:
            case AudioEngineGroup::titleMusic:
                _musicVolume = vol;
                break;
        }
    }

    void AudioEngine::processFadeOut(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.fadeOut.handle);
        if (voice != nullptr && voice->state == VoiceState::playing)
        {
            if (voice->volume <= 0.001f)
            {
                voice->state = VoiceState::idle;
                return;
            }
            voice->state = VoiceState::stopping;
            voice->targetVolume = 0.0f;
            if (cmd.data.fadeOut.durationMs > 0 && _outputSampleRate > 0)
            {
                float durationSamples = (cmd.data.fadeOut.durationMs / 1000.0f) * static_cast<float>(_outputSampleRate);
                voice->fadePerSample = voice->volume / std::max(1.0f, durationSamples);
            }
            else
            {
                voice->fadePerSample = voice->volume;
            }
        }
    }

    float AudioEngine::getGroupVolume(AudioEngineGroup group) const
    {
        switch (group)
        {
            case AudioEngineGroup::sound:
                return _soundVolume;
            case AudioEngineGroup::rideMusic:
            case AudioEngineGroup::titleMusic:
                return _musicVolume;
        }
        return 1.0f;
    }

    void AudioEngine::updateGovernor(float callbackDurationUs, size_t framesRequested)
    {
        float deadlineUs = (static_cast<float>(framesRequested) / static_cast<float>(_outputSampleRate)) * 1e6f;
        float budgetPct = (callbackDurationUs / deadlineUs) * 100.0f;

        _budgetAvg = _budgetAvg * 0.9f + budgetPct * 0.1f;
        _stats.budgetPercent = _budgetAvg;

        if (budgetPct > 70.0f)
        {
            _comfortStreak = 0;

            _cullThreshold = std::min(_cullThreshold * 1.5f, kCullVolumeMax);

            size_t currentLimit = _voicePool.getVoiceLimit();
            if (currentLimit > kVoiceLimitMin)
            {
                _voicePool.setVoiceLimit(currentLimit - 8);
            }
            return;
        }

        if (_budgetAvg < 15.0f)
        {
            _comfortStreak++;
        }
        else
        {
            _comfortStreak = 0;
        }

        if (_comfortStreak >= 50)
        {
            _comfortStreak = 0;

            if (_cullThreshold > kCullVolumeMin)
                _cullThreshold = std::max(_cullThreshold * 0.8f, kCullVolumeMin);

            size_t currentLimit = _voicePool.getVoiceLimit();
            if (currentLimit < kVoiceLimitMax)
                _voicePool.setVoiceLimit(std::min(currentLimit + 4, kVoiceLimitMax));
        }
    }

    void AudioEngine::mixAllVoices(float* outputBuffer, size_t frames, uint32_t outputSampleRate, size_t& culled)
    {
        for (size_t i = 0; i < kMaxVoices; i++)
        {
            auto& voice = _voicePool.getByIndex(i);
            if (voice.state == VoiceState::idle)
                continue;

            float groupVol = getGroupVolume(voice.group);
            float effectiveVol = std::max(voice.volume, voice.targetVolume) * groupVol;
            if (effectiveVol < _cullThreshold && voice.state == VoiceState::playing)
            {
                double rateRatio = (static_cast<double>(voice.sampleRate) / static_cast<double>(outputSampleRate)) * voice.rate;
                voice.playbackPosition += rateRatio * static_cast<double>(frames);

                if (static_cast<uint64_t>(voice.playbackPosition) >= voice.pcmLengthInFrames)
                {
                    if (voice.looping)
                    {
                        voice.playbackPosition = std::fmod(
                            voice.playbackPosition, static_cast<double>(voice.pcmLengthInFrames));
                    }
                    else
                    {
                        voice.state = VoiceState::idle;
                        if (voice.gameHandle.isValid())
                            markSlotInactive(i);
                    }
                }
                culled++;
                continue;
            }

            mixVoice(voice, outputBuffer, frames, outputSampleRate);

            if (voice.state == VoiceState::idle)
            {
                if (voice.gameHandle.isValid())
                    markSlotInactive(i);
            }
        }
    }

    void AudioEngine::mixVoice(Voice& voice, float* outputBuffer, size_t frames, uint32_t outputSampleRate)
    {
        if (voice.pcmData == nullptr || voice.pcmLengthInFrames == 0)
        {
            voice.state = VoiceState::idle;
            return;
        }

        float groupVol = getGroupVolume(voice.group);

        double rateRatio = (static_cast<double>(voice.sampleRate) / static_cast<double>(outputSampleRate)) * voice.rate;

        float targetPanL = 1.0f;
        float targetPanR = 1.0f;
        if (voice.pan != 0.5f)
        {
            float deviation = std::abs(voice.pan - 0.5f) * 2.0f;
            float decibels = deviation * 100.0f;
            float attenuation = std::pow(10.0f, decibels / 20.0f);
            if (voice.pan <= 0.5f)
                targetPanR = 1.0f / attenuation;
            else
                targetPanL = 1.0f / attenuation;
        }

        float currentVolume = voice.volume;
        bool fading = (voice.state == VoiceState::stopping) && voice.fadePerSample > 0.0f;

        if (voice.state == VoiceState::stopping && !fading)
        {
            voice.state = VoiceState::idle;
            voice.volume = 0.0f;
            return;
        }

        bool volumeRamping = !fading && (currentVolume != voice.targetVolume);

        float panL = voice.prevPanL;
        float panR = voice.prevPanR;
        float panStepL = (targetPanL - panL) / static_cast<float>(frames);
        float panStepR = (targetPanR - panR) / static_cast<float>(frames);

        float volumeStep = 0.0f;
        if (volumeRamping)
            volumeStep = (voice.targetVolume - currentVolume) / static_cast<float>(frames);

        for (size_t frame = 0; frame < frames; frame++)
        {
            double srcPos = voice.playbackPosition;
            auto srcIndex = static_cast<uint64_t>(srcPos);

            if (srcIndex >= voice.pcmLengthInFrames)
            {
                if (voice.looping)
                {
                    voice.playbackPosition = std::fmod(voice.playbackPosition, static_cast<double>(voice.pcmLengthInFrames));
                    srcPos = voice.playbackPosition;
                    srcIndex = static_cast<uint64_t>(srcPos);
                }
                else
                {
                    voice.state = VoiceState::idle;
                    return;
                }
            }

            float frac = static_cast<float>(srcPos - static_cast<double>(srcIndex));
            uint64_t nextIndex = srcIndex + 1;
            if (nextIndex >= voice.pcmLengthInFrames)
                nextIndex = voice.looping ? 0 : srcIndex;

            float sampleL, sampleR;
            if (voice.channels == 1)
            {
                float s0 = voice.pcmData[srcIndex];
                float s1 = voice.pcmData[nextIndex];
                float sample = s0 + (s1 - s0) * frac;
                sampleL = sample;
                sampleR = sample;
            }
            else
            {
                float s0L = voice.pcmData[srcIndex * 2];
                float s0R = voice.pcmData[srcIndex * 2 + 1];
                float s1L = voice.pcmData[nextIndex * 2];
                float s1R = voice.pcmData[nextIndex * 2 + 1];
                sampleL = s0L + (s1L - s0L) * frac;
                sampleR = s0R + (s1R - s0R) * frac;
            }

            float vol = currentVolume * groupVol;
            sampleL *= vol * panL;
            sampleR *= vol * panR;

            outputBuffer[frame * 2] += sampleL;
            outputBuffer[frame * 2 + 1] += sampleR;

            voice.playbackPosition += rateRatio;

            panL += panStepL;
            panR += panStepR;

            if (fading)
            {
                currentVolume -= voice.fadePerSample;
                if (currentVolume <= 0.0f)
                {
                    currentVolume = 0.0f;
                    voice.state = VoiceState::idle;
                    voice.volume = 0.0f;
                    return;
                }
            }
            else
            {
                currentVolume += volumeStep;
            }
        }

        voice.volume = currentVolume;
        voice.prevPanL = targetPanL;
        voice.prevPanR = targetPanR;
    }

    bool AudioEngine::isHandleActive(AudioHandle gameHandle) const
    {
        if (!gameHandle.isValid())
            return false;

        for (size_t i = 0; i < kMaxVoices; i++)
        {
            if (_slotStatus[i].gameHandle.load(std::memory_order_relaxed) == gameHandle.value)
                return _slotStatus[i].active.load(std::memory_order_relaxed);
        }

        return false;
    }

    void AudioEngine::markSlotActive(size_t slotIndex, AudioHandle gameHandle)
    {
        _slotStatus[slotIndex].gameHandle.store(gameHandle.value, std::memory_order_relaxed);
        _slotStatus[slotIndex].active.store(true, std::memory_order_relaxed);
    }

    void AudioEngine::markSlotInactive(size_t slotIndex)
    {
        _slotStatus[slotIndex].active.store(false, std::memory_order_relaxed);
    }

} // namespace OpenRCT2::Audio
