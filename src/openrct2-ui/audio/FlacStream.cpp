/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"
#include "AudioFormat.h"

#include <FLAC/all.h>
#include <SDL.h>
#include <algorithm>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/common.h>

namespace OpenRCT2::Audio
{
    /**
     * An audio source which decodes a FLAC stream.
     */
    class FlacAudioSource final : public ISDLAudioSource
    {
    private:
        AudioFormat _format = {};
        SDL_RWops* _rw = nullptr;
        bool _released{};

        FLAC__StreamDecoder* _decoder{};
        uint32_t _bitsPerSample{};
        uint32_t _totalSamples{};
        uint64_t _dataLength{};
        std::vector<uint8_t> _decodeBuffer;
        size_t _decodeBufferReadOffset{};
        size_t _currentOffset{};

    public:
        ~FlacAudioSource() override
        {
            Release();
        }

        bool IsReleased() const override
        {
            return _released;
        }

        void Release() override
        {
            if (!_released)
            {
                Unload();
                _released = true;
            }
        }

        [[nodiscard]] uint64_t GetLength() const override
        {
            return _dataLength;
        }

        [[nodiscard]] AudioFormat GetFormat() const override
        {
            return _format;
        }

        bool LoadFlac(SDL_RWops* rw)
        {
            _rw = rw;
            _decoder = FLAC__stream_decoder_new();
            if (_decoder == nullptr)
            {
                log_verbose("Could not create FLAC stream decoder");
                return false;
            }

            auto status = FLAC__stream_decoder_init_stream(
                _decoder, FlacCallbackRead, FlacCallbackSeek, FlacCallbackTell, FlacCallbackLength, FlacCallbackEof,
                FlacCallbackWrite, FlacCallbackMetadata, FlacCallbackError, this);
            if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
            {
                log_verbose("Could not initialise FLAC stream");
                return false;
            }

            if (!FLAC__stream_decoder_process_until_end_of_metadata(_decoder))
            {
                log_verbose("Could not read FLAC metadata");
                return false;
            }

            return true;
        }

        size_t Read(void* dst, uint64_t offset, size_t len) override
        {
            if (_decoder == nullptr)
                return 0;

            if (_currentOffset != offset)
            {
                // We have been asked for a new position in the stream
                auto byteRate = _format.GetByteRate();
                Seek(offset / byteRate);
                _currentOffset = offset;
            }

            auto dst8 = reinterpret_cast<uint8_t*>(dst);
            auto bytesRead = ReadFromDecodeBuffer(dst8, len);
            dst8 += bytesRead;
            if (bytesRead < len)
            {
                FillDecodeBuffer(len - bytesRead);
                bytesRead += ReadFromDecodeBuffer(dst8, len - bytesRead);
                dst8 += bytesRead;
            }

            _currentOffset += bytesRead;
            return bytesRead;
        }

    private:
        void Seek(uint64_t sampleIndex)
        {
            ResetDecodeBuffer();
            if (!FLAC__stream_decoder_seek_absolute(_decoder, sampleIndex))
            {
                if (FLAC__stream_decoder_get_state(_decoder) == FLAC__STREAM_DECODER_SEEK_ERROR)
                {
                    FLAC__stream_decoder_flush(_decoder);
                }
            }
        }

        void ResetDecodeBuffer()
        {
            _decodeBufferReadOffset = 0;
            _decodeBuffer.clear();
        }

        size_t ReadFromDecodeBuffer(void* dst, size_t len)
        {
            auto decodeReadLen = std::min(_decodeBuffer.size() - _decodeBufferReadOffset, len);
            std::memcpy(dst, _decodeBuffer.data() + _decodeBufferReadOffset, decodeReadLen);
            _decodeBufferReadOffset += decodeReadLen;
            if (_decodeBufferReadOffset == _decodeBuffer.size())
            {
                ResetDecodeBuffer();
            }
            return decodeReadLen;
        }

        void FillDecodeBuffer(size_t minimumLength)
        {
            // Decode more data until we have enough to pass back or we have reached the end
            while (FLAC__stream_decoder_get_state(_decoder) != FLAC__STREAM_DECODER_END_OF_STREAM
                   && _decodeBuffer.size() < minimumLength)
            {
                if (!FLAC__stream_decoder_process_single(_decoder))
                {
                    break;
                }
            }
        }

