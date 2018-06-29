/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cmath>
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
#include "core/Util.hpp"
#include "drawing/IDrawingEngine.h"
#include "localisation/Localisation.h"
#include "FileClassifier.h"
#include "GameState.h"
#include "network/network.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "OpenRCT2.h"
#include "paint/Painter.h"
#include "ParkImporter.h"
#include "platform/Crash.h"
#include "PlatformEnvironment.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/Scenario.h"
#include "scenario/ScenarioRepository.h"
#include "title/TitleScreen.h"
#include "title/TitleSequenceManager.h"
#include "ui/WindowManager.h"
#include "world/Park.h"
#include "Version.h"

#include "Editor.h"
#include "Game.h"
#include "Input.h"
#include "Intro.h"
#include "audio/audio.h"
#include "config/Config.h"
#include "drawing/LightFX.h"
#include "interface/Chat.h"
#include "interface/InteractiveConsole.h"
#include "interface/Viewport.h"
#include "Intro.h"
#include "localisation/Date.h"
#include "localisation/LocalisationService.h"
#include "network/DiscordService.h"
#include "network/Http.h"
#include "network/network.h"
#include "network/twitch.h"
#include "platform/platform.h"
#include "util/Util.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Localisation;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

namespace OpenRCT2
{
    class Context : public IContext
    {
    private:
        // Dependencies
        std::shared_ptr<IPlatformEnvironment> const _env;
        std::shared_ptr<IAudioContext> const _audioContext;
        std::shared_ptr<IUiContext> const _uiContext;

        // Services
        std::shared_ptr<LocalisationService> _localisationService;
        std::shared_ptr<IObjectRepository> _objectRepository;
        std::shared_ptr<IObjectManager> _objectManager;
        std::unique_ptr<ITrackDesignRepository> _trackDesignRepository;
        std::unique_ptr<IScenarioRepository> _scenarioRepository;
#ifdef __ENABLE_DISCORD__
        std::unique_ptr<DiscordService> _discordService;
#endif
        StdInOutConsole             _stdInOutConsole;
#ifndef DISABLE_HTTP
        Network::Http::Http _http;
#endif

        // Game states
        std::unique_ptr<TitleScreen> _titleScreen;
        std::unique_ptr<GameState> _gameState;

        int32_t _drawingEngineType = DRAWING_ENGINE_SOFTWARE;
        std::unique_ptr<IDrawingEngine> _drawingEngine;
        std::unique_ptr<Painter> _painter;

        bool    _initialised = false;
        bool    _isWindowMinimised = false;
        uint32_t  _lastTick = 0;
        uint32_t  _accumulator = 0;
        uint32_t  _lastUpdateTick = 0;
        bool    _variableFrame = false;

        /** If set, will end the OpenRCT2 game loop. Intentially private to this module so that the flag can not be set back to false. */
        bool _finished = false;

    public:
        // Singleton of Context.
        // Remove this when GetContext() is no longer called so that
        // multiple instances can be created in parallel
        static Context * Instance;

    public:
        Context(
            const std::shared_ptr<IPlatformEnvironment>& env,
            const std::shared_ptr<IAudioContext>& audioContext,
            const std::shared_ptr<IUiContext>& uiContext)
            : _env(env),
            _audioContext(audioContext),
            _uiContext(uiContext),
            _localisationService(std::make_shared<LocalisationService>(env))
        {
            Instance = this;
        }

        ~Context() override
        {
            window_close_all();
            object_manager_unload_all_objects();
            gfx_object_check_all_images_freed();
            gfx_unload_g2();
            gfx_unload_g1();
            config_release();

            Instance = nullptr;
        }

        std::shared_ptr<IAudioContext> GetAudioContext() override
        {
            return _audioContext;
        }

        std::shared_ptr<IUiContext> GetUiContext() override
        {
            return _uiContext;
        }

        GameState * GetGameState() override
        {
            return _gameState.get();
        }

        std::shared_ptr<IPlatformEnvironment> GetPlatformEnvironment() override
        {
            return _env;
        }

        Localisation::LocalisationService& GetLocalisationService() override
        {
            return *_localisationService;
        }

        std::shared_ptr<IObjectManager> GetObjectManager() override
        {
            return _objectManager;
        }

