#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include <exception>
#include <memory>
#include <string>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__
#include "audio/AudioContext.h"
#include "Context.h"
#include "ui/UiContext.h"
#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileScanner.h"
#include "core/FileStream.hpp"
#include "core/Guard.hpp"
#include "core/Math.hpp"
#include "core/MemoryStream.h"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "FileClassifier.h"
#include "network/network.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "platform/crash.h"
#include "PlatformEnvironment.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/ScenarioRepository.h"
#include "title/TitleScreen.h"
#include "title/TitleSequenceManager.h"
#include "ui/WindowManager.h"
#include "Version.h"

#include "audio/audio.h"
#include "config/Config.h"
#include "drawing/lightfx.h"
#include "Editor.h"
#include "game.h"
#include "input.h"
#include "interface/chat.h"
#include "interface/console.h"
#include "interface/themes.h"
#include "intro.h"
#include "localisation/localisation.h"
#include "network/http.h"
#include "network/network.h"
#include "network/twitch.h"
#include "object_list.h"
#include "platform/platform.h"
#include "rct1.h"
#include "rct2/interop.h"
#include "util/util.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

namespace OpenRCT2
{
    class Context : public IContext
    {
    private:
        // Dependencies
        IPlatformEnvironment * const    _env = nullptr;
        IAudioContext * const           _audioContext = nullptr;
        IUiContext * const              _uiContext = nullptr;

        // Services
        IObjectRepository *         _objectRepository = nullptr;
        IObjectManager *            _objectManager = nullptr;
        ITrackDesignRepository *    _trackDesignRepository = nullptr;
        IScenarioRepository *       _scenarioRepository = nullptr;

        // Game states
        TitleScreen * _titleScreen = nullptr;

        bool    _initialised = false;
        bool    _isWindowMinimised = false;
        uint32  _lastTick = 0;
        uint32  _accumulator = 0;
        uint32  _lastUpdateTick = 0;
        bool    _variableFrame = false;

        /** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to false. */
        bool _finished = false;

    public:
        // Singleton of Context.
        // Remove this when GetContext() is no longer called so that
        // multiple instances can be created in parallel
        static Context * Instance;

    public:
        Context(IPlatformEnvironment * env, IAudioContext * audioContext, IUiContext * uiContext)
            : _env(env),
              _audioContext(audioContext),
              _uiContext(uiContext)
        {
            Instance = this;
        }

        ~Context() override
        {
            network_close();
            http_dispose();
            language_close_all();
            object_manager_unload_all_objects();
            gfx_object_check_all_images_freed();
            gfx_unload_g2();
            gfx_unload_g1();
            config_release();
#ifndef DISABLE_NETWORK
            EVP_MD_CTX_destroy(gHashCTX);
#endif // DISABLE_NETWORK
            rct2_interop_dispose();

            delete _titleScreen;

            delete _scenarioRepository;
            delete _trackDesignRepository;
            delete _objectManager;
            delete _objectRepository;

            Instance = nullptr;
        }

        IAudioContext * GetAudioContext() override
        {
            return _audioContext;
        }

        IUiContext * GetUiContext() override
        {
            return _uiContext;
        }

        sint32 RunOpenRCT2(int argc, char * * argv) override
        {
            if (Initialise())
            {
                Launch();
            }
            return gExitCode;
        }

        /**
         * Causes the OpenRCT2 game loop to finish.
         */
        void Finish() override
        {
            _finished = true;
        }

        void Quit() override
        {
            gSavePromptMode = PM_QUIT;
            context_open_window(WC_SAVE_PROMPT);
        }

