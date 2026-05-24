/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewAudioContext.h"

#include "../SDLAudioSource.h"
#include "NewEngineAudioChannel.h"

#include <SDL.h>
#include <cmath>
#include <openrct2/Diagnostic.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/IStream.hpp>

namespace OpenRCT2::Audio
{
    Float32AudioSource::Float32AudioSource(Float32AudioData* data)
        : _data(data)
    {
    }

    void Float32AudioSource::Release()
    {
        _released = true;
    }

    bool Float32AudioSource::IsReleased() const
    {
        return _released;
    }

    int32_t Float32AudioSource::GetBytesPerSecond() const
    {
        if (_data == nullptr)
            return 0;
        return static_cast<int32_t>(_data->sampleRate * _data->channels * sizeof(float));
    }

    uint64_t Float32AudioSource::GetLength() const
    {
        if (_data == nullptr)
            return 0;
        return _data->samples.size() * sizeof(float);
    }

    size_t Float32AudioSource::Read(void* dst, uint64_t offset, size_t len)
    {
        if (_data == nullptr || _released)
            return 0;
        size_t totalBytes = _data->samples.size() * sizeof(float);
        if (offset >= totalBytes)
            return 0;
        size_t available = totalBytes - static_cast<size_t>(offset);
        size_t toRead = std::min(len, available);
        std::memcpy(dst, reinterpret_cast<const uint8_t*>(_data->samples.data()) + offset, toRead);
        return toRead;
    }

    Float32AudioData* Float32AudioSource::getData() const
    {
        return _data;
    }

    NewAudioContext::NewAudioContext()
    {
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            LOG_ERROR("SDL_Init(SDL_INIT_AUDIO) failed: %s", SDL_GetError());
            return;
        }

