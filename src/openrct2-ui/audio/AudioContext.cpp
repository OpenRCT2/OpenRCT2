/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "AudioContext.h"

#include "../SDLException.h"
#include "AudioMixer.h"
#include "SDLAudioSource.h"

#include <SDL3/SDL.h>
#include <memory>
#include <openrct2/Diagnostic.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/audio/AudioSource.h>
#include <openrct2/core/String.hpp>

namespace OpenRCT2::Audio
{
    class AudioContext final : public IAudioContext
    {
    private:
        static constexpr size_t kStreamMinSize = 2 * 1024 * 1024; // 2 MiB

        std::unique_ptr<AudioMixer> _audioMixer;

    public:
        AudioContext()
        {
            if (!SDL_Init(SDL_INIT_AUDIO))
            {
                Ui::SDLException::Throw("SDL_Init(SDL_INIT_AUDIO)");
            }
            _audioMixer = std::make_unique<AudioMixer>();
        }

        ~AudioContext() override
        {
            // Must close the mixer (which destroys its SDL_AudioStream) before quitting the audio
            // subsystem - otherwise the implicit ~AudioMixer() runs after SDL has torn it down.
            _audioMixer->Close();
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }

        IAudioMixer* GetMixer() override
        {
            return _audioMixer.get();
        }

        std::vector<std::string> GetOutputDevices() override
        {
            std::vector<std::string> devices;
            int numDevices = 0;
            auto* deviceIds = SDL_GetAudioPlaybackDevices(&numDevices);
            if (deviceIds != nullptr)
            {
                for (int32_t i = 0; i < numDevices; i++)
                {
                    devices.emplace_back(String::toStd(SDL_GetAudioDeviceName(deviceIds[i])));
                }
                SDL_free(deviceIds);
            }
            return devices;
        }

        void SetOutputDevice(const std::string& deviceName) override
        {
            const char* szDeviceName = nullptr;
            if (!deviceName.empty())
            {
                szDeviceName = deviceName.c_str();
            }
            _audioMixer->Init(szDeviceName);
        }

        IAudioSource* CreateStreamFromCSS(std::unique_ptr<IStream> stream, uint32_t index) override
        {
            auto* rw = StreamToSDL2(std::move(stream));
            if (rw == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<SDLAudioSource> source;
            try
            {
                source = CreateAudioSource(rw, index);
            }
            catch (const std::exception& e)
            {
                LOG_VERBOSE("Unable to create audio source: %s", e.what());
            }

            SDL_CloseIO(rw);

            if (source == nullptr)
            {
                return nullptr;
            }

            // Stream will already be in memory, so convert to target format
            auto& targetFormat = _audioMixer->GetFormat();
            source = source->ToMemory(targetFormat);

            return AddSource(std::move(source));
        }

        IAudioSource* CreateStreamFromWAV(std::unique_ptr<IStream> stream) override
        {
            auto* rw = StreamToSDL2(std::move(stream));
            if (rw == nullptr)
            {
                return nullptr;
            }

            try
            {
                auto source = CreateAudioSource(rw);

                // Load whole stream into memory if small enough
                auto dataLength = source->GetLength();
                if (dataLength < kStreamMinSize)
                {
                    auto& targetFormat = _audioMixer->GetFormat();
                    source = source->ToMemory(targetFormat);
                }

                return AddSource(std::move(source));
            }
            catch (const std::exception& e)
            {
                SDL_CloseIO(rw);
                LOG_VERBOSE("Unable to create audio source: %s", e.what());
                return nullptr;
            }
        }

        void StartTitleMusic() override
        {
        }

        void ToggleAllSounds() override
        {
        }
        void PauseSounds() override
        {
        }
        void UnpauseSounds() override
        {
        }

        void StopAll() override
        {
        }
        void StopCrowdSound() override
        {
        }
        void StopRideMusic() override
        {
        }
        void StopTitleMusic() override
        {
        }
        void StopVehicleSounds() override
        {
        }

    private:
        IAudioSource* AddSource(std::unique_ptr<SDLAudioSource> source)
        {
            return _audioMixer->AddSource(std::move(source));
        }

        static SDL_IOStream* StreamToSDL2(std::unique_ptr<IStream> stream)
        {
            SDL_IOStreamInterface iface{};
            SDL_INIT_INTERFACE(&iface);
            iface.seek = [](void* userdata, Sint64 offset, SDL_IOWhence whence) {
                auto ptr = static_cast<IStream*>(userdata);
                ptr->Seek(offset, static_cast<int>(whence));
                return static_cast<Sint64>(ptr->GetPosition());
            };
            iface.read = [](void* userdata, void* buf, size_t size, SDL_IOStatus*) {
                auto ptr = static_cast<IStream*>(userdata);
                return static_cast<size_t>(ptr->TryRead(buf, size));
            };
            iface.size = [](void* userdata) {
                auto ptr = static_cast<IStream*>(userdata);
                return static_cast<Sint64>(ptr->GetLength());
            };
            iface.close = [](void* userdata) {
                auto* ptr = static_cast<IStream*>(userdata);
                delete ptr;
                return true;
            };
            return SDL_OpenIO(&iface, stream.release());
        }
    };

    std::unique_ptr<IAudioContext> CreateAudioContext()
    {
        return std::make_unique<AudioContext>();
    }
} // namespace OpenRCT2::Audio
