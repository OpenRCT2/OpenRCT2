/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioMixer.h"

#include <algorithm>
#include <iterator>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <speex/speex_resampler.h>

using namespace OpenRCT2::Audio;

AudioMixer::~AudioMixer()
{
    Close();
}

void AudioMixer::Init(const char* device)
{
    Close();

    SDL_AudioSpec want = {};
    want.freq = 22050;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 2048;
    want.callback = [](void* arg, uint8_t* dst, int32_t length) -> void {
        auto* mixer = static_cast<AudioMixer*>(arg);
        mixer->GetNextAudioChunk(dst, static_cast<size_t>(length));
        mixer->RemoveReleasedSources();
    };
    want.userdata = this;

    SDL_AudioSpec have;
    _deviceId = SDL_OpenAudioDevice(device, 0, &want, &have, 0);
    _format.format = have.format;
    _format.channels = have.channels;
    _format.freq = have.freq;

    SDL_PauseAudioDevice(_deviceId, 0);
}

void AudioMixer::Close()
{
    // Free channels
    Lock();
    _channels.clear();
    Unlock();

    SDL_CloseAudioDevice(_deviceId);

    // Free buffers
    _channelBuffer.clear();
    _channelBuffer.shrink_to_fit();
    _convertBuffer.clear();
    _convertBuffer.shrink_to_fit();
    _effectBuffer.clear();
    _effectBuffer.shrink_to_fit();
}

void AudioMixer::Lock()
{
    SDL_LockAudioDevice(_deviceId);
}

void AudioMixer::Unlock()
{
    SDL_UnlockAudioDevice(_deviceId);
}

std::shared_ptr<IAudioChannel> AudioMixer::Play(IAudioSource* source, int32_t loop, bool deleteondone)
{
    Lock();
    auto channel = std::shared_ptr<ISDLAudioChannel>(AudioChannel::Create());
    if (channel != nullptr)
    {
        channel->Play(source, loop);
        channel->SetDeleteOnDone(deleteondone);
        _channels.push_back(channel);
    }
    Unlock();
    return channel;
}

void AudioMixer::SetVolume(float volume)
{
    _volume = volume;
}

SDLAudioSource* AudioMixer::AddSource(std::unique_ptr<SDLAudioSource> source)
{
    std::lock_guard<std::mutex> guard(_mutex);
    if (source != nullptr)
    {
        _sources.push_back(std::move(source));
        return _sources.back().get();
    }
    return nullptr;
}

void AudioMixer::RemoveReleasedSources()
{
    std::lock_guard<std::mutex> guard(_mutex);
    _sources.erase(
        std::remove_if(
            _sources.begin(), _sources.end(),
            [](std::unique_ptr<SDLAudioSource>& source) {
                {
                    return source->IsReleased();
                }
            }),
        _sources.end());
}

const AudioFormat& AudioMixer::GetFormat() const
{
    return _format;
}

void AudioMixer::GetNextAudioChunk(uint8_t* dst, size_t length)
{
    UpdateAdjustedSound();

    // Zero the output buffer
    std::fill_n(dst, length, 0);

    // Mix channels onto output buffer
    auto it = _channels.begin();
    while (it != _channels.end())
    {
        auto& channel = *it;
        auto channelSource = channel->GetSource();
        auto channelSourceReleased = channelSource == nullptr || channelSource->IsReleased();
        if (channelSourceReleased || (channel->IsDone() && channel->DeleteOnDone()) || channel->IsStopping())
        {
            channel->SetDone(true);
            it = _channels.erase(it);
        }
        else
        {
            auto group = channel->GetGroup();
            if ((group != MixerGroup::Sound || Config::Get().sound.SoundEnabled) && Config::Get().sound.MasterSoundEnabled
                && Config::Get().sound.MasterVolume != 0)
            {
                MixChannel(channel.get(), dst, length);
            }
            it++;
        }
    }
}

void AudioMixer::UpdateAdjustedSound()
{
    // Did the volume level get changed? Recalculate level in this case.
    if (_settingSoundVolume != Config::Get().sound.SoundVolume)
    {
        _settingSoundVolume = Config::Get().sound.SoundVolume;
        _adjustSoundVolume = powf(static_cast<float>(_settingSoundVolume) / 100.f, 10.f / 6.f);
    }
    if (_settingMusicVolume != Config::Get().sound.AudioFocus)
    {
        _settingMusicVolume = Config::Get().sound.AudioFocus;
        _adjustMusicVolume = powf(static_cast<float>(_settingMusicVolume) / 100.f, 10.f / 6.f);
    }
}