        std::shared_ptr<IObjectRepository> GetObjectRepository() override
        {
            return _objectRepository;
        }

        ITrackDesignRepository * GetTrackDesignRepository() override
        {
            return _trackDesignRepository.get();
        }

        IScenarioRepository * GetScenarioRepository() override
        {
            return _scenarioRepository.get();
        }

        int32_t GetDrawingEngineType() override
        {
            return _drawingEngineType;
        }

        IDrawingEngine * GetDrawingEngine() override
        {
            return _drawingEngine.get();
        }

        int32_t RunOpenRCT2(int argc, const char * * argv) override
        {
            if (Initialise())
            {
                Launch();
            }
            return gExitCode;
        }

        void WriteLine(const std::string &s) override
        {
            _stdInOutConsole.WriteLine(s);
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

        std::string GetPathLegacy(int32_t pathId) override
        {
            static constexpr const char * const LegacyFileNames[PATH_ID_END] =
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
                if (!(_env->GetDirectoryPath(DIRBASE::RCT1).empty()))
                {
                    auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT1, DIRID::DATA);
                    result = Path::Combine(dataPath, "css17.dat");
                }
                else
                {
                    auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
                    result = Path::Combine(dataPath, "css50.dat");
                }
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

            crash_init();

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

            // TODO add configuration option to allow multiple instances
            // if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
            //  log_fatal("OpenRCT2 is already running.");
            //  return false;
            // } //This comment was relocated so it would stay where it was in relation to the following lines of code.

            if (!gOpenRCT2Headless)
            {
                auto rct2InstallPath = GetOrPromptRCT2Path();
                if (rct2InstallPath.empty())
                {
                    return false;
                }
                _env->SetBasePath(DIRBASE::RCT2, rct2InstallPath);
            }

            _objectRepository = std::shared_ptr<IObjectRepository>(CreateObjectRepository(_env));
            _objectManager = std::shared_ptr<IObjectManager>(CreateObjectManager(_objectRepository));
            _trackDesignRepository = CreateTrackDesignRepository(_env);
            _scenarioRepository = CreateScenarioRepository(_env);
#ifdef __ENABLE_DISCORD__
            _discordService = std::make_unique<DiscordService>();
#endif

            try
            {
                _localisationService->OpenLanguage(gConfigGeneral.language, *_objectManager);
            }
            catch (const std::exception& e)
            {
                log_error("Failed to open configured language: %s", e.what());
                try
                {
                    _localisationService->OpenLanguage(LANGUAGE_ENGLISH_UK, *_objectManager);
                }
                catch (const std::exception&)
                {
                    log_fatal("Failed to open fallback language: %s", e.what());
                    return false;
                }
            }

            if (platform_process_is_elevated())
            {
                std::string elevationWarning = _localisationService->GetString(STR_ADMIN_NOT_RECOMMENDED);
                if (gOpenRCT2Headless)
                {
                    Console::Error::WriteLine(elevationWarning.c_str());
                }
                else
                {
                    _uiContext->ShowMessageBox(elevationWarning);
                }
            }

            if (!gOpenRCT2Headless)
            {
                _uiContext->CreateWindow();
            }

            // TODO Ideally we want to delay this until we show the title so that we can
            //      still open the game window and draw a progress screen for the creation
            //      of the object cache.
            _objectRepository->LoadOrConstruct(_localisationService->GetCurrentLanguage());

            // TODO Like objects, this can take a while if there are a lot of track designs
            //      its also really something really we might want to do in the background
            //      as its not required until the player wants to place a new ride.
            _trackDesignRepository->Scan(_localisationService->GetCurrentLanguage());

            _scenarioRepository->Scan(_localisationService->GetCurrentLanguage());
            TitleSequenceManager::Scan();

            if (!gOpenRCT2Headless)
            {
                audio_init();
                audio_populate_devices();
                audio_init_ride_sounds_and_info();
            }

            network_set_env(_env);
            chat_init();
            CopyOriginalUserFilesOver();

            if (!gOpenRCT2NoGraphics)
            {
                if (!LoadBaseGraphics())
                {
                    return false;
                }
#ifdef __ENABLE_LIGHTFX__
                lightfx_init();
#endif
            }
            gScenarioTicks = 0;
            util_srand((uint32_t)time(nullptr));
            input_reset_place_obj_modifier();
            viewport_init_all();

            _gameState = std::make_unique<GameState>();
            _gameState->InitAll(150);

            _titleScreen = std::make_unique<TitleScreen>(*_gameState);
            return true;
        }

