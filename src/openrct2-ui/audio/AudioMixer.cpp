/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/core/String.hpp>
#include <utility>

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
    want.format = SDL_AUDIO_S16;
    want.channels = 2;

    SDL_AudioDeviceID devId = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
    if (device != nullptr)
    {
        int numDevices = 0;
        auto* deviceIds = SDL_GetAudioPlaybackDevices(&numDevices);
        if (deviceIds != nullptr)
        {
            for (int i = 0; i < numDevices; i++)
            {
                if (String::equals(SDL_GetAudioDeviceName(deviceIds[i]), device))
                {
                    devId = deviceIds[i];
                    break;
                }
            }
            SDL_free(deviceIds);
        }
    }

    _deviceStream = SDL_OpenAudioDeviceStream(devId, &want, AudioStreamCallback, this);

    // PutAudioStreamData is always fed in `want` format; SDL resamples to the device's native format internally.
    _outputFormat.format = want.format;
    _outputFormat.channels = want.channels;
    _outputFormat.freq = want.freq;
    _deviceId = _deviceStream != nullptr ? SDL_GetAudioStreamDevice(_deviceStream) : 0;

    if (_deviceStream != nullptr)
    {
        SDL_ResumeAudioStreamDevice(_deviceStream);
    }
}

void AudioMixer::Close()
{
    // Destroy the stream first: SDL_DestroyAudioStream synchronously stops the device thread,
    // so once it returns the audio callback can no longer be running and it's safe to touch
    // mixer state below without locking against it.
    SDL_AudioStream* stream = std::exchange(_deviceStream, nullptr);
    if (stream != nullptr)
    {
        SDL_DestroyAudioStream(stream);
    }
    _deviceId = 0;

    // Free channels
    _channels.clear();

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
    if (_deviceStream != nullptr)
    {
        SDL_LockAudioStream(_deviceStream);
    }
}

void AudioMixer::Unlock()
{
    if (_deviceStream != nullptr)
    {
        SDL_UnlockAudioStream(_deviceStream);
    }
}

void AudioMixer::AudioStreamCallback(void* userdata, SDL_AudioStream* stream, int additionalAmount, int totalAmount)
{
    auto* mixer = static_cast<AudioMixer*>(userdata);
    std::vector<uint8_t> buffer(static_cast<size_t>(additionalAmount));
    mixer->GetNextAudioChunk(buffer.data(), buffer.size());
    mixer->RemoveReleasedSources();
    SDL_PutAudioStreamData(stream, buffer.data(), additionalAmount);
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
    return _outputFormat;
}

// TODO: investigate replacing this with OpenAL (#26035)
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
            if ((group != MixerGroup::sound || Config::Get().sound.soundEnabled) && Config::Get().sound.masterSoundEnabled
                && Config::Get().sound.masterVolume != 0)
            {
                MixChannel(channel.get(), dst, length);
            }
            it++;
        }
    }
}

// TODO: investigate replacing this with OpenAL (#26035)
void AudioMixer::UpdateAdjustedSound()
{
    // Did the volume level get changed? Recalculate level in this case.
    if (_settingSoundVolume != Config::Get().sound.soundVolume)
    {
        _settingSoundVolume = Config::Get().sound.soundVolume;
        _adjustSoundVolume = powf(static_cast<float>(_settingSoundVolume) / 100.f, 10.f / 6.f);
    }
    if (_settingMusicVolume != Config::Get().sound.rideMusicVolume)
    {
        _settingMusicVolume = Config::Get().sound.rideMusicVolume;
        _adjustMusicVolume = powf(static_cast<float>(_settingMusicVolume) / 100.f, 10.f / 6.f);
    }
}