void AudioMixer::MixChannel(ISDLAudioChannel* channel, uint8_t* data, size_t length)
{
    int32_t byteRate = _format.GetByteRate();
    auto numSamples = static_cast<int32_t>(length / byteRate);
    double rate = 1;
    if (_format.format == AUDIO_S16SYS)
    {
        rate = channel->GetRate();
    }

    bool mustConvert = false;
    SDL_AudioCVT cvt;
    cvt.len_ratio = 1;
    AudioFormat streamformat = channel->GetFormat();
    if (streamformat != _format)
    {
        if (SDL_BuildAudioCVT(
                &cvt, streamformat.format, streamformat.channels, streamformat.freq, _format.format, _format.channels,
                _format.freq)
            == -1)
        {
            // Unable to convert channel data
            return;
        }
        mustConvert = true;
    }

    // Read raw PCM from channel
    int32_t readSamples = numSamples * rate;
    auto readLength = static_cast<size_t>(readSamples / cvt.len_ratio) * byteRate;
    _channelBuffer.resize(readLength);
    size_t bytesRead = channel->Read(_channelBuffer.data(), readLength);

    // Convert data to required format if necessary
    void* buffer = nullptr;
    size_t bufferLen = 0;
    if (mustConvert)
    {
        if (Convert(&cvt, _channelBuffer.data(), bytesRead))
        {
            buffer = cvt.buf;
            bufferLen = cvt.len_cvt;
        }
        else
        {
            return;
        }
    }
    else
    {
        buffer = _channelBuffer.data();
        bufferLen = bytesRead;
    }

    // Apply effects
    if (rate != 1)
    {
        auto inRate = static_cast<int32_t>(bufferLen / byteRate);
        int32_t outRate = numSamples;
        if (bytesRead != readLength)
        {
            inRate = _format.freq;
            outRate = _format.freq * (1 / rate);
        }
        _effectBuffer.resize(length);
        bufferLen = ApplyResample(channel, buffer, static_cast<int32_t>(bufferLen / byteRate), numSamples, inRate, outRate);
        buffer = _effectBuffer.data();
    }

    // Apply panning and volume
    ApplyPan(channel, buffer, bufferLen, byteRate);
    int32_t mixVolume = ApplyVolume(channel, buffer, bufferLen);

    // Finally mix on to destination buffer
    size_t dstLength = std::min(length, bufferLen);
    SDL_MixAudioFormat(data, static_cast<const uint8_t*>(buffer), _format.format, static_cast<uint32_t>(dstLength), mixVolume);

    channel->UpdateOldVolume();
}

/**
 * Resample the given buffer into _effectBuffer.
 * Assumes that srcBuffer is the same format as _format.
 */
size_t AudioMixer::ApplyResample(
    ISDLAudioChannel* channel, const void* srcBuffer, int32_t srcSamples, int32_t dstSamples, int32_t inRate, int32_t outRate)
{
    int32_t byteRate = _format.GetByteRate();

    // Create resampler
    SpeexResamplerState* resampler = channel->GetResampler();
    if (resampler == nullptr)
    {
        resampler = speex_resampler_init(_format.channels, _format.freq, _format.freq, 0, nullptr);
        channel->SetResampler(resampler);
    }
    speex_resampler_set_rate(resampler, inRate, outRate);

    uint32_t inLen = srcSamples;
    uint32_t outLen = dstSamples;
    speex_resampler_process_interleaved_int(
        resampler, static_cast<const spx_int16_t*>(srcBuffer), &inLen, reinterpret_cast<spx_int16_t*>(_effectBuffer.data()),
        &outLen);

    return outLen * byteRate;
}

void AudioMixer::ApplyPan(const IAudioChannel* channel, void* buffer, size_t len, size_t sampleSize)
{
    if (channel->GetPan() != 0.5f && _format.channels == 2)
    {
        switch (_format.format)
        {
            case AUDIO_S16SYS:
                EffectPanS16(channel, static_cast<int16_t*>(buffer), static_cast<int32_t>(len / sampleSize));
                break;
            case AUDIO_U8:
                EffectPanU8(channel, static_cast<uint8_t*>(buffer), static_cast<int32_t>(len / sampleSize));
                break;
        }
    }
}