        void InitialiseDrawingEngine() final override
        {
            assert(_drawingEngine == nullptr);
            assert(_painter == nullptr);

            _drawingEngineType = gConfigGeneral.drawing_engine;

            auto drawingEngineFactory = _uiContext->GetDrawingEngineFactory();
            auto drawingEngine = drawingEngineFactory->Create((DRAWING_ENGINE_TYPE)_drawingEngineType, _uiContext);

            if (drawingEngine == nullptr)
            {
                if (_drawingEngineType == DRAWING_ENGINE_SOFTWARE)
                {
                    _drawingEngineType = DRAWING_ENGINE_NONE;
                    log_fatal("Unable to create a drawing engine.");
                    exit(-1);
                }
                else
                {
                    log_error("Unable to create drawing engine. Falling back to software.");

                    // Fallback to software
                    gConfigGeneral.drawing_engine = DRAWING_ENGINE_SOFTWARE;
                    config_save_default();
                    drawing_engine_init();
                }
            }
            else
            {
                _painter = std::make_unique<Painter>(_uiContext);
                try
                {
                    drawingEngine->Initialise();
                    drawingEngine->SetVSync(gConfigGeneral.use_vsync);
                    _drawingEngine = std::unique_ptr<IDrawingEngine>(std::move(drawingEngine));
                }
                catch (const std::exception &ex)
                {
                    _painter = nullptr;
                    if (_drawingEngineType == DRAWING_ENGINE_SOFTWARE)
                    {
                        _drawingEngineType = DRAWING_ENGINE_NONE;
                        log_error(ex.what());
                        log_fatal("Unable to initialise a drawing engine.");
                        exit(-1);
                    }
                    else
                    {
                        log_error(ex.what());
                        log_error("Unable to initialise drawing engine. Falling back to software.");

                        // Fallback to software
                        gConfigGeneral.drawing_engine = DRAWING_ENGINE_SOFTWARE;
                        config_save_default();
                        drawing_engine_init();
                    }
                }
            }
        }

        void DisposeDrawingEngine() final override
        {
            _drawingEngine = nullptr;
            _painter = nullptr;
        }

        bool LoadParkFromFile(const std::string &path, bool loadTitleScreenOnFail) final override
        {
            try
            {
                auto fs = FileStream(path, FILE_MODE_OPEN);
                return LoadParkFromStream(&fs, path, loadTitleScreenOnFail);
            }
            catch (const std::exception &e)
            {
                Console::Error::WriteLine(e.what());
            }
            return false;
        }

        bool LoadParkFromStream(IStream * stream, const std::string &path, bool loadTitleScreenFirstOnFail) final override
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
                        parkImporter = ParkImporter::CreateS4();
                    }
                    else
                    {
                        // Save is an S6 (RCT2 format)
                        parkImporter = ParkImporter::CreateS6(_objectRepository, _objectManager);
                    }