        std::string GetPathLegacy(sint32 pathId) override
        {
            static const char * const LegacyFileNames[PATH_ID_END] =
            {
                nullptr,
                nullptr,
                "css1.dat",
                "css2.dat",
                "css4.dat",
                "css5.dat",
                "css6.dat",
                "css7.dat",
                "css8.dat",
                "css9.dat",
                "css11.dat",
                "css12.dat",
                "css13.dat",
                "css14.dat",
                "css15.dat",
                "css3.dat",
                "css17.dat",
                "css18.dat",
                "css19.dat",
                "css20.dat",
                "css21.dat",
                "css22.dat",
                nullptr,
                "css23.dat",
                "css24.dat",
                "css25.dat",
                "css26.dat",
                "css27.dat",
                "css28.dat",
                "css29.dat",
                "css30.dat",
                "css31.dat",
                "css32.dat",
                "css33.dat",
                "css34.dat",
                "css35.dat",
                "css36.dat",
                "css37.dat",
                "css38.dat",
                "CUSTOM1.WAV",
                "CUSTOM2.WAV",
                "css39.dat",
                "css40.dat",
                "css41.dat",
                nullptr,
                "css42.dat",
                "css43.dat",
                "css44.dat",
                "css45.dat",
                "css46.dat",
                "css50.dat"
            };

            std::string result;
            if (pathId == PATH_ID_CSS50)
            {
                auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT1, DIRID::DATA);
                result = Path::Combine(dataPath, "css17.dat");
            }
            else if (pathId >= 0 && pathId < PATH_ID_END)
            {
                auto fileName = LegacyFileNames[pathId];
                if (fileName != nullptr)
                {
                    auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
                    result = Path::Combine(dataPath, fileName);
                }
            }
            return result;
        }

        bool Initialise() final override
        {
            if (_initialised)
            {
                throw std::runtime_error("Context already initialised.");
            }
            _initialised = true;

#ifndef DISABLE_NETWORK
            gHashCTX = EVP_MD_CTX_create();
            Guard::Assert(gHashCTX != nullptr, "EVP_MD_CTX_create failed");
#endif // DISABLE_NETWORK

            crash_init();

            if (!rct2_interop_setup_segment())
            {
                log_fatal("Unable to load RCT2 data sector");
                return false;
            }

            if (gConfigGeneral.last_run_version != nullptr && String::Equals(gConfigGeneral.last_run_version, OPENRCT2_VERSION))
            {
                gOpenRCT2ShowChangelog = false;
            }
            else
            {
                gOpenRCT2ShowChangelog = true;
                gConfigGeneral.last_run_version = String::Duplicate(OPENRCT2_VERSION);
                config_save_default();
            }

            if (platform_process_is_elevated())
            {
                std::string elevationWarning = "For security reasons, it is strongly recommended not to run OpenRCT2 with elevated permissions.";
                if (gOpenRCT2Headless)
                {
                    Console::Error::WriteLine(elevationWarning.c_str());
                }
                else
                {
                    _uiContext->ShowMessageBox(elevationWarning);
                }
            }

            auto rct2InstallPath = GetOrPromptRCT2Path();
            if (rct2InstallPath.empty())
            {
                return false;
            }
            _env->SetBasePath(DIRBASE::RCT2, rct2InstallPath);

            if (!gOpenRCT2Headless)
            {
                _uiContext->CreateWindow();
            }

            // TODO add configuration option to allow multiple instances
            // if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
            //  log_fatal("OpenRCT2 is already running.");
            //  return false;
            // }

            _objectRepository = CreateObjectRepository(_env);
            _objectManager = CreateObjectManager(_objectRepository);
            _trackDesignRepository = CreateTrackDesignRepository(_env);
            _scenarioRepository = CreateScenarioRepository(_env);

            if (!language_open(gConfigGeneral.language))
            {
                log_error("Failed to open configured language...");
                if (!language_open(LANGUAGE_ENGLISH_UK))
                {
                    log_fatal("Failed to open fallback language...");
                    return false;
                }
            }

            // TODO Ideally we want to delay this until we show the title so that we can
            //      still open the game window and draw a progress screen for the creation
            //      of the object cache.
            _objectRepository->LoadOrConstruct();

            // TODO Like objects, this can take a while if there are a lot of track designs
            //      its also really something really we might want to do in the background
            //      as its not required until the player wants to place a new ride.
            _trackDesignRepository->Scan();

            _scenarioRepository->Scan();
            TitleSequenceManager::Scan();

            if (!gOpenRCT2Headless)
            {
                audio_init();
                audio_populate_devices();
                audio_init_ride_sounds_and_info();
            }

            http_init();
            network_set_env(_env);
            chat_init();
            theme_manager_initialise();
            CopyOriginalUserFilesOver();

            rct2_interop_setup_hooks();

            if (!gOpenRCT2NoGraphics)
            {
                LoadBaseGraphics();
#ifdef __ENABLE_LIGHTFX__
                lightfx_init();
#endif
            }
            gScenarioTicks = 0;
            util_srand((uint32)time(0));
            input_reset_place_obj_modifier();
            viewport_init_all();
            game_init_all(150);

            _titleScreen = new TitleScreen();
            return true;
        }

