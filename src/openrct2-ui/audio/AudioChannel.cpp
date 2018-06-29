/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/common.h>
#include <cmath>
#include <SDL2/SDL.h>
#include <speex/speex_resampler.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/audio/AudioChannel.h>
#include <openrct2/audio/AudioSource.h>
#include "AudioContext.h"
#include "AudioFormat.h"

namespace OpenRCT2::Audio
{
    class AudioChannelImpl : public ISDLAudioChannel
    {
    private:
        ISDLAudioSource * _source = nullptr;
        SpeexResamplerState * _resampler = nullptr;

        int32_t _group = MIXER_GROUP_SOUND;
        double _rate = 0;
        uint64_t _offset = 0;
        int32_t _loop = 0;

        int32_t  _volume = 1;
        float   _volume_l = 0.f;
        float   _volume_r = 0.f;
        float   _oldvolume_l = 0.f;
        float   _oldvolume_r = 0.f;
        int32_t  _oldvolume = 0;
        float   _pan = 0;

        bool    _stopping = false;
        bool    _done = true;
        bool    _deleteondone = false;
        bool    _deletesourceondone = false;

    public:
        AudioChannelImpl()
        {
            SetRate(1);
            SetVolume(MIXER_VOLUME_MAX);
            SetPan(0.5f);
        }

        ~AudioChannelImpl() override
        {
            if (_resampler != nullptr)
            {
                speex_resampler_destroy(_resampler);
                _resampler = nullptr;
            }
            if (_deletesourceondone)
            {
                delete _source;
            }
        }

        IAudioSource * GetSource() const override
        {
            return _source;
        }

        SpeexResamplerState * GetResampler() const override
        {
            return _resampler;
        }

        void SetResampler(SpeexResamplerState * value) override
        {
            _resampler = value;
        }

        int32_t GetGroup() const override
        {
            return _group;
        }

        void SetGroup(int32_t group) override
        {
            _group = group;
        }

        double GetRate() const override
        {
            return _rate;
        }

        void SetRate(double rate) override
        {
            _rate = std::max(0.001, rate);
        }

        uint64_t GetOffset() const override
        {
            return _offset;
        }

        bool SetOffset(uint64_t offset) override
        {
            if (_source != nullptr && offset < _source->GetLength())
            {
                AudioFormat format = _source->GetFormat();
                int32_t samplesize = format.channels * format.BytesPerSample();
                _offset = (offset / samplesize) * samplesize;
                return true;
            }
            return false;
        }

        virtual int32_t GetLoop() const override
        {
            return _loop;
        }

        virtual void SetLoop(int32_t value) override
        {
            _loop = value;
        }

        int32_t GetVolume() const override
        {
            return _volume;
        }

        float GetVolumeL() const override
        {
            return _volume_l;
        }

        float GetVolumeR() const override
        {
            return _volume_r;
        }

        float GetOldVolumeL() const override
        {
            return _oldvolume_l;
        }

        float GetOldVolumeR() const override
        {
            return _oldvolume_r;
        }

        int32_t GetOldVolume() const override
        {
            return _oldvolume;
        }

        void SetVolume(int32_t volume) override
        {
            _volume = Math::Clamp(0, volume, MIXER_VOLUME_MAX);
        }

        float GetPan() const override
        {
            return _pan;
        }

        void SetPan(float pan) override
        {
            _pan = Math::Clamp(0.0f, pan, 1.0f);
            double decibels = (std::abs(_pan - 0.5) * 2.0) * 100.0;
            double attenuation = pow(10, decibels / 20.0);
            if (_pan <= 0.5)
            {
                _volume_l = 1.0;
                _volume_r = (float)(1.0 / attenuation);
            }
            else
            {
                _volume_r = 1.0;
                _volume_l = (float)(1.0 / attenuation);
            }
        }

        bool IsStopping() const override
        {
            return _stopping;
        }

        void SetStopping(bool value) override
        {
            _stopping = value;
        }

        bool IsDone() const override
        {
            return _done;
        }

        void SetDone(bool value) override
        {
            _done = value;
        }

        bool DeleteOnDone() const override
        {
            return _deleteondone;
        }

        void SetDeleteOnDone(bool value) override
        {
            _deleteondone = value;
        }

        void SetDeleteSourceOnDone(bool value) override
        {
            _deletesourceondone = value;
        }

        bool IsPlaying() const override
        {
            return !_done;
        }

        void Play(IAudioSource * source, int32_t loop) override
        {
            _source = static_cast<ISDLAudioSource *>(source);
            _loop = loop;
            _offset = 0;
            _done = false;
        }

        void UpdateOldVolume() override
        {
            _oldvolume = _volume;
            _oldvolume_l = _volume_l;
            _oldvolume_r = _volume_r;
        }

        AudioFormat GetFormat() const override
        {
            AudioFormat result = {};
            // The second check is there because NullAudioSource does not implement GetFormat. Avoid calling it.
            if (_source != nullptr && _source->GetLength() > 0)
            {
                result = _source->GetFormat();
            }
            return result;
        }

        size_t Read(void * dst, size_t len) override
        {
            size_t bytesRead = 0;
            size_t bytesToRead = len;
            while (bytesToRead > 0 && !_done)
            {
                size_t readLen = _source->Read(dst, _offset, bytesToRead);
                if (readLen > 0)
                {
                    dst = (void *)((uintptr_t)dst + readLen);
                    bytesToRead -= readLen;
                    bytesRead += readLen;
                    _offset += readLen;
                }
                if (_offset >= _source->GetLength())
                {
                    if (_loop == 0)
                    {
                        _done = true;
                    }
                    else if (_loop == MIXER_LOOP_INFINITE)
                    {
                        _offset = 0;
                    }
                    else
                    {
                        _loop--;
                        _offset = 0;
                    }
                }
            }
            return bytesRead;
        }
    };

    ISDLAudioChannel * AudioChannel::Create()
    {
        return new (std::nothrow) AudioChannelImpl();
    }
} // namespace OpenRCT2::Audio