                    try
                    {
                        auto result = parkImporter->LoadFromStream(stream, info.Type == FILE_TYPE::SCENARIO, false, path.c_str());
                        _objectManager->LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
                        parkImporter->Import();
                        String::Set(gScenarioSavePath, Util::CountOf(gScenarioSavePath), path.c_str());
                        String::Set(gCurrentLoadedPath, Util::CountOf(gCurrentLoadedPath), path.c_str());
                        gFirstTimeSaving = true;
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
                    catch (const ObjectLoadException& e)
                    {
                        // This option is used when loading parks from the command line
                        // to ensure that the title sequence loads before the window
                        if (loadTitleScreenFirstOnFail)
                        {
                            title_load();
                        }
                        // The path needs to be duplicated as it's a const here
                        // which the window function doesn't like
                        auto intent = Intent(WC_OBJECT_LOAD_ERROR);
                        intent.putExtra(INTENT_EXTRA_PATH, path);
                        intent.putExtra(INTENT_EXTRA_LIST, (void *)e.MissingObjects.data());
                        intent.putExtra(INTENT_EXTRA_LIST_COUNT, (uint32_t)e.MissingObjects.size());

                        auto windowManager = _uiContext->GetWindowManager();
                        windowManager->OpenIntent(&intent);
                    }
                    catch (const UnsupportedRCTCFlagException& e)
                    {
                        // This option is used when loading parks from the command line
                        // to ensure that the title sequence loads before the window
                        if (loadTitleScreenFirstOnFail)
                        {
                            title_load();
                        }

                        auto windowManager = _uiContext->GetWindowManager();
                        set_format_arg(0, uint16_t, e.Flag);
                        windowManager->ShowError(STR_FAILED_TO_LOAD_IMCOMPATIBLE_RCTC_FLAG, STR_NONE);
                    }
                    catch (const std::exception& e)
                    {
                        // If loading the SV6 or SV4 failed for a reason other than invalid objects
                        // the current park state will be corrupted so just go back to the title screen.
                        title_load();
                        Console::Error::WriteLine(e.what());
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
            if (!gfx_load_g1(*_env))
            {
                return false;
            }
            gfx_load_g2();
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
            if (gOpenRCT2Headless)
            {
                // NONE or OPEN are the only allowed actions for headless mode
                if (gOpenRCT2StartupAction != STARTUP_ACTION_OPEN)
                {
                    gOpenRCT2StartupAction = STARTUP_ACTION_NONE;
                }
            }
            else
            {
                if ((gOpenRCT2StartupAction == STARTUP_ACTION_TITLE) && gConfigGeneral.play_intro)
                {
                    gOpenRCT2StartupAction = STARTUP_ACTION_INTRO;
                }
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
                    size_t dataSize = Network::Http::DownloadPark(gOpenRCT2StartupActionPath, &data);
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

            // For now, only allow interactive console in headless mode
            if (gOpenRCT2Headless)
            {
                _stdInOutConsole.Start();
            }
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
            uint32_t currentTick = platform_get_ticks();

            if (_lastTick == 0)
            {
                _lastTick = currentTick;
            }

            uint32_t elapsed = currentTick - _lastTick;
            _lastTick = currentTick;
            _accumulator = std::min(_accumulator + elapsed, (uint32_t)GAME_UPDATE_MAX_THRESHOLD);

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
                _drawingEngine->BeginDraw();
                _painter->Paint(*_drawingEngine);
                _drawingEngine->EndDraw();
            }
        }

        void RunVariableFrame()
        {
            uint32_t currentTick = platform_get_ticks();

            bool draw = !_isWindowMinimised && !gOpenRCT2Headless;

            if (_lastTick == 0)
            {
                sprite_position_tween_reset();
                _lastTick = currentTick;
            }

            uint32_t elapsed = currentTick - _lastTick;

            _lastTick = currentTick;
            _accumulator = std::min(_accumulator + elapsed, (uint32_t)GAME_UPDATE_MAX_THRESHOLD);

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

                _drawingEngine->BeginDraw();
                _painter->Paint(*_drawingEngine);
                _drawingEngine->EndDraw();

                sprite_position_tween_restore();
            }
        }

        void Update()
        {
            uint32_t currentUpdateTick = platform_get_ticks();
            gTicksSinceLastUpdate = std::min<uint32_t>(currentUpdateTick - _lastUpdateTick, 500);
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
                _gameState->Update();
            }

#ifdef __ENABLE_DISCORD__
            if (_discordService != nullptr)
            {
                _discordService->Update();
            }
#endif

            twitch_update();
            chat_update();
            _stdInOutConsole.ProcessEvalQueue();
            _uiContext->Update();
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

    Context * Context::Instance = nullptr;

    std::unique_ptr<IContext> CreateContext()
    {
        return std::make_unique<Context>(CreatePlatformEnvironment(), CreateDummyAudioContext(), CreateDummyUiContext());
    }

    std::unique_ptr<IContext> CreateContext(
        const std::shared_ptr<IPlatformEnvironment>& env,
        const std::shared_ptr<Audio::IAudioContext>& audioContext,
        const std::shared_ptr<IUiContext>& uiContext)
    {
        return std::make_unique<Context>(env, audioContext, uiContext);
    }

    IContext * GetContext()
    {
        return Context::Instance;
    }
}

void context_init()
{
    GetContext()->GetUiContext()->GetWindowManager()->Init();
}

bool context_load_park_from_file(const utf8 * path)
{
    return GetContext()->LoadParkFromFile(path);
}