// TODO: investigate replacing this with OpenAL (#26035)
void AudioMixer::MixChannel(ISDLAudioChannel* channel, uint8_t* data, size_t length)
{
    int32_t outputByteRate = _outputFormat.GetByteRate();
    auto numSamples = static_cast<int32_t>(length / outputByteRate);
    double rate = 1;
    if (_outputFormat.format == SDL_AUDIO_S16)
    {
        rate = channel->GetRate();
    }

    bool mustConvert = false;
    double lenRatio = 1;
    AudioFormat streamformat = channel->GetFormat();
    if (streamformat != _outputFormat)
    {
        lenRatio = (static_cast<double>(_outputFormat.freq) / streamformat.freq)
            * (static_cast<double>(_outputFormat.channels) / streamformat.channels)
            * (static_cast<double>(SDL_AUDIO_BYTESIZE(_outputFormat.format)) / SDL_AUDIO_BYTESIZE(streamformat.format));
        mustConvert = true;
    }

    // Read raw PCM from channel
    int32_t readSamples = numSamples * rate;
    auto readLength = static_cast<size_t>(ceil(readSamples / lenRatio)) * outputByteRate;
    _channelBuffer.resize(readLength);
    size_t bytesRead = channel->Read(_channelBuffer.data(), readLength);

    // Convert data to required format if necessary
    void* buffer = nullptr;
    size_t bufferLen = 0;
    if (mustConvert)
    {
        if (!Convert(streamformat, _channelBuffer.data(), bytesRead, &buffer, &bufferLen))
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
        auto inRate = static_cast<int32_t>(bufferLen / outputByteRate);
        int32_t outRate = numSamples;
        if (bytesRead != readLength)
        {
            inRate = _outputFormat.freq;
            outRate = _outputFormat.freq * (1 / rate);
        }
        _effectBuffer.resize(length);
        bufferLen = ApplyResample(buffer, static_cast<int32_t>(bufferLen / outputByteRate), numSamples, inRate, outRate);
        buffer = _effectBuffer.data();
    }

    // Apply panning and volume
    ApplyPan(channel, buffer, bufferLen, outputByteRate);
    int32_t mixVolume = ApplyVolume(channel, buffer, bufferLen);

    // Finally mix on to destination buffer
    size_t dstLength = std::min(length, bufferLen);
    SDL_MixAudio(
        data, static_cast<const uint8_t*>(buffer), _outputFormat.format, static_cast<uint32_t>(dstLength),
        static_cast<float>(mixVolume) / kMixerVolumeMax);

    channel->UpdateOldVolume();
}

/**
 * Resample the given buffer into _effectBuffer.
 * Assumes that srcBuffer is the same format as _outputFormat.
 *
 * TODO: investigate replacing this with OpenAL (#26035)
 */
size_t AudioMixer::ApplyResample(const void* srcBuffer, int32_t srcSamples, int32_t dstSamples, int32_t inRate, int32_t outRate)
{
    // Prevent buffer underread in inner loop
    if (srcSamples < 2)
        return 0;

    const int channels = _outputFormat.channels;
    const int bytesPerFrame = channels * sizeof(int16_t);

    const int16_t* src = static_cast<const int16_t*>(srcBuffer);
    int16_t* dst = reinterpret_cast<int16_t*>(_effectBuffer.data());

    double ratio = static_cast<double>(inRate) / static_cast<double>(outRate);

    for (int32_t i = 0; i < dstSamples; ++i)
    {
        double srcPos = i * ratio;
        int32_t index = static_cast<int32_t>(srcPos);
        double frac = srcPos - index;

        // Clamp to avoid reading past end
        if (index >= srcSamples - 1)
        {
            index = srcSamples - 2;
            frac = 1.0;
        }

        for (int ch = 0; ch < channels; ++ch)
        {
            int32_t baseIndex = index * channels + ch;

            int16_t s1 = src[baseIndex];
            int16_t s2 = src[baseIndex + channels];

            // Linear interpolation
            double sample = (1.0 - frac) * s1 + frac * s2;

            // Clamp to int16 range
            if (sample > 32767.0)
                sample = 32767.0;
            if (sample < -32768.0)
                sample = -32768.0;

            dst[i * channels + ch] = static_cast<int16_t>(sample);
        }
    }

    return dstSamples * bytesPerFrame;
}