        bool LoadParkFromFile(const std::string &path, bool loadTitleScreenOnFail = false) final override
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            return LoadParkFromStream(&fs, path, loadTitleScreenOnFail);
        }

    private:
        std::string GetOrPromptRCT2Path()
        {
            auto result = std::string();
            if (String::IsNullOrEmpty(gCustomRCT2DataPath))
            {
                // Check install directory
                if (gConfigGeneral.rct2_path == nullptr || !platform_original_game_data_exists(gConfigGeneral.rct2_path))
                {
                    log_verbose("install directory does not exist or invalid directory selected, %s", gConfigGeneral.rct2_path);
                    if (!config_find_or_browse_install_directory())
                    {
                        utf8 path[MAX_PATH];
                        config_get_default_path(path, sizeof(path));
                        Console::Error::WriteLine("An RCT2 install directory must be specified! Please edit \"game_path\" in %s.\n", path);
                        return std::string();
                    }
                }
                result = std::string(gConfigGeneral.rct2_path);
            }
            else
            {
                result = std::string(gCustomRCT2DataPath);
            }
            return result;
        }

        bool LoadBaseGraphics()
        {
            if (!gfx_load_g1(_env))
            {
                return false;
            }
            if (!gfx_load_g2())
            {
                return false;
            }
            gfx_load_csg();
            font_sprite_initialise_characters();
            return true;
        }

        /**
         * Launches the game, after command line arguments have been parsed and processed.
         */
        void Launch()
        {
            gIntroState = INTRO_STATE_NONE;
            if ((gOpenRCT2StartupAction == STARTUP_ACTION_TITLE) && gConfigGeneral.play_intro)
            {
                gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;
            }

            switch (gOpenRCT2StartupAction) {
            case STARTUP_ACTION_INTRO:
                gIntroState = INTRO_STATE_PUBLISHER_BEGIN;
                title_load();
                break;
            case STARTUP_ACTION_TITLE:
                title_load();
                break;
            case STARTUP_ACTION_OPEN:
            {
                // A path that includes "://" is illegal with all common filesystems, so it is almost certainly a URL
                // This way all cURL supported protocols, like http, ftp, scp and smb are automatically handled
                if (strstr(gOpenRCT2StartupActionPath, "://") != nullptr)
                {
#ifndef DISABLE_HTTP
                    // Download park and open it using its temporary filename
                    void * data;
                    size_t dataSize = http_download_park(gOpenRCT2StartupActionPath, &data);
                    if (dataSize == 0)
                    {
                        title_load();
                        break;
                    }

                    auto ms = MemoryStream(data, dataSize, MEMORY_ACCESS::OWNER);
                    if (!LoadParkFromStream(&ms, gOpenRCT2StartupActionPath, true))
                    {
                        Console::Error::WriteLine("Failed to load '%s'", gOpenRCT2StartupActionPath);
                        title_load();
                        break;
                    }
#endif
                }
                else
                {
                    try
                    {
                        if (!LoadParkFromFile(gOpenRCT2StartupActionPath, true))
                        {
                            break;
                        }
                    }
                    catch (const std::exception &ex)
                    {
                        Console::Error::WriteLine("Failed to load '%s'", gOpenRCT2StartupActionPath);
                        Console::Error::WriteLine("%s", ex.what());
                        title_load();
                        break;
                    }
                }

                gScreenFlags = SCREEN_FLAGS_PLAYING;

#ifndef DISABLE_NETWORK
                if (gNetworkStart == NETWORK_MODE_SERVER)
                {
                    if (gNetworkStartPort == 0)
                    {
                        gNetworkStartPort = gConfigNetwork.default_port;
                    }

                    if (String::IsNullOrEmpty(gNetworkStartAddress))
                    {
                        gNetworkStartAddress = gConfigNetwork.listen_address;
                    }

                    if (String::IsNullOrEmpty(gCustomPassword))
                    {
                        network_set_password(gConfigNetwork.default_password);
                    }
                    else
                    {
                        network_set_password(gCustomPassword);
                    }
                    network_begin_server(gNetworkStartPort, gNetworkStartAddress);
                }
#endif // DISABLE_NETWORK
                break;
            }
            case STARTUP_ACTION_EDIT:
                if (String::SizeOf(gOpenRCT2StartupActionPath) == 0)
                {
                    Editor::Load();
                }
                else if (!Editor::LoadLandscape(gOpenRCT2StartupActionPath))
                {
                    title_load();
                }
                break;
            }

#ifndef DISABLE_NETWORK
            if (gNetworkStart == NETWORK_MODE_CLIENT)
            {
                if (gNetworkStartPort == 0)
                {
                    gNetworkStartPort = gConfigNetwork.default_port;
                }
                network_begin_client(gNetworkStartHost, gNetworkStartPort);
            }
#endif // DISABLE_NETWORK

            RunGameLoop();
        }

        bool ShouldRunVariableFrame()
        {
            if (!gConfigGeneral.uncap_fps) return false;
            if (gGameSpeed > 4) return false;
            if (gOpenRCT2Headless) return false;
            if (_uiContext->IsMinimised()) return false;
            return true;
        }

        /**
        * Run the main game loop until the finished flag is set.
        */
        void RunGameLoop()
        {
            log_verbose("begin openrct2 loop");
            _finished = false;

#ifndef __EMSCRIPTEN__
            _variableFrame = ShouldRunVariableFrame();
            do
            {
                RunFrame();
            }
            while (!_finished);
#else
            emscripten_set_main_loop_arg([](void * vctx) ->
                {
                    auto ctx = reinterpret_cast<Context *>(vctx);
                    ctx->RunFrame();
                }, this, 0, 1);
#endif // __EMSCRIPTEN__
            log_verbose("finish openrct2 loop");
        }

        void RunFrame()
        {
            // Make sure we catch the state change and reset it.
            bool useVariableFrame = ShouldRunVariableFrame();
            if (_variableFrame != useVariableFrame)
            {
                _lastTick = 0;
                _variableFrame = useVariableFrame;
            }

            if (useVariableFrame)
            {
                RunVariableFrame();
            }
            else
            {
                RunFixedFrame();
            }
        }

        void RunFixedFrame()
        {
            uint32 currentTick = platform_get_ticks();

            if (_lastTick == 0)
            {
                _lastTick = currentTick;
            }

            uint32 elapsed = currentTick - _lastTick;
            _lastTick = currentTick;
            _accumulator = Math::Min(_accumulator + elapsed, (uint32)GAME_UPDATE_MAX_THRESHOLD);

            _uiContext->ProcessMessages();

            if (_accumulator < GAME_UPDATE_TIME_MS)
            {
                platform_sleep(GAME_UPDATE_TIME_MS - _accumulator - 1);
                return;
            }

            _accumulator -= GAME_UPDATE_TIME_MS;

            Update();
            if (!_isWindowMinimised && !gOpenRCT2Headless)
            {
                drawing_engine_draw();
            }
        }

        void RunVariableFrame()
        {
            uint32 currentTick = platform_get_ticks();

            bool draw = !_isWindowMinimised && !gOpenRCT2Headless;

            if (_lastTick == 0)
            {
                sprite_position_tween_reset();
                _lastTick = currentTick;
            }

            uint32 elapsed = currentTick - _lastTick;

            _lastTick = currentTick;
            _accumulator = Math::Min(_accumulator + elapsed, (uint32)GAME_UPDATE_MAX_THRESHOLD);

            _uiContext->ProcessMessages();

            while (_accumulator >= GAME_UPDATE_TIME_MS)
            {
                // Get the original position of each sprite
                if(draw)
                    sprite_position_tween_store_a();

                Update();

                _accumulator -= GAME_UPDATE_TIME_MS;

                // Get the next position of each sprite
                if(draw)
                    sprite_position_tween_store_b();
            }

            if (draw)
            {
                const float alpha = (float)_accumulator / GAME_UPDATE_TIME_MS;
                sprite_position_tween_all(alpha);

                drawing_engine_draw();

                sprite_position_tween_restore();
            }
        }

        void Update()
        {
            uint32 currentUpdateTick = platform_get_ticks();
            gTicksSinceLastUpdate = std::min<uint32>(currentUpdateTick - _lastUpdateTick, 500);
            _lastUpdateTick = currentUpdateTick;
            
            if (game_is_not_paused())
            {
                gPaletteEffectFrame += gTicksSinceLastUpdate;
            }

            date_update_real_time_of_day();

            if (gIntroState != INTRO_STATE_NONE)
            {
                intro_update();
            }
            else if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !gOpenRCT2Headless)
            {
                _titleScreen->Update();
            }
            else
            {
                game_update();
            }

            twitch_update();
            chat_update();
            console_update();
        }

        bool LoadParkFromStream(IStream * stream, const std::string &path, bool loadTitleScreenFirstOnFail)
        {
            ClassifiedFileInfo info;
            if (TryClassifyFile(stream, &info))
            {
                if (info.Type == FILE_TYPE::SAVED_GAME ||
                    info.Type == FILE_TYPE::SCENARIO)
                {
                    std::unique_ptr<IParkImporter> parkImporter;
                    if (info.Version <= FILE_TYPE_S4_CUTOFF)
                    {
                        // Save is an S4 (RCT1 format)
                        parkImporter.reset(ParkImporter::CreateS4());
                    }
                    else
                    {
                        // Save is an S6 (RCT2 format)
                        parkImporter.reset(ParkImporter::CreateS6(_objectRepository, _objectManager));
                    }

                    auto result = parkImporter->LoadFromStream(stream, info.Type == FILE_TYPE::SCENARIO, false, path.c_str());
                    if (result.Error == PARK_LOAD_ERROR_OK)
                    {
                        parkImporter->Import();
                        game_fix_save_vars();
                        sprite_position_tween_reset();
                        gScreenAge = 0;
                        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
                        if (info.Type == FILE_TYPE::SAVED_GAME)
                        {
                            if (network_get_mode() == NETWORK_MODE_CLIENT)
                            {
                                network_close();
                            }
                            game_load_init();
                            if (network_get_mode() == NETWORK_MODE_SERVER)
                            {
                                network_send_map();
                            }
                        }
                        else
                        {
                            scenario_begin();
                            if (network_get_mode() == NETWORK_MODE_SERVER)
                            {
                                    network_send_map();
                            }
                            if (network_get_mode() == NETWORK_MODE_CLIENT)
                            {
                                network_close();
                            }
                        }
                        // This ensures that the newly loaded save reflects the user's
                        // 'show real names of guests' option, now that it's a global setting
                        peep_update_names(gConfigGeneral.show_real_names_of_guests);
                        return true;
                    }
                    else
                    {
                        handle_park_load_failure_with_title_opt(&result, path.c_str(), loadTitleScreenFirstOnFail);
                    }

                }
                else
                {
                    Console::Error::WriteLine("Invalid file type.");
                }
            }
            else
            {
                Console::Error::WriteLine("Unable to detect file type.");
            }
            return false;
        }

        /**
        * Copy saved games and landscapes to user directory
        */
        void CopyOriginalUserFilesOver()
        {
            CopyOriginalUserFilesOver(DIRID::SAVE, "*.sv6");
            CopyOriginalUserFilesOver(DIRID::LANDSCAPE, "*.sc6");
        }

        void CopyOriginalUserFilesOver(DIRID dirid, const std::string &pattern)
        {
            auto src = _env->GetDirectoryPath(DIRBASE::RCT2, dirid);
            auto dst = _env->GetDirectoryPath(DIRBASE::USER, dirid);
            CopyOriginalUserFilesOver(src, dst, pattern);
        }

        void CopyOriginalUserFilesOver(const std::string &srcRoot, const std::string &dstRoot, const std::string &pattern)
        {
            log_verbose("CopyOriginalUserFilesOver('%s', '%s', '%s')", srcRoot.c_str(), dstRoot.c_str(), pattern.c_str());

            auto scanPattern = Path::Combine(srcRoot, pattern);
            auto scanner = Path::ScanDirectory(scanPattern, true);
            while (scanner->Next())
            {
                auto src = std::string(scanner->GetPath());
                auto dst = Path::Combine(dstRoot, scanner->GetPathRelative());
                auto dstDirectory = Path::GetDirectory(dst);

                // Create the directory if necessary
                if (!platform_directory_exists(dstDirectory.c_str()))
                {
                    Console::WriteLine("Creating directory '%s'", dstDirectory.c_str());
                    if (!platform_ensure_directory_exists(dstDirectory.c_str()))
                    {
                        Console::Error::WriteLine("Could not create directory %s.", dstDirectory.c_str());
                        break;
                    }
                }

                // Only copy the file if it doesn't already exist
                if (!File::Exists(dst))
                {
                    Console::WriteLine("Copying '%s' to '%s'", src.c_str(), dst.c_str());
                    if (!File::Copy(src, dst, false))
                    {
                        Console::Error::WriteLine("Failed to copy '%s' to '%s'", src.c_str(), dst.c_str());
                    }
                }
            }
            delete scanner;
        }
    };

    class PlainContext final : public Context
    {
        std::unique_ptr<IPlatformEnvironment>   _env;
        std::unique_ptr<IAudioContext>          _audioContext;
        std::unique_ptr<IUiContext>             _uiContext;

    public:
        PlainContext()
            : PlainContext(CreatePlatformEnvironment(), CreateDummyAudioContext(), CreateDummyUiContext())
        {
        }

        PlainContext(IPlatformEnvironment * env, IAudioContext * audioContext, IUiContext * uiContext)
            : Context(env, audioContext, uiContext)
        {
            _env = std::unique_ptr<IPlatformEnvironment>(env);
            _audioContext = std::unique_ptr<IAudioContext>(audioContext);
            _uiContext = std::unique_ptr<IUiContext>(uiContext);
        }
    };

    Context * Context::Instance = nullptr;

    IContext * CreateContext()
    {
        return new PlainContext();
    }

    IContext * CreateContext(IPlatformEnvironment * env, Audio::IAudioContext * audioContext, IUiContext * uiContext)
    {
        return new Context(env, audioContext, uiContext);
    }

    IContext * GetContext()
    {
        return Context::Instance;
    }
}