        _engine = std::make_unique<AudioEngine>();
        _platform = std::make_unique<AudioPlatformSDL2>(*_engine);
    }

    NewAudioContext::~NewAudioContext()
    {
        if (_platform)
            _platform->close();

        _sources.clear();
        _audioData.clear();

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    IAudioMixer* NewAudioContext::GetMixer()
    {
        return nullptr;
    }

    void NewAudioContext::CloseDevice()
    {
        if (_platform)
            _platform->close();
    }

    std::vector<std::string> NewAudioContext::GetOutputDevices()
    {
        return AudioPlatformSDL2::enumerateDevices();
    }

    void NewAudioContext::SetOutputDevice(const std::string& deviceName)
    {
        if (_platform == nullptr)
            return;

        const char* name = deviceName.empty() ? nullptr : deviceName.c_str();
        if (!_platform->open(name))
        {
            LOG_ERROR("Failed to open audio device '%s' for new engine", deviceName.c_str());
        }
    }

    static SDL_RWops* createRWopsFromStream(std::unique_ptr<IStream> stream)
    {
        auto* rw = SDL_AllocRW();
        if (rw == nullptr)
            return nullptr;

        *rw = {};
        rw->type = SDL_RWOPS_UNKNOWN;
        rw->hidden.unknown.data1 = stream.release();
        rw->seek = [](SDL_RWops* ctx, Sint64 offset, int whence) {
            auto* ptr = static_cast<IStream*>(ctx->hidden.unknown.data1);
            ptr->Seek(offset, whence);
            return static_cast<Sint64>(ptr->GetPosition());
        };
        rw->read = [](SDL_RWops* ctx, void* buf, size_t size, size_t maxnum) {
            auto* ptr = static_cast<IStream*>(ctx->hidden.unknown.data1);
            return static_cast<size_t>(ptr->TryRead(buf, size * maxnum) / size);
        };
        rw->size = [](SDL_RWops* ctx) {
            auto* ptr = static_cast<IStream*>(ctx->hidden.unknown.data1);
            return static_cast<Sint64>(ptr->GetLength());
        };
        rw->close = [](SDL_RWops* ctx) {
            auto* ptr = static_cast<IStream*>(ctx->hidden.unknown.data1);
            delete ptr;
            ctx->hidden.unknown.data1 = nullptr;
            SDL_free(ctx);
            return 0;
        };

        return rw;
    }

    IAudioSource* NewAudioContext::CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index)
    {
        removeReleasedSources();

        auto* rw = createRWopsFromStream(std::move(stream));
        if (rw == nullptr)
            return nullptr;

        try
        {
            auto sdlSource = CreateAudioSource(rw, index);
            SDL_RWclose(rw);

            if (sdlSource == nullptr)
                return nullptr;

            auto srcFormat = sdlSource->GetFormat();
            auto dataLen = sdlSource->GetLength();
            std::vector<uint8_t> rawPcm(static_cast<size_t>(dataLen));
            sdlSource->Read(rawPcm.data(), 0, rawPcm.size());

            auto* f32data = convertToFloat32(
                rawPcm.data(), rawPcm.size(), AudioPlatformSDL2::mapSDLFormat(srcFormat.format), srcFormat.channels,
                srcFormat.freq);
            if (f32data == nullptr)
                return nullptr;

            auto source = std::make_unique<Float32AudioSource>(f32data);
            auto* ptr = source.get();
            _sources.push_back(std::move(source));
            return ptr;
        }
        catch (const std::exception& e)
        {
            LOG_VERBOSE("Unable to create CSS audio source: %s", e.what());
            SDL_RWclose(rw);
            return nullptr;
        }
    }

    IAudioSource* NewAudioContext::CreateStreamFromWAV(std::unique_ptr<IStream> stream)
    {
        removeReleasedSources();

        auto* rw = createRWopsFromStream(std::move(stream));
        if (rw == nullptr)
            return nullptr;

        try
        {
            auto sdlSource = CreateAudioSource(rw);

            if (sdlSource == nullptr)
            {
                SDL_RWclose(rw);
                return nullptr;
            }

            auto srcFormat = sdlSource->GetFormat();
            auto dataLen = sdlSource->GetLength();

            static constexpr size_t kMaxLoadSize = 128 * 1024 * 1024;
            if (dataLen > kMaxLoadSize)
            {
                LOG_VERBOSE("Audio source too large for full load (%llu bytes), skipping", dataLen);
                return nullptr;
            }

            // TODO: Everything gets fully decoded into memory right now...
            // For short ride music tracks this is fine, but if someone loads a 20
            // minute custom track it'll eat RAM away. This might need streaming playback
            // for music specifically. It should probably happen eventually, but not today
            std::vector<uint8_t> rawPcm(static_cast<size_t>(dataLen));
            sdlSource->Read(rawPcm.data(), 0, rawPcm.size());

            auto* f32data = convertToFloat32(
                rawPcm.data(), rawPcm.size(), AudioPlatformSDL2::mapSDLFormat(srcFormat.format), srcFormat.channels,
                srcFormat.freq);
            if (f32data == nullptr)
                return nullptr;

            auto source = std::make_unique<Float32AudioSource>(f32data);
            auto* ptr = source.get();
            _sources.push_back(std::move(source));
            return ptr;
        }
        catch (const std::exception& e)
        {
            LOG_VERBOSE("Unable to create WAV audio source: %s", e.what());
            SDL_RWclose(rw);
            return nullptr;
        }
    }

    // TODO: Remove. Nothing calls this, so it can be removed from IAudioContext later... First legacy has to be yeeted.
    void NewAudioContext::StartTitleMusic()
    {
    }

    void NewAudioContext::ToggleAllSounds()
    {
        SyncVolumeSettings();
    }

    void NewAudioContext::PauseSounds()
    {
        if (_platform)
            _platform->pause();
    }

    void NewAudioContext::UnpauseSounds()
    {
        if (_platform)
            _platform->unpause();
    }

    void NewAudioContext::StopAll()
    {
        if (_engine)
            _engine->stopAll();
    }

    // Crowd and vehicle sounds are still managed by the old global code
    // in Peep.cpp and Audio.cpp, not through the new engine's voice pool.
    // These are no-ops until that changes at some point
    void NewAudioContext::StopCrowdSound()
    {
    }

    void NewAudioContext::StopVehicleSounds()
    {
    }

    void NewAudioContext::StopRideMusic()
    {
        if (_engine)
            _engine->stopGroup(AudioEngineGroup::rideMusic);
    }

    void NewAudioContext::StopTitleMusic()
    {
        if (_engine)
            _engine->stopGroup(AudioEngineGroup::titleMusic);
    }

    AudioEngine* NewAudioContext::getEngine()
    {
        return _engine.get();
    }

    bool NewAudioContext::IsNewEngine() const
    {
        return true;
    }

    void NewAudioContext::PlayOneShot(IAudioSource* source, float volume, float pan)
    {
        if (_engine == nullptr)
            return;

        auto* f32source = dynamic_cast<Float32AudioSource*>(source);
        if (f32source == nullptr || f32source->getData() == nullptr)
            return;

        auto* data = f32source->getData();
        _engine->playOneShot(
            data->samples.data(), data->lengthInFrames(), data->sampleRate, data->channels, volume, pan,
            AudioEngineGroup::sound);
    }

    std::shared_ptr<IAudioChannel> NewAudioContext::CreateChannel(
        IAudioSource* source, MixerGroup group, bool loop, int32_t volume, float pan, double rate)
    {
        if (_engine == nullptr)
            return nullptr;

        auto* f32source = dynamic_cast<Float32AudioSource*>(source);
        if (f32source == nullptr || f32source->getData() == nullptr)
            return nullptr;

        auto* data = f32source->getData();
        float normalizedVolume = static_cast<float>(volume) / static_cast<float>(kMixerVolumeMax);

        AudioEngineGroup engineGroup;
        switch (group)
        {
            case MixerGroup::RideMusic:
                engineGroup = AudioEngineGroup::rideMusic;
                break;
            case MixerGroup::TitleMusic:
                engineGroup = AudioEngineGroup::titleMusic;
                break;
            case MixerGroup::Sound:
            default:
                engineGroup = AudioEngineGroup::sound;
                break;
        }

        AudioHandle handle = _engine->playTracked(
            data->samples.data(), data->lengthInFrames(), data->sampleRate, data->channels, normalizedVolume, pan,
            static_cast<float>(rate), engineGroup, loop);

        auto channel = std::make_shared<NewEngineAudioChannel>(
            _engine.get(), handle, group, data->channels, data->lengthInFrames(), data->sourceBytesPerFrame());
        channel->SetVolume(volume);
        channel->SetPan(pan);
        channel->SetRate(rate);
        channel->UpdateOldVolume();
        return channel;
    }

    void NewAudioContext::SyncVolumeSettings()
    {
        if (_engine == nullptr)
            return;

        auto& sound = Config::Get().sound;

        float master = sound.masterSoundEnabled ? (static_cast<float>(sound.masterVolume) / 100.0f) : 0.0f;
        _engine->setMasterVolume(master);

        float sfx = powf(static_cast<float>(sound.soundVolume) / 100.0f, 10.0f / 6.0f);
        if (gLegacyScene == LegacyScene::titleSequence)
            sfx = std::min(sfx, 0.75f);
        _engine->setGroupVolume(AudioEngineGroup::sound, sfx);

        float music = powf(static_cast<float>(sound.rideMusicVolume) / 100.0f, 10.0f / 6.0f);
        _engine->setGroupVolume(AudioEngineGroup::rideMusic, music);
        _engine->setGroupVolume(AudioEngineGroup::titleMusic, music);
    }

    bool NewAudioContext::HandleAudioDeviceEvent(uint32_t eventType, uint32_t deviceIndex, bool isCapture)
    {
        if (_platform)
            return _platform->handleDeviceEvent(eventType, deviceIndex, isCapture);
        return false;
    }

    AudioStreamInfo NewAudioContext::ProbeStream(std::unique_ptr<IStream> stream)
    {
        auto* rw = createRWopsFromStream(std::move(stream));
        if (rw == nullptr)
            return {};

        try
        {
            // CreateAudioSource takes ownership of rw (closes it on destruction)
            auto sdlSource = CreateAudioSource(rw);
            if (sdlSource == nullptr)
                return {};

            AudioStreamInfo info;
            info.bytesPerSecond = sdlSource->GetBytesPerSecond();
            info.length = sdlSource->GetLength();
            return info;
        }
        catch (const std::exception& e)
        {
            LOG_VERBOSE("Unable to probe audio stream: %s", e.what());
            return {};
        }
    }

    void NewAudioContext::removeReleasedSources()
    {
        for (size_t i = 0; i < _sources.size();)
        {
            if (_sources[i] && _sources[i]->IsReleased())
            {
                _sources[i] = std::move(_sources.back());
                _sources.pop_back();
            }
            else
            {
                i++;
            }
        }

        for (size_t i = 0; i < _audioData.size();)
        {
            bool referenced = false;
            for (const auto& src : _sources)
            {
                if (src && src->getData() == _audioData[i].get())
                {
                    referenced = true;
                    break;
                }
            }
            if (!referenced)
            {
                _audioData[i] = std::move(_audioData.back());
                _audioData.pop_back();
            }
            else
            {
                i++;
            }
        }
    }

    Float32AudioData* NewAudioContext::convertToFloat32(
        const void* pcmData, size_t pcmLen, AudioSampleFormat format, uint8_t srcChannels, int srcFreq)
    {
        if (pcmLen == 0)
            return nullptr;

        auto data = std::make_unique<Float32AudioData>();
        data->sampleRate = static_cast<uint32_t>(srcFreq);
        data->channels = srcChannels;

        size_t srcBytesPerSample = 2;
        switch (format)
        {
            case AudioSampleFormat::u8:
                srcBytesPerSample = 1;
                break;
            case AudioSampleFormat::s16:
                srcBytesPerSample = 2;
                break;
            case AudioSampleFormat::s32:
            case AudioSampleFormat::f32:
                srcBytesPerSample = 4;
                break;
            default:
                LOG_WARNING("Unknown audio sample format, assuming S16");
                format = AudioSampleFormat::s16;
                srcBytesPerSample = 2;
                break;
        }

        data->sourceBytesPerSample = static_cast<uint8_t>(srcBytesPerSample);

        size_t totalSamples = pcmLen / srcBytesPerSample;
        data->samples.resize(totalSamples);

        switch (format)
        {
            case AudioSampleFormat::s16:
            {
                const auto* src = static_cast<const int16_t*>(pcmData);
                for (size_t i = 0; i < totalSamples; i++)
                    data->samples[i] = static_cast<float>(src[i]) / 32768.0f;
                break;
            }
            case AudioSampleFormat::u8:
            {
                const auto* src = static_cast<const uint8_t*>(pcmData);
                for (size_t i = 0; i < totalSamples; i++)
                    data->samples[i] = (static_cast<float>(src[i]) - 128.0f) / 128.0f;
                break;
            }
            case AudioSampleFormat::f32:
            {
                const auto* src = static_cast<const float*>(pcmData);
                std::copy_n(src, totalSamples, data->samples.data());
                break;
            }
            case AudioSampleFormat::s32:
            {
                const auto* src = static_cast<const int32_t*>(pcmData);
                for (size_t i = 0; i < totalSamples; i++)
                    data->samples[i] = static_cast<float>(static_cast<double>(src[i]) / 2147483648.0);
                break;
            }
            default:
                break;
        }

        auto* ptr = data.get();
        _audioData.push_back(std::move(data));
        return ptr;
    }

} // namespace OpenRCT2::Audio
