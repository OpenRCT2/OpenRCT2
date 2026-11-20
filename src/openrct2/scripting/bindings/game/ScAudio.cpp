/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScAudio.hpp"

    #include "../../../Context.h"
    #include "../../../audio/Audio.h"
    #include "../../../audio/AudioContext.h"
    #include "../../../audio/AudioSource.h"
    #include "../../../core/MemoryStream.h"
    #include "../../../world/Location.hpp"
    #include "../../Plugin.h"
    #include "../../ScriptEngine.h"
    #include "../../SoundNames.hpp"

    #include <algorithm>
    #include <cstring>
    #include <optional>
    #include <thirdparty/base64.hpp>

namespace OpenRCT2::Scripting
{
    struct CustomSound
    {
        int32_t Handle{};
        std::shared_ptr<Plugin> Owner;
        Audio::IAudioSource* Source{};
    };

    static std::vector<CustomSound> _customSounds;
    static int32_t _nextCustomSoundHandle = 1;
    static bool _cleanupSubscribed = false;

    static void FreePluginSounds(const std::shared_ptr<Plugin>& plugin)
    {
        for (auto it = _customSounds.begin(); it != _customSounds.end();)
        {
            if (it->Owner == plugin)
            {
                it->Source->Release();
                it = _customSounds.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    static void SubscribeToCleanup()
    {
        if (!_cleanupSubscribed)
        {
            _cleanupSubscribed = true;
            auto& scriptEngine = GetContext()->GetScriptEngine();
            scriptEngine.SubscribeToPluginStoppedEvent([](std::shared_ptr<Plugin> stopped) -> void { FreePluginSounds(stopped); });
        }
    }

    static CustomSound* FindCustomSound(int32_t handle)
    {
        auto it = std::find_if(
            _customSounds.begin(), _customSounds.end(), [handle](const CustomSound& s) { return s.Handle == handle; });
        return it != _customSounds.end() ? &*it : nullptr;
    }

    // Mirrors what the WAV loader accepts, so invalid files are rejected before reaching it.
    static bool IsSupportedWav(const std::vector<uint8_t>& d)
    {
        auto u16 = [&](size_t i) { return static_cast<uint32_t>(d[i] | (d[i + 1] << 8)); };
        auto u32 = [&](size_t i) { return u16(i) | (u16(i + 2) << 16); };

        if (d.size() < 12 || std::memcmp(&d[0], "RIFF", 4) != 0 || std::memcmp(&d[8], "WAVE", 4) != 0)
            return false;

        bool hasFmt = false;
        size_t pos = 12;
        while (pos + 8 <= d.size())
        {
            auto size = static_cast<size_t>(u32(pos + 4));
            if (std::memcmp(&d[pos], "fmt ", 4) == 0)
            {
                if (size < 16 || pos + 8 + 16 > d.size())
                    return false;
                auto bits = u16(pos + 8 + 14);
                if (u16(pos + 8) != 1 || (bits != 8 && bits != 16))
                    return false;
                hasFmt = true;
            }
            else if (std::memcmp(&d[pos], "data", 4) == 0)
            {
                return hasFmt && size != 0;
            }
            else if (
                std::memcmp(&d[pos], "fact", 4) != 0 && std::memcmp(&d[pos], "LIST", 4) != 0
                && std::memcmp(&d[pos], "bext", 4) != 0 && std::memcmp(&d[pos], "JUNK", 4) != 0)
            {
                return false;
            }
            pos += 8 + size;
        }
        return false;
    }

    JSValue ScAudio::loadSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(base64Data, ctx, argv[0]);

        std::vector<uint8_t> bytes;
        try
        {
            bytes = base64::decode_into<std::vector<uint8_t>>(base64Data);
        }
        catch (...)
        {
            JS_ThrowPlainError(ctx, "Invalid base64 data");
            return JS_EXCEPTION;
        }

        if (!IsSupportedWav(bytes))
        {
            JS_ThrowPlainError(ctx, "Unable to load sound. Only PCM WAV files (8 or 16 bit) are supported.");
            return JS_EXCEPTION;
        }

        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();

        auto& audioContext = GetContext()->GetAudioContext();
        auto* source = audioContext.CreateStreamFromWAV(std::make_unique<MemoryStream>(bytes));
        if (source == nullptr)
        {
            JS_ThrowPlainError(ctx, "Unable to load sound. Only PCM WAV files (8 or 16 bit) are supported.");
            return JS_EXCEPTION;
        }

        SubscribeToCleanup();

        auto handle = _nextCustomSoundHandle++;
        _customSounds.push_back({ handle, plugin, source });
        return JS_NewInt32(ctx, handle);
    }

    JSValue ScAudio::freeSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(handle, ctx, argv[0]);

        auto& scriptEngine = GetContext()->GetScriptEngine();
        auto plugin = scriptEngine.GetExecInfo().GetCurrentPlugin();
        auto* sound = FindCustomSound(handle);
        if (sound == nullptr || sound->Owner != plugin)
        {
            JS_ThrowPlainError(ctx, "This plugin did not load the specified sound.");
            return JS_EXCEPTION;
        }

        // The mixer drops channels whose source has been released, so any playing instances stop.
        sound->Source->Release();
        _customSounds.erase(_customSounds.begin() + (sound - _customSounds.data()));
        return JS_UNDEFINED;
    }

    // Reads the optional arguments shared by playSound and playMusic: position?, rate?
    // Returns false if a JS exception was raised.
    static bool ParsePlayArgs(JSContext* ctx, int argc, JSValue* argv, std::optional<CoordsXYZ>& position, double& rate)
    {
        if (argc > 1 && JS_IsObject(argv[1]))
        {
            position = JStoCoordsXYZ(ctx, argv[1]);
        }

        if (argc > 2 && !JS_IsUndefined(argv[2]) && !JS_IsNull(argv[2]))
        {
            if (JS_ToFloat64(ctx, &rate, argv[2]) < 0)
                return false;
        }
        return true;
    }

    static void PlaySource(Audio::IAudioSource* source, int32_t sampleModifier, Audio::MixerGroup group, const std::optional<CoordsXYZ>& position, double rate)
    {
        if (source == nullptr || !Audio::IsAvailable())
            return;

        int32_t volume = Audio::kMixerVolumeMax;
        float pan = Audio::DStoMixerPan(0);
        if (position.has_value() && !Audio::GetLocationParams(sampleModifier, *position, volume, pan))
            return;

        Audio::CreateAudioChannel(source, group, false, volume, pan, rate, true);
    }

    JSValue ScAudio::playSound(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        std::optional<CoordsXYZ> position;
        double rate = 1.0;
        if (!ParsePlayArgs(ctx, argc, argv, position, rate))
            return JS_EXCEPTION;

        if (JS_IsNumber(argv[0]))
        {
            JS_UNPACK_INT32(handle, ctx, argv[0]);
            auto* sound = FindCustomSound(handle);
            if (sound == nullptr)
            {
                JS_ThrowPlainError(ctx, "Invalid sound handle.");
                return JS_EXCEPTION;
            }
            PlaySource(sound->Source, 0, Audio::MixerGroup::sound, position, rate);
            return JS_UNDEFINED;
        }

        JS_UNPACK_STR(soundName, ctx, argv[0]);
        auto soundId = GetSoundByName(soundName);
        int32_t sampleModifier = 0;
        auto* source = Audio::GetSoundSource(soundId, &sampleModifier);
        PlaySource(source, sampleModifier, Audio::MixerGroup::sound, position, rate);
        return JS_UNDEFINED;
    }

    JSValue ScAudio::playMusic(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        std::optional<CoordsXYZ> position;
        double rate = 1.0;
        if (!ParsePlayArgs(ctx, argc, argv, position, rate))
            return JS_EXCEPTION;

        JS_UNPACK_STR(songName, ctx, argv[0]);
        PlaySource(Audio::GetMusicSource(songName), 0, Audio::MixerGroup::rideMusic, position, rate);
        return JS_UNDEFINED;
    }

    void ScAudio::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CFUNC_DEF("playSound", 3, ScAudio::playSound),
            JS_CFUNC_DEF("playMusic", 3, ScAudio::playMusic),
            JS_CFUNC_DEF("loadSound", 1, ScAudio::loadSound),
            JS_CFUNC_DEF("freeSound", 1, ScAudio::freeSound),
        };
        RegisterBase(ctx, "Audio", nullptr, funcs);
    }

    JSValue ScAudio::New(JSContext* ctx)
    {
        return MakeWithOpaque(ctx, nullptr);
    }

} // namespace OpenRCT2::Scripting

#endif