extern "C"
{
    void context_init()
    {
        GetContext()->GetUiContext()->GetWindowManager()->Init();
    }

    bool context_load_park_from_file(const utf8 * path)
    {
        return GetContext()->LoadParkFromFile(path);
    }

    void openrct2_write_full_version_info(utf8 * buffer, size_t bufferSize)
    {
        String::Set(buffer, bufferSize, gVersionInfoFull);
    }

    void openrct2_finish()
    {
        GetContext()->Finish();
    }

    void context_setcurrentcursor(sint32 cursor)
    {
        GetContext()->GetUiContext()->SetCursor((CURSOR_ID)cursor);
    }

    void context_hide_cursor()
    {
        GetContext()->GetUiContext()->SetCursorVisible(false);
    }

    void context_show_cursor()
    {
        GetContext()->GetUiContext()->SetCursorVisible(true);
    }

    void context_get_cursor_position(sint32 * x, sint32 * y)
    {
        GetContext()->GetUiContext()->GetCursorPosition(x, y);
    }

    void context_get_cursor_position_scaled(sint32 * x, sint32 * y)
    {
        context_get_cursor_position(x, y);

        // Compensate for window scaling.
        *x = (sint32)ceilf(*x / gConfigGeneral.window_scale);
        *y = (sint32)ceilf(*y / gConfigGeneral.window_scale);
    }

    void context_set_cursor_position(sint32 x, sint32 y)
    {
        GetContext()->GetUiContext()->SetCursorPosition(x, y);
    }

    const CursorState * context_get_cursor_state()
    {
        return GetContext()->GetUiContext()->GetCursorState();
    }

    const uint8 * context_get_keys_state()
    {
        return GetContext()->GetUiContext()->GetKeysState();
    }

    const uint8 * context_get_keys_pressed()
    {
        return GetContext()->GetUiContext()->GetKeysPressed();
    }

    TextInputSession * context_start_text_input(utf8 * buffer, size_t maxLength)
    {
        return GetContext()->GetUiContext()->StartTextInput(buffer, maxLength);
    }

    void context_stop_text_input()
    {
        GetContext()->GetUiContext()->StopTextInput();
    }

    bool context_is_input_active()
    {
        return GetContext()->GetUiContext()->IsTextInputActive();
    }

    void context_trigger_resize()
    {
        return GetContext()->GetUiContext()->TriggerResize();
    }

    void context_set_fullscreen_mode(sint32 mode)
    {
        return GetContext()->GetUiContext()->SetFullscreenMode((FULLSCREEN_MODE)mode);
    }

    void context_recreate_window()
    {
        GetContext()->GetUiContext()->RecreateWindow();
    }

    sint32 context_get_resolutions(Resolution * * outResolutions)
    {
        auto resolutions = GetContext()->GetUiContext()->GetFullscreenResolutions();
        sint32 count = (sint32)resolutions.size();
        *outResolutions = Memory::AllocateArray<Resolution>(count);
        Memory::CopyArray(*outResolutions, resolutions.data(), count);
        return count;
    }

    sint32 context_get_width()
    {
        return GetContext()->GetUiContext()->GetWidth();
    }

    sint32 context_get_height()
    {
        return GetContext()->GetUiContext()->GetHeight();
    }

    bool context_has_focus()
    {
        return GetContext()->GetUiContext()->HasFocus();
    }

    void context_set_cursor_trap(bool value)
    {
        GetContext()->GetUiContext()->SetCursorTrap(value);
    }

    rct_window * context_open_window(rct_windowclass wc)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        return windowManager->OpenWindow(wc);
    }

    rct_window * context_open_window_view(rct_windowclass wc)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        return windowManager->OpenView(wc);
    }

    rct_window * context_open_detail_window(uint8 type, sint32 id)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        return windowManager->OpenDetails(type, id);
    }

    rct_window * context_open_intent(Intent * intent)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        return windowManager->OpenIntent(intent);
    }

    rct_window * context_show_error(rct_string_id title, rct_string_id message)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        return windowManager->ShowError(title, message);
    }

    void context_input_handle_keyboard(bool isTitle)
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        windowManager->HandleKeyboard(isTitle);
    }

    bool context_read_bmp(void * * outPixels, uint32 * outWidth, uint32 * outHeight, const utf8 * path)
    {
        return GetContext()->GetUiContext()->ReadBMP(outPixels, outWidth, outHeight, std::string(path));
    }

    void context_quit()
    {
        GetContext()->Quit();
    }

    const utf8 * context_get_path_legacy(sint32 pathId)
    {
        static utf8 result[MAX_PATH];
        auto path = GetContext()->GetPathLegacy(pathId);
        String::Set(result, sizeof(result), path.c_str());
        return result;
    }

    bool platform_open_common_file_dialog(utf8 * outFilename, file_dialog_desc * desc, size_t outSize)
    {
        try
        {
            FileDialogDesc desc2;
            desc2.Type = (FILE_DIALOG_TYPE)desc->type;
            desc2.Title = String::ToStd(desc->title);
            desc2.InitialDirectory = String::ToStd(desc->initial_directory);
            desc2.DefaultFilename = String::ToStd(desc->default_filename);
            for (const auto &filter : desc->filters)
            {
                if (filter.name != nullptr)
                {
                    desc2.Filters.push_back({ String::ToStd(filter.name), String::ToStd(filter.pattern) });
                }
            }
            std::string result = GetContext()->GetUiContext()->ShowFileDialog(desc2);
            String::Set(outFilename, outSize, result.c_str());
            return !result.empty();
        }
        catch (const std::exception &ex)
        {
            log_error(ex.what());
            outFilename[0] = '\0';
            return false;
        }
    }

    utf8 * platform_open_directory_browser(const utf8 * title)
    {
        try
        {
            std::string result = GetContext()->GetUiContext()->ShowDirectoryDialog(title);
            return String::Duplicate(result.c_str());
        }
        catch (const std::exception &ex)
        {
            log_error(ex.what());
            return nullptr;
        }
    }

    bool platform_place_string_on_clipboard(utf8* target)
    {
        return GetContext()->GetUiContext()->SetClipboardText(target);
    }
}