int32_t AudioMixer::ApplyVolume(const IAudioChannel* channel, void* buffer, size_t len)
{
    float volumeAdjust = _volume;
    volumeAdjust *= Config::Get().sound.MasterSoundEnabled ? (static_cast<float>(Config::Get().sound.MasterVolume) / 100.0f)
                                                           : 0.0f;

    switch (channel->GetGroup())
    {
        case MixerGroup::Sound:
            volumeAdjust *= _adjustSoundVolume;

            // Cap sound volume on title screen so music is more audible
            if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
            {
                volumeAdjust = std::min(volumeAdjust, 0.75f);
            }
            break;
        case MixerGroup::RideMusic:
        case MixerGroup::TitleMusic:
            volumeAdjust *= _adjustMusicVolume;
            break;
    }

    int32_t startVolume = channel->GetOldVolume() * volumeAdjust;
    int32_t endVolume = channel->GetVolume() * volumeAdjust;
    if (channel->IsStopping())
    {
        endVolume = 0;
    }

    int32_t mixVolume = channel->GetVolume() * volumeAdjust;
    if (startVolume != endVolume)
    {
        // Set to max since we are adjusting the volume ourselves
        mixVolume = kMixerVolumeMax;

        // Fade between volume levels to smooth out sound and minimize clicks from sudden volume changes
        int32_t fadeLength = static_cast<int32_t>(len) / _format.BytesPerSample();
        switch (_format.format)
        {
            case AUDIO_S16SYS:
                EffectFadeS16(static_cast<int16_t*>(buffer), fadeLength, startVolume, endVolume);
                break;
            case AUDIO_U8:
                EffectFadeU8(static_cast<uint8_t*>(buffer), fadeLength, startVolume, endVolume);
                break;
        }
    }
    return mixVolume;
}

void AudioMixer::EffectPanS16(const IAudioChannel* channel, int16_t* data, int32_t length)
{
    const float dt = 1.0f / static_cast<float>(length * 2.0f);
    float volumeL = channel->GetOldVolumeL();
    float volumeR = channel->GetOldVolumeR();
    const float d_left = dt * (channel->GetVolumeL() - channel->GetOldVolumeL());
    const float d_right = dt * (channel->GetVolumeR() - channel->GetOldVolumeR());

    for (int32_t i = 0; i < length * 2; i += 2)
    {
        data[i + 0] = static_cast<int16_t>(volumeL * static_cast<float>(data[i + 0]));
        data[i + 1] = static_cast<int16_t>(volumeR * static_cast<float>(data[i + 1]));
        volumeL += d_left;
        volumeR += d_right;
    }
}

void AudioMixer::EffectPanU8(const IAudioChannel* channel, uint8_t* data, int32_t length)
{
    float volumeL = channel->GetVolumeL();
    float volumeR = channel->GetVolumeR();
    float oldVolumeL = channel->GetOldVolumeL();
    float oldVolumeR = channel->GetOldVolumeR();

    for (int32_t i = 0; i < length * 2; i += 2)
    {
        float t = static_cast<float>(i) / static_cast<float>(length * 2.0f);
        data[i] = static_cast<uint8_t>(data[i] * ((1.0 - t) * oldVolumeL + t * volumeL));
        data[i + 1] = static_cast<uint8_t>(data[i + 1] * ((1.0 - t) * oldVolumeR + t * volumeR));
    }
}

void AudioMixer::EffectFadeS16(int16_t* data, int32_t length, int32_t startvolume, int32_t endvolume)
{
    static_assert(SDL_MIX_MAXVOLUME == kMixerVolumeMax, "Max volume differs between OpenRCT2 and SDL2");

    float startvolume_f = static_cast<float>(startvolume) / SDL_MIX_MAXVOLUME;
    float endvolume_f = static_cast<float>(endvolume) / SDL_MIX_MAXVOLUME;
    for (int32_t i = 0; i < length; i++)
    {
        float t = static_cast<float>(i) / length;
        data[i] = static_cast<int16_t>(data[i] * ((1.0f - t) * startvolume_f + t * endvolume_f));
    }
}

void AudioMixer::EffectFadeU8(uint8_t* data, int32_t length, int32_t startvolume, int32_t endvolume)
{
    static_assert(SDL_MIX_MAXVOLUME == kMixerVolumeMax, "Max volume differs between OpenRCT2 and SDL2");

    float startvolume_f = static_cast<float>(startvolume) / SDL_MIX_MAXVOLUME;
    float endvolume_f = static_cast<float>(endvolume) / SDL_MIX_MAXVOLUME;
    for (int32_t i = 0; i < length; i++)
    {
        float t = static_cast<float>(i) / length;
        data[i] = static_cast<uint8_t>(data[i] * ((1.0f - t) * startvolume_f + t * endvolume_f));
    }
}

bool AudioMixer::Convert(SDL_AudioCVT* cvt, const void* src, size_t len)
{
    // tofix: there seems to be an issue with converting audio using SDL_ConvertAudio in the callback vs preconverted,
    // can cause pops and static depending on sample rate and channels
    bool result = false;
    if (len != 0 && cvt->len_mult != 0)
    {
        size_t reqConvertBufferCapacity = len * cvt->len_mult;
        _convertBuffer.resize(reqConvertBufferCapacity);
        std::copy_n(static_cast<const uint8_t*>(src), len, _convertBuffer.data());

        cvt->len = static_cast<int32_t>(len);
        cvt->buf = static_cast<uint8_t*>(_convertBuffer.data());
        if (SDL_ConvertAudio(cvt) >= 0)
        {
            result = true;
        }
    }
    return result;
}