        void Unload()
        {
            if (_decoder != nullptr)
            {
                FLAC__stream_decoder_delete(_decoder);
            }
            if (_rw != nullptr)
            {
                SDL_RWclose(_rw);
                _rw = nullptr;
            }
        }

        static FLAC__StreamDecoderReadStatus FlacCallbackRead(
            const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            if (*bytes > 0)
            {
                *bytes = SDL_RWread(self->_rw, buffer, sizeof(FLAC__byte), *bytes);
                if (*bytes == 0)
                {
                    return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
                }
                else
                {
                    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
                }
            }
            else
            {
                return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
            }
        }

        static FLAC__StreamDecoderSeekStatus FlacCallbackSeek(
            const FLAC__StreamDecoder* decoder, FLAC__uint64 absoluteByteOffset, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            if (SDL_RWseek(self->_rw, absoluteByteOffset, RW_SEEK_SET) < 0)
            {
                return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
            }
            else
            {
                return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
            }
        }

        static FLAC__StreamDecoderTellStatus FlacCallbackTell(
            const FLAC__StreamDecoder* decoder, FLAC__uint64* absoluteByteOffset, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            auto pos = SDL_RWtell(self->_rw);
            if (pos < 0)
            {
                return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
            }
            else
            {
                *absoluteByteOffset = static_cast<FLAC__uint64>(pos);
                return FLAC__STREAM_DECODER_TELL_STATUS_OK;
            }
        }

        static FLAC__StreamDecoderLengthStatus FlacCallbackLength(
            const FLAC__StreamDecoder* decoder, FLAC__uint64* streamLength, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            auto pos = SDL_RWtell(self->_rw);
            auto length = SDL_RWseek(self->_rw, 0, RW_SEEK_END);
            if (SDL_RWseek(self->_rw, pos, RW_SEEK_SET) != pos || length < 0)
            {
                return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
            }
            else
            {
                *streamLength = static_cast<FLAC__uint64>(length);
                return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
            }
        }

        static FLAC__bool FlacCallbackEof(const FLAC__StreamDecoder* decoder, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            auto pos = SDL_RWtell(self->_rw);
            auto end = SDL_RWseek(self->_rw, 0, RW_SEEK_END);
            if (pos == end)
            {
                return true;
            }
            else
            {
                SDL_RWseek(self->_rw, pos, RW_SEEK_SET);
                return false;
            }
        }

        static FLAC__StreamDecoderWriteStatus FlacCallbackWrite(
            const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);

            // Determine sizes
            auto channels = self->_format.channels;
            auto sampleSize = sizeof(int16_t);
            auto frameSize = frame->header.blocksize * channels * sampleSize;
            int shiftAmount;
            switch (self->_bitsPerSample)
            {
                case 16:
                    shiftAmount = 0;
                    break;
                case 20:
                    shiftAmount = 4;
                    break;
                case 24:
                    shiftAmount = 8;
                    break;
                default:
                    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
            }

            // Allocate room on decode buffer
            auto& decodeBuffer = self->_decodeBuffer;
            decodeBuffer.resize(decodeBuffer.size() + frameSize);

            // Copy decoded data to buffer
            auto dst0 = reinterpret_cast<int16_t*>(decodeBuffer.data());
            for (int32_t i = 0; i < channels; i++)
            {
                auto* dst = dst0 + i;
                for (uint32_t j = 0; j < frame->header.blocksize; j++)
                {
                    *dst = static_cast<int16_t>(buffer[i][j] >> shiftAmount);
                    dst += channels;
                }
            }

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
        }

        static void FlacCallbackMetadata(
            const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata, void* clientData)
        {
            auto* self = reinterpret_cast<FlacAudioSource*>(clientData);
            if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
            {
                self->_bitsPerSample = metadata->data.stream_info.bits_per_sample;
                self->_totalSamples = metadata->data.stream_info.total_samples;
                self->_format.freq = metadata->data.stream_info.sample_rate;
                self->_format.format = AUDIO_S16LSB;
                self->_format.channels = metadata->data.stream_info.channels;
                self->_dataLength = self->_totalSamples * self->_format.channels * sizeof(int16_t);
            }
        }

        static void FlacCallbackError(
            const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* clientData)
        {
        }
    };

    std::unique_ptr<ISDLAudioSource> AudioSource::CreateStreamFromFlac(SDL_RWops* rw)
    {
        auto source = std::make_unique<FlacAudioSource>();
        if (!source->LoadFlac(rw))
        {
            source = nullptr;
        }
        return source;
    }
} // namespace OpenRCT2::Audio