// TODO: investigate replacing this with OpenAL (#26035)
void AudioMixer::ApplyPan(const IAudioChannel* channel, void* buffer, size_t len, size_t sampleSize)
{
    if (channel->GetPan() != 0.5f && _outputFormat.channels == 2)
    {
        switch (_outputFormat.format)
        {
            case SDL_AUDIO_S16:
                EffectPanS16(channel, static_cast<int16_t*>(buffer), static_cast<int32_t>(len / sampleSize));
                break;
            case SDL_AUDIO_U8:
                EffectPanU8(channel, static_cast<uint8_t*>(buffer), static_cast<int32_t>(len / sampleSize));
                break;
            default:
                break;
        }
    }
}

// TODO: investigate replacing this with OpenAL (#26035)
int32_t AudioMixer::ApplyVolume(const IAudioChannel* channel, void* buffer, size_t len)
{
    float volumeAdjust = _volume;
    volumeAdjust *= Config::Get().sound.masterSoundEnabled ? (static_cast<float>(Config::Get().sound.masterVolume) / 100.0f)
                                                           : 0.0f;

    switch (channel->GetGroup())
    {
        case MixerGroup::sound:
            volumeAdjust *= _adjustSoundVolume;

            // Cap sound volume on title screen so music is more audible
            if (gLegacyScene == LegacyScene::titleSequence)
            {
                volumeAdjust = std::min(volumeAdjust, 0.75f);
            }
            break;
        case MixerGroup::rideMusic:
        case MixerGroup::titleMusic:
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
        int32_t fadeLength = static_cast<int32_t>(len) / _outputFormat.BytesPerSample();
        switch (_outputFormat.format)
        {
            case SDL_AUDIO_S16:
                EffectFadeS16(static_cast<int16_t*>(buffer), fadeLength, startVolume, endVolume);
                break;
            case SDL_AUDIO_U8:
                EffectFadeU8(static_cast<uint8_t*>(buffer), fadeLength, startVolume, endVolume);
                break;
            default:
                break;
        }
    }
    return mixVolume;
}

// TODO: investigate replacing this with OpenAL (#26035)
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

// TODO: investigate replacing this with OpenAL (#26035)
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

// TODO: investigate replacing this with OpenAL (#26035)
void AudioMixer::EffectFadeS16(int16_t* data, int32_t length, int32_t startvolume, int32_t endvolume)
{
    float startvolume_f = static_cast<float>(startvolume) / kMixerVolumeMax;
    float endvolume_f = static_cast<float>(endvolume) / kMixerVolumeMax;
    for (int32_t i = 0; i < length; i++)
    {
        float t = static_cast<float>(i) / length;
        data[i] = static_cast<int16_t>(data[i] * ((1.0f - t) * startvolume_f + t * endvolume_f));
    }
}

// TODO: investigate replacing this with OpenAL (#26035)
void AudioMixer::EffectFadeU8(uint8_t* data, int32_t length, int32_t startvolume, int32_t endvolume)
{
    float startvolume_f = static_cast<float>(startvolume) / kMixerVolumeMax;
    float endvolume_f = static_cast<float>(endvolume) / kMixerVolumeMax;
    for (int32_t i = 0; i < length; i++)
    {
        float t = static_cast<float>(i) / length;
        data[i] = static_cast<uint8_t>(data[i] * ((1.0f - t) * startvolume_f + t * endvolume_f));
    }
}

bool AudioMixer::Convert(const AudioFormat& srcFormat, const void* src, size_t len, void** outBuf, size_t* outLen)
{
    // tofix: there seems to be an issue with converting audio using SDL_ConvertAudioSamples in the callback vs
    // preconverted, can cause pops and static depending on sample rate and channels
    if (len == 0)
    {
        return false;
    }

    SDL_AudioSpec srcSpec{ srcFormat.format, srcFormat.channels, srcFormat.freq };
    SDL_AudioSpec dstSpec{ _outputFormat.format, _outputFormat.channels, _outputFormat.freq };

    Uint8* dstData = nullptr;
    int dstLen = 0;
    if (!SDL_ConvertAudioSamples(&srcSpec, static_cast<const Uint8*>(src), static_cast<int>(len), &dstSpec, &dstData, &dstLen))
    {
        return false;
    }

    _convertBuffer.assign(dstData, dstData + dstLen);
    SDL_free(dstData);

    *outBuf = _convertBuffer.data();
    *outLen = _convertBuffer.size();
    return true;
}