bool context_load_park_from_stream(void * stream)
{
    return GetContext()->LoadParkFromStream((IStream*)stream, "");
}

void openrct2_write_full_version_info(utf8 * buffer, size_t bufferSize)
{
    String::Set(buffer, bufferSize, gVersionInfoFull);
}

void openrct2_finish()
{
    GetContext()->Finish();
}

void context_setcurrentcursor(int32_t cursor)
{
    GetContext()->GetUiContext()->SetCursor((CURSOR_ID)cursor);
}

void context_update_cursor_scale()
{
    GetContext()->GetUiContext()->SetCursorScale(static_cast<uint8_t>(std::round(gConfigGeneral.window_scale)));
}

void context_hide_cursor()
{
    GetContext()->GetUiContext()->SetCursorVisible(false);
}

void context_show_cursor()
{
    GetContext()->GetUiContext()->SetCursorVisible(true);
}

void context_get_cursor_position(int32_t * x, int32_t * y)
{
    GetContext()->GetUiContext()->GetCursorPosition(x, y);
}

void context_get_cursor_position_scaled(int32_t * x, int32_t * y)
{
    context_get_cursor_position(x, y);

    // Compensate for window scaling.
    *x = (int32_t)std::ceil(*x / gConfigGeneral.window_scale);
    *y = (int32_t)std::ceil(*y / gConfigGeneral.window_scale);
}

void context_set_cursor_position(int32_t x, int32_t y)
{
    GetContext()->GetUiContext()->SetCursorPosition(x, y);
}

const CursorState * context_get_cursor_state()
{
    return GetContext()->GetUiContext()->GetCursorState();
}

const uint8_t * context_get_keys_state()
{
    return GetContext()->GetUiContext()->GetKeysState();
}

const uint8_t * context_get_keys_pressed()
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

void context_set_fullscreen_mode(int32_t mode)
{
    return GetContext()->GetUiContext()->SetFullscreenMode((FULLSCREEN_MODE)mode);
}

void context_recreate_window()
{
    GetContext()->GetUiContext()->RecreateWindow();
}

int32_t context_get_resolutions(Resolution * * outResolutions)
{
auto resolutions = GetContext()->GetUiContext()->GetFullscreenResolutions();
int32_t count = (int32_t)resolutions.size();
*outResolutions = Memory::AllocateArray<Resolution>(count);
std::copy_n(resolutions.begin(), count, *outResolutions);
return count;
}

int32_t context_get_width()
{
    return GetContext()->GetUiContext()->GetWidth();
}

int32_t context_get_height()
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

rct_window * context_open_detail_window(uint8_t type, int32_t id)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenDetails(type, id);
}

rct_window * context_open_intent(Intent * intent)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenIntent(intent);
}

void context_broadcast_intent(Intent * intent)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->BroadcastIntent(*intent);
}

void context_force_close_window_by_class(rct_windowclass windowClass)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->ForceClose(windowClass);
}

rct_window * context_show_error(rct_string_id title, rct_string_id message)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->ShowError(title, message);
}

void context_update_map_tooltip()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->UpdateMapTooltip();
}

void context_handle_input()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->HandleInput();
}

void context_input_handle_keyboard(bool isTitle)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->HandleKeyboard(isTitle);
}

void context_quit()
{
    GetContext()->Quit();
}

const utf8 * context_get_path_legacy(int32_t pathId)
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

/**
 * This function is deprecated.
 * Use IPlatformEnvironment instead.
 */
void platform_get_user_directory(utf8 * outPath, const utf8 * subDirectory, size_t outSize)
{
    auto env = GetContext()->GetPlatformEnvironment();
    auto path = env->GetDirectoryPath(DIRBASE::USER);
    if (!String::IsNullOrEmpty(subDirectory))
    {
        path = Path::Combine(path, subDirectory);
    }
    String::Set(outPath, outSize, path.c_str());
}

/**
 * This function is deprecated.
 * Use IPlatformEnvironment instead.
 */
void platform_get_openrct_data_path(utf8 * outPath, size_t outSize)
{
    auto env = GetContext()->GetPlatformEnvironment();
    auto path = env->GetDirectoryPath(DIRBASE::OPENRCT2);
    String::Set(outPath, outSize, path.c_str());
}
