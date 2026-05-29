/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioEngine.h"

#include "AudioResampler.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <openrct2/Diagnostic.h>

namespace OpenRCT2::Audio
{
    static constexpr float kLimiterThreshold = 0.9f;
    static constexpr float kLimiterReleaseMs = 200.0f;
    static constexpr float kVolumeRampMs = 15.0f;
    static constexpr float kStopFadeMs = 5.0f;
    static constexpr float kPanFullAttenuationDb = 100.0f;

    static inline float safetyClip(float x)
    {
        constexpr float kCeil = 1.0f;
        if (x > kCeil)
            return kCeil;
        if (x < -kCeil)
            return -kCeil;
        return x;
    }

    AudioEngine::AudioEngine()
    {
        SincResampler::get();
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

    void AudioEngine::setOffset(AudioHandle handle, uint64_t offsetInFrames)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setOffset;
        cmd.data.setOffset.handle = handle;
        cmd.data.setOffset.offsetInFrames = offsetInFrames;
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

    void AudioEngine::setResamplerQuality(bool highQuality)
    {
        AudioCommand cmd{};
        cmd.type = AudioCommandType::setResamplerQuality;
        cmd.data.setResamplerQuality.highQuality = highQuality;
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
        size_t firstHalf = framesRequested / 2;
        size_t secondHalf = framesRequested - firstHalf;

        if (firstHalf > 0)
        {
            mixAllVoices(outputBuffer, firstHalf, outputSampleRate, culled);
            processCommands();
        }

        mixAllVoices(outputBuffer + firstHalf * 2, secondHalf, outputSampleRate, culled);

        // Per-sample limiter with instant attack, slow release
        {
            float gain = _limiterGain;
            float releaseCoeff = 1.0f - std::exp(-1000.0f / (kLimiterReleaseMs * static_cast<float>(outputSampleRate)));

            for (size_t i = 0; i < framesRequested; i++)
            {
                float L = outputBuffer[i * 2] * _masterVolume;
                float R = outputBuffer[i * 2 + 1] * _masterVolume;

                float peak = std::max(std::fabs(L), std::fabs(R));

                float targetGain = 1.0f;
                if (peak * gain > kLimiterThreshold)
                    targetGain = kLimiterThreshold / std::max(peak, 0.0001f);

                if (targetGain < gain)
                    gain = targetGain;
                else
                    gain = gain + releaseCoeff * (targetGain - gain);

                gain = std::min(gain, 1.0f);

                outputBuffer[i * 2] = safetyClip(L * gain);
                outputBuffer[i * 2 + 1] = safetyClip(R * gain);
            }

            _limiterGain = gain;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        float durationUs = std::chrono::duration<float, std::micro>(t1 - t0).count();

        updateGovernor(durationUs, framesRequested);

        _statActiveVoices.store(static_cast<uint32_t>(_voicePool.activeCount()), std::memory_order_relaxed);
        _statCulledVoices.store(static_cast<uint32_t>(culled), std::memory_order_relaxed);
        _statVoiceLimit.store(static_cast<uint32_t>(_voicePool.getVoiceLimit()), std::memory_order_relaxed);
        _statCallbackUs.store(durationUs, std::memory_order_relaxed);
    }

    void AudioEngine::processCommands()
    {
        if (false)
        {
            // Frozy was here
        }

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
                    _voicePool.clearGameHandleMap();
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
                case AudioCommandType::setOffset:
                    processSetOffset(cmd);
                    break;
                case AudioCommandType::setResamplerQuality:
                    processSetResamplerQuality(cmd);
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
        {
            _voicePool.registerGameHandle(p.handle.value, slotHandle.slotIndex());
            markSlotActive(slotHandle.slotIndex(), p.handle);
        }
    }

    Voice* AudioEngine::resolveVoice(AudioHandle handle)
    {
        if (!handle.isValid())
            return nullptr;
        return _voicePool.getByGameHandle(handle);
    }

    AudioEngineStats AudioEngine::getStats() const
    {
        AudioEngineStats stats{};
        stats.activeVoices = _statActiveVoices.load(std::memory_order_relaxed);
        stats.culledVoices = _statCulledVoices.load(std::memory_order_relaxed);
        stats.voiceLimit = _statVoiceLimit.load(std::memory_order_relaxed);
        stats.lastCallbackDurationUs = _statCallbackUs.load(std::memory_order_relaxed);
        stats.budgetPercent = _statBudgetPercent.load(std::memory_order_relaxed);
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
                float durationSamples = (kStopFadeMs / 1000.0f) * static_cast<float>(_outputSampleRate);
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

    void AudioEngine::processSetResamplerQuality(const AudioCommand& cmd)
    {
        _useSincResampler = cmd.data.setResamplerQuality.highQuality;
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

    void AudioEngine::processSetOffset(const AudioCommand& cmd)
    {
        auto* voice = resolveVoice(cmd.data.setOffset.handle);
        if (voice != nullptr && voice->state != VoiceState::idle)
        {
            uint64_t newPos = cmd.data.setOffset.offsetInFrames;
            if (newPos < voice->pcmLengthInFrames)
            {
                voice->playbackPosition = static_cast<double>(newPos);
                updateSlotPosition(_voicePool.indexOf(voice), newPos);
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
        _statBudgetPercent.store(_budgetAvg, std::memory_order_relaxed);

        if (budgetPct > 70.0f)
        {
            _comfortStreak = 0;
            _stressStreak++;

            if (_stressStreak >= 3)
            {
                _cullThreshold = std::min(_cullThreshold * 1.5f, kCullVolumeMax);

                size_t currentLimit = _voicePool.getVoiceLimit();
                if (currentLimit > kVoiceLimitMin)
                {
                    _voicePool.setVoiceLimit(currentLimit - 8);
                }
            }
            return;
        }

        _stressStreak = 0;

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

    void AudioEngine::advanceVoiceSilently(Voice& voice, size_t frames, uint32_t outputSampleRate, size_t voiceIndex)
    {
        if (voice.pcmLengthInFrames == 0)
        {
            voice.state = VoiceState::idle;
            if (voice.gameHandle.isValid())
                markSlotInactive(voiceIndex);
            return;
        }

        double rateRatio = (static_cast<double>(voice.sampleRate) / static_cast<double>(outputSampleRate)) * voice.rate;
        voice.playbackPosition += rateRatio * static_cast<double>(frames);

        if (static_cast<uint64_t>(voice.playbackPosition) >= voice.pcmLengthInFrames)
        {
            if (voice.looping)
            {
                voice.playbackPosition = std::fmod(voice.playbackPosition, static_cast<double>(voice.pcmLengthInFrames));
            }
            else
            {
                voice.state = VoiceState::idle;
                if (voice.gameHandle.isValid())
                    markSlotInactive(voiceIndex);
            }
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
                advanceVoiceSilently(voice, frames, outputSampleRate, i);
                culled++;
                continue;
            }

            mixVoice(voice, outputBuffer, frames, outputSampleRate);

            if (voice.gameHandle.isValid())
            {
                if (voice.state == VoiceState::idle)
                    markSlotInactive(i);
                else
                    updateSlotPosition(i, static_cast<uint64_t>(voice.playbackPosition));
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
            float decibels = deviation * kPanFullAttenuationDb;
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
        bool panChanged = (voice.prevPanL != targetPanL) || (voice.prevPanR != targetPanR);

        // Steady-state fast path (no ramping or fading)
        if (!fading && !volumeRamping && !panChanged)
        {
            float gainL = currentVolume * groupVol * targetPanL;
            float gainR = currentVolume * groupVol * targetPanR;

            if (_useSincResampler)
            {
                const auto& resampler = SincResampler::get();
                if (voice.channels == 1)
                    resampler.mixMono(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, rateRatio, voice.playbackPosition, outputBuffer,
                        frames, gainL, gainR);
                else
                    resampler.mixStereo(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, rateRatio, voice.playbackPosition, outputBuffer,
                        frames, gainL, gainR);
            }
            else
            {
                if (voice.channels == 1)
                    LinearResampler::mixMono(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, rateRatio, voice.playbackPosition, outputBuffer,
                        frames, gainL, gainR);
                else
                    LinearResampler::mixStereo(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, rateRatio, voice.playbackPosition, outputBuffer,
                        frames, gainL, gainR);
            }

            if (static_cast<uint64_t>(voice.playbackPosition) >= voice.pcmLengthInFrames && !voice.looping)
                voice.state = VoiceState::idle;

            voice.prevPanL = targetPanL;
            voice.prevPanR = targetPanR;
            return;
        }

        float panL = voice.prevPanL;
        float panR = voice.prevPanR;
        float panStepL = (targetPanL - panL) / static_cast<float>(frames);
        float panStepR = (targetPanR - panR) / static_cast<float>(frames);

        float volumeStep = 0.0f;
        if (volumeRamping)
        {
            float rampSamples = (kVolumeRampMs / 1000.0f) * static_cast<float>(outputSampleRate);
            volumeStep = (voice.targetVolume - currentVolume) / std::max(1.0f, rampSamples);
        }

        const auto& resampler = SincResampler::get();
        const auto resampleStep = SincResampler::makeStep(rateRatio);

        for (size_t frame = 0; frame < frames; frame++)
        {
            auto srcIndex = static_cast<uint64_t>(voice.playbackPosition);

            if (srcIndex >= voice.pcmLengthInFrames)
            {
                if (voice.looping)
                {
                    voice.playbackPosition = std::fmod(voice.playbackPosition, static_cast<double>(voice.pcmLengthInFrames));
                }
                else
                {
                    voice.state = VoiceState::idle;
                    return;
                }
            }

            float sampleL, sampleR;
            if (_useSincResampler)
            {
                if (voice.channels == 1)
                {
                    float sample = resampler.sampleMono(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, voice.playbackPosition, resampleStep.dhb,
                        resampleStep.filterScale);
                    sampleL = sample;
                    sampleR = sample;
                }
                else
                {
                    resampler.sampleStereo(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, voice.playbackPosition, resampleStep.dhb,
                        resampleStep.filterScale, sampleL, sampleR);
                }
            }
            else
            {
                if (voice.channels == 1)
                {
                    float sample = LinearResampler::sampleMono(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, voice.playbackPosition);
                    sampleL = sample;
                    sampleR = sample;
                }
                else
                {
                    LinearResampler::sampleStereo(
                        voice.pcmData, voice.pcmLengthInFrames, voice.looping, voice.playbackPosition, sampleL, sampleR);
                }
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
            else if (volumeStep != 0.0f)
            {
                currentVolume += volumeStep;
                if ((volumeStep > 0.0f && currentVolume >= voice.targetVolume)
                    || (volumeStep < 0.0f && currentVolume <= voice.targetVolume))
                {
                    currentVolume = voice.targetVolume;
                    volumeStep = 0.0f;
                }
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

        // A handle lives in at most one slot (cleared on completion), so the scan is unambiguous
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
        uint32_t handle = _slotStatus[slotIndex].gameHandle.load(std::memory_order_relaxed);
        _slotStatus[slotIndex].active.store(false, std::memory_order_relaxed);
        _slotStatus[slotIndex].gameHandle.store(AudioHandle::kInvalid, std::memory_order_relaxed);

        if (handle != AudioHandle::kInvalid)
            _voicePool.unregisterGameHandle(handle);
    }

    void AudioEngine::updateSlotPosition(size_t slotIndex, uint64_t positionInFrames)
    {
        _slotStatus[slotIndex].positionInFrames.store(positionInFrames, std::memory_order_relaxed);
    }

    uint64_t AudioEngine::getOffset(AudioHandle gameHandle) const
    {
        if (!gameHandle.isValid())
            return 0;

        for (size_t i = 0; i < kMaxVoices; i++)
        {
            if (_slotStatus[i].gameHandle.load(std::memory_order_relaxed) == gameHandle.value)
                return _slotStatus[i].positionInFrames.load(std::memory_order_relaxed);
        }
        return 0;
    }

} // namespace OpenRCT2::Audio
