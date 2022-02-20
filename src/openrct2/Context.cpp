/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#endif // __EMSCRIPTEN__

#include "Context.h"
#include "Editor.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "GameStateSnapshots.h"
#include "Input.h"
#include "Intro.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "PlatformEnvironment.h"
#include "ReplayManager.h"
#include "Version.h"
#include "actions/GameAction.h"
#include "audio/AudioContext.h"
#include "audio/audio.h"
#include "config/Config.h"
#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileScanner.h"
#include "core/FileStream.h"
#include "core/Guard.hpp"
#include "core/Http.h"
#include "core/MemoryStream.h"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "core/Timer.hpp"
#include "drawing/IDrawingEngine.h"
#include "drawing/Image.h"
#include "drawing/LightFX.h"
#include "entity/EntityRegistry.h"
#include "entity/EntityTweener.h"
#include "interface/Chat.h"
#include "interface/InteractiveConsole.h"
#include "interface/Viewport.h"
#include "localisation/Date.h"
#include "localisation/Formatter.h"
#include "localisation/Localisation.h"
#include "localisation/LocalisationService.h"
#include "network/DiscordService.h"
#include "network/NetworkBase.h"
#include "network/network.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "paint/Painter.h"
#include "platform/Crash.h"
#include "platform/Platform.h"
#include "profiling/Profiling.h"
#include "ride/TrackData.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/Scenario.h"
#include "scenario/ScenarioRepository.h"
#include "scripting/HookEngine.h"
#include "scripting/ScriptEngine.h"
#include "title/TitleScreen.h"
#include "title/TitleSequenceManager.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "util/Util.h"
#include "world/Park.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <future>
#include <iterator>
#include <memory>
#include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Localisation;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::Ui;

namespace OpenRCT2
{
    class Context final : public IContext
    {
    private:
        // Dependencies
        std::shared_ptr<IPlatformEnvironment> const _env;
        std::shared_ptr<IAudioContext> const _audioContext;
        std::shared_ptr<IUiContext> const _uiContext;

        // Services
        std::unique_ptr<LocalisationService> _localisationService;
        std::unique_ptr<IObjectRepository> _objectRepository;
        std::unique_ptr<IObjectManager> _objectManager;
        std::unique_ptr<ITrackDesignRepository> _trackDesignRepository;
        std::unique_ptr<IScenarioRepository> _scenarioRepository;
        std::unique_ptr<IReplayManager> _replayManager;
        std::unique_ptr<IGameStateSnapshots> _gameStateSnapshots;
#ifdef __ENABLE_DISCORD__
        std::unique_ptr<DiscordService> _discordService;
#endif
        StdInOutConsole _stdInOutConsole;
#ifdef ENABLE_SCRIPTING
        ScriptEngine _scriptEngine;
#endif
#ifndef DISABLE_NETWORK
        NetworkBase _network;
#endif

        // Game states
        std::unique_ptr<TitleScreen> _titleScreen;
        std::unique_ptr<GameState> _gameState;

        DrawingEngine _drawingEngineType = DrawingEngine::Software;
        std::unique_ptr<IDrawingEngine> _drawingEngine;
        std::unique_ptr<Painter> _painter;

        bool _initialised = false;

        Timer _timer;
        float _ticksAccumulator = 0.0f;
        float _realtimeAccumulator = 0.0f;
        float _timeScale = 1.0f;
        bool _variableFrame = false;

        // If set, will end the OpenRCT2 game loop. Intentionally private to this module so that the flag can not be set back to
        // false.
        bool _finished = false;

        std::future<void> _versionCheckFuture;
        NewVersionInfo _newVersionInfo;
        bool _hasNewVersionInfo = false;

    public:
        // Singleton of Context.
        // Remove this when GetContext() is no longer called so that
        // multiple instances can be created in parallel
        static Context* Instance;

    public:
        Context(
            const std::shared_ptr<IPlatformEnvironment>& env, const std::shared_ptr<IAudioContext>& audioContext,
            const std::shared_ptr<IUiContext>& uiContext)
            : _env(env)
            , _audioContext(audioContext)
            , _uiContext(uiContext)
            , _localisationService(std::make_unique<LocalisationService>(env))
#ifdef ENABLE_SCRIPTING
            , _scriptEngine(_stdInOutConsole, *env)
#endif
#ifndef DISABLE_NETWORK
            , _network(*this)
#endif
            , _painter(std::make_unique<Painter>(uiContext))
        {
            // Can't have more than one context currently.
            Guard::Assert(Instance == nullptr);

            Instance = this;
        }

        ~Context() override
        {
            // NOTE: We must shutdown all systems here before Instance is set back to null.
            //       If objects use GetContext() in their destructor things won't go well.

            GameActions::ClearQueue();
#ifndef DISABLE_NETWORK
            _network.Close();
#endif
            window_close_all();

            // Unload objects after closing all windows, this is to overcome windows like
            // the object selection window which loads objects when closed.
            if (_objectManager != nullptr)
            {
                _objectManager->UnloadAll();
            }

            gfx_object_check_all_images_freed();
            gfx_unload_csg();
            gfx_unload_g2();
            gfx_unload_g1();
            Audio::Close();
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

#ifdef ENABLE_SCRIPTING
        Scripting::ScriptEngine& GetScriptEngine() override
        {
            return _scriptEngine;
        }
#endif

        GameState* GetGameState() override
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

        IObjectManager& GetObjectManager() override
        {
            return *_objectManager;
        }

        IObjectRepository& GetObjectRepository() override
        {
            return *_objectRepository;
        }

        ITrackDesignRepository* GetTrackDesignRepository() override
        {
            return _trackDesignRepository.get();
        }

        IScenarioRepository* GetScenarioRepository() override
        {
            return _scenarioRepository.get();
        }

        IReplayManager* GetReplayManager() override
        {
            return _replayManager.get();
        }

        IGameStateSnapshots* GetGameStateSnapshots() override
        {
            return _gameStateSnapshots.get();
        }

        DrawingEngine GetDrawingEngineType() override
        {
            return _drawingEngineType;
        }

        IDrawingEngine* GetDrawingEngine() override
        {
            return _drawingEngine.get();
        }

        Paint::Painter* GetPainter() override
        {
            return _painter.get();
        }

#ifndef DISABLE_NETWORK
        NetworkBase& GetNetwork() override
        {
            return _network;
        }
#endif

        int32_t RunOpenRCT2(int argc, const char** argv) override
        {
            if (Initialise())
            {
                Launch();
                return EXIT_SUCCESS;
            }
            return EXIT_FAILURE;
        }

        void WriteLine(const std::string& s) override
        {
            _stdInOutConsole.WriteLine(s);
        }

        void WriteErrorLine(const std::string& s) override
        {
            _stdInOutConsole.WriteLineError(s);
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
            gSavePromptMode = PromptMode::Quit;
            context_open_window(WC_SAVE_PROMPT);
        }

        std::string GetPathLegacy(int32_t pathId) override
        {
            static constexpr const char* const LegacyFileNames[PATH_ID_END] = {
                nullptr,       nullptr,     "css1.dat",  "css2.dat",  "css4.dat",  "css5.dat",  "css6.dat",  "css7.dat",
                "css8.dat",    "css9.dat",  "css11.dat", "css12.dat", "css13.dat", "css14.dat", "css15.dat", "css3.dat",
                "css17.dat",   "css18.dat", "css19.dat", "css20.dat", "css21.dat", "css22.dat", nullptr,     "css23.dat",
                "css24.dat",   "css25.dat", "css26.dat", "css27.dat", "css28.dat", "css29.dat", "css30.dat", "css31.dat",
                "css32.dat",   "css33.dat", "css34.dat", "css35.dat", "css36.dat", "css37.dat", "css38.dat", "CUSTOM1.WAV",
                "CUSTOM2.WAV", "css39.dat", "css40.dat", "css41.dat", nullptr,     "css42.dat", "css43.dat", "css44.dat",
                "css45.dat",   "css46.dat", "css50.dat",
            };

            std::string result;
            if (pathId == PATH_ID_CSS50)
            {
                if (!(_env->GetDirectoryPath(DIRBASE::RCT1).empty()))
                {
                    auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT1, DIRID::DATA);
                    result = Path::ResolveCasing(Path::Combine(dataPath, u8"css17.dat"));

                    if (!File::Exists(result))
                    {
                        auto rct1Path = _env->GetDirectoryPath(DIRBASE::RCT1);
                        result = Path::ResolveCasing(Path::Combine(rct1Path, u8"RCTdeluxe_install", u8"Data", u8"css17.dat"));
                    }
                }
                else
                {
                    auto dataPath = _env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
                    result = Path::ResolveCasing(Path::Combine(dataPath, u8"css50.dat"));
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

            if (String::Equals(gConfigGeneral.last_run_version, OPENRCT2_VERSION))
            {
                gOpenRCT2ShowChangelog = false;
            }
            else
            {
                gOpenRCT2ShowChangelog = true;
                gConfigGeneral.last_run_version = OPENRCT2_VERSION;
                config_save_default();
            }

            try
            {
                _localisationService->OpenLanguage(gConfigGeneral.language);
            }
            catch (const std::exception& e)
            {
                log_error("Failed to open configured language: %s", e.what());
                try
                {
                    _localisationService->OpenLanguage(LANGUAGE_ENGLISH_UK);
                }
                catch (const std::exception& eFallback)
                {
                    log_fatal("Failed to open fallback language: %s", eFallback.what());
                    auto uiContext = GetContext()->GetUiContext();
                    uiContext->ShowMessageBox("Failed to load language file!\nYour installation may be damaged.");
                    return false;
                }
            }

            // TODO add configuration option to allow multiple instances
            // if (!gOpenRCT2Headless && !Platform::LockSingleInstance()) {
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

            _objectRepository = CreateObjectRepository(_env);
            _objectManager = CreateObjectManager(*_objectRepository);
            _trackDesignRepository = CreateTrackDesignRepository(_env);
            _scenarioRepository = CreateScenarioRepository(_env);
            _replayManager = CreateReplayManager();
            _gameStateSnapshots = CreateGameStateSnapshots();
#ifdef __ENABLE_DISCORD__
            if (!gOpenRCT2Headless)
            {
                _discordService = std::make_unique<DiscordService>();
            }
#endif

            if (Platform::ProcessIsElevated())
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

            if (Platform::IsRunningInWine())
            {
                std::string wineWarning = _localisationService->GetString(STR_WINE_NOT_RECOMMENDED);
                if (gOpenRCT2Headless)
                {
                    Console::Error::WriteLine(wineWarning.c_str());
                }
                else
                {
                    _uiContext->ShowMessageBox(wineWarning);
                }
            }

            if (!gOpenRCT2Headless)
            {
                _uiContext->CreateWindow();
            }

            EnsureUserContentDirectoriesExist();

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
                Init();
                PopulateDevices();
                InitRideSoundsAndInfo();
                gGameSoundsOff = !gConfigSound.master_sound_enabled;
            }

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

            input_reset_place_obj_modifier();
            viewport_init_all();

            _gameState = std::make_unique<GameState>();
            _gameState->InitAll(DEFAULT_MAP_SIZE);

            _titleScreen = std::make_unique<TitleScreen>(*_gameState);
            _uiContext->Initialise();

            return true;
        }

        void InitialiseDrawingEngine() final override
        {
            assert(_drawingEngine == nullptr);

            _drawingEngineType = gConfigGeneral.drawing_engine;

            auto drawingEngineFactory = _uiContext->GetDrawingEngineFactory();
            auto drawingEngine = drawingEngineFactory->Create(_drawingEngineType, _uiContext);

            if (drawingEngine == nullptr)
            {
                if (_drawingEngineType == DrawingEngine::Software)
                {
                    _drawingEngineType = DrawingEngine::None;
                    log_fatal("Unable to create a drawing engine.");
                    exit(-1);
                }
                else
                {
                    log_error("Unable to create drawing engine. Falling back to software.");

                    // Fallback to software
                    gConfigGeneral.drawing_engine = DrawingEngine::Software;
                    config_save_default();
                    drawing_engine_init();
                }
            }
            else
            {
                try
                {
                    drawingEngine->Initialise();
                    drawingEngine->SetVSync(gConfigGeneral.use_vsync);
                    _drawingEngine = std::move(drawingEngine);
                }
                catch (const std::exception& ex)
                {
                    if (_drawingEngineType == DrawingEngine::Software)
                    {
                        _drawingEngineType = DrawingEngine::None;
                        log_error(ex.what());
                        log_fatal("Unable to initialise a drawing engine.");
                        exit(-1);
                    }
                    else
                    {
                        log_error(ex.what());
                        log_error("Unable to initialise drawing engine. Falling back to software.");

                        // Fallback to software
                        gConfigGeneral.drawing_engine = DrawingEngine::Software;
                        config_save_default();
                        drawing_engine_init();
                    }
                }
            }

            window_check_all_valid_zoom();
        }

        void DisposeDrawingEngine() final override
        {
            _drawingEngine = nullptr;
        }

        bool LoadParkFromFile(
            const std::string& path, bool loadTitleScreenOnFail = false, bool asScenario = false) final override
        {
            log_verbose("Context::LoadParkFromFile(%s)", path.c_str());
            try
            {
                if (String::Equals(Path::GetExtension(path), ".sea", true))
                {
                    auto data = DecryptSea(fs::u8path(path));
                    auto ms = MemoryStream(data.data(), data.size(), MEMORY_ACCESS::READ);
                    if (!LoadParkFromStream(&ms, path, loadTitleScreenOnFail, asScenario))
                    {
                        throw std::runtime_error(".sea file may have been renamed.");
                    }
                    return true;
                }

                auto fs = FileStream(path, FILE_MODE_OPEN);
                if (!LoadParkFromStream(&fs, path, loadTitleScreenOnFail, asScenario))
                {
                    return false;
                }
                return true;
            }
            catch (const std::exception& e)
            {
                Console::Error::WriteLine(e.what());
                if (loadTitleScreenOnFail)
                {
                    title_load();
                }
                auto windowManager = _uiContext->GetWindowManager();
                windowManager->ShowError(STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, STR_NONE, {});
            }
            return false;
        }

        bool LoadParkFromStream(
            IStream* stream, const std::string& path, bool loadTitleScreenFirstOnFail = false,
            bool asScenario = false) final override
        {
            try
            {
                ClassifiedFileInfo info;
                if (!TryClassifyFile(stream, &info))
                {
                    throw std::runtime_error("Unable to detect file type");
                }

                if (info.Type != FILE_TYPE::PARK && info.Type != FILE_TYPE::SAVED_GAME && info.Type != FILE_TYPE::SCENARIO)
                {
                    throw std::runtime_error("Invalid file type.");
                }

                std::unique_ptr<IParkImporter> parkImporter;
                if (info.Type == FILE_TYPE::PARK)
                {
                    parkImporter = ParkImporter::CreateParkFile(*_objectRepository);
                }
                else if (info.Version <= FILE_TYPE_S4_CUTOFF)
                {
                    // Save is an S4 (RCT1 format)
                    parkImporter = ParkImporter::CreateS4();
                }
                else
                {
                    // Save is an S6 (RCT2 format)
                    parkImporter = ParkImporter::CreateS6(*_objectRepository);
                }

                auto result = parkImporter->LoadFromStream(stream, info.Type == FILE_TYPE::SCENARIO, false, path.c_str());

                // From this point onwards the currently loaded park will be corrupted if loading fails
                // so reload the title screen if that happens.
                loadTitleScreenFirstOnFail = true;

                game_unload_scripts();
                _objectManager->LoadObjects(result.RequiredObjects);
                parkImporter->Import();
                gScenarioSavePath = path;
                gCurrentLoadedPath = path;
                gFirstTimeSaving = true;
                game_fix_save_vars();
                AutoCreateMapAnimations();
                EntityTweener::Get().Reset();
                gScreenAge = 0;
                gLastAutoSaveUpdate = AUTOSAVE_PAUSE;

#ifndef DISABLE_NETWORK
                bool sendMap = false;
#endif
                if (!asScenario && (info.Type == FILE_TYPE::PARK || info.Type == FILE_TYPE::SAVED_GAME))
                {
#ifndef DISABLE_NETWORK
                    if (_network.GetMode() == NETWORK_MODE_CLIENT)
                    {
                        _network.Close();
                    }
#endif
                    game_load_init();
#ifndef DISABLE_NETWORK
                    if (_network.GetMode() == NETWORK_MODE_SERVER)
                    {
                        sendMap = true;
                    }
#endif
                }
                else
                {
                    scenario_begin();
#ifndef DISABLE_NETWORK
                    if (_network.GetMode() == NETWORK_MODE_SERVER)
                    {
                        sendMap = true;
                    }
                    if (_network.GetMode() == NETWORK_MODE_CLIENT)
                    {
                        _network.Close();
                    }
#endif
                }
                // This ensures that the newly loaded save reflects the user's
                // 'show real names of guests' option, now that it's a global setting
                peep_update_names(gConfigGeneral.show_real_names_of_guests);
#ifndef DISABLE_NETWORK
                if (sendMap)
                {
                    _network.Server_Send_MAP();
                }
#endif

#ifdef USE_BREAKPAD
                if (_network.GetMode() == NETWORK_MODE_NONE)
                {
                    start_silent_record();
                }
#endif
                return true;
            }
            catch (const ObjectLoadException& e)
            {
                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    title_load();
                }
                // The path needs to be duplicated as it's a const here
                // which the window function doesn't like
                auto intent = Intent(WC_OBJECT_LOAD_ERROR);
                intent.putExtra(INTENT_EXTRA_PATH, path);
                intent.putExtra(INTENT_EXTRA_LIST, const_cast<ObjectEntryDescriptor*>(e.MissingObjects.data()));
                intent.putExtra(INTENT_EXTRA_LIST_COUNT, static_cast<uint32_t>(e.MissingObjects.size()));

                auto windowManager = _uiContext->GetWindowManager();
                windowManager->OpenIntent(&intent);
            }
            catch (const UnsupportedRCTCFlagException& e)
            {
                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    title_load();
                }
                auto windowManager = _uiContext->GetWindowManager();
                auto ft = Formatter();
                ft.Add<uint16_t>(e.Flag);
                windowManager->ShowError(STR_FAILED_TO_LOAD_IMCOMPATIBLE_RCTC_FLAG, STR_NONE, ft);
            }
            catch (const UnsupportedRideTypeException&)
            {
                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    title_load();
                }
                auto windowManager = _uiContext->GetWindowManager();
                windowManager->ShowError(STR_FILE_CONTAINS_UNSUPPORTED_RIDE_TYPES, STR_NONE, {});
            }
            catch (const std::exception& e)
            {
                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    title_load();
                }
                Console::Error::WriteLine(e.what());
            }

            return false;
        }

    private:
        std::string GetOrPromptRCT2Path()
        {
            auto result = std::string();
            if (gCustomRCT2DataPath.empty())
            {
                // Check install directory
                if (gConfigGeneral.rct2_path.empty() || !Platform::OriginalGameDataExists(gConfigGeneral.rct2_path))
                {
                    log_verbose(
                        "install directory does not exist or invalid directory selected, %s", gConfigGeneral.rct2_path.c_str());
                    if (!config_find_or_browse_install_directory())
                    {
                        auto path = config_get_default_path();
                        Console::Error::WriteLine(
                            "An RCT2 install directory must be specified! Please edit \"game_path\" in %s.\n", path.c_str());
                        return std::string();
                    }
                }
                result = gConfigGeneral.rct2_path;
            }
            else
            {
                result = gCustomRCT2DataPath;
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
            if (!_versionCheckFuture.valid())
            {
                _versionCheckFuture = std::async(std::launch::async, [this] {
                    _newVersionInfo = get_latest_version();
                    if (!String::StartsWith(gVersionInfoTag, _newVersionInfo.tag))
                    {
                        _hasNewVersionInfo = true;
                    }
                });
            }

            gIntroState = IntroState::None;
            if (gOpenRCT2Headless)
            {
                // NONE or OPEN are the only allowed actions for headless mode
                if (gOpenRCT2StartupAction != StartupAction::Open)
                {
                    gOpenRCT2StartupAction = StartupAction::None;
                }
            }
            else
            {
                if ((gOpenRCT2StartupAction == StartupAction::Title) && gConfigGeneral.play_intro)
                {
                    gOpenRCT2StartupAction = StartupAction::Intro;
                }
            }

            switch (gOpenRCT2StartupAction)
            {
                case StartupAction::Intro:
                    gIntroState = IntroState::PublisherBegin;
                    title_load();
                    break;
                case StartupAction::Title:
                    title_load();
                    break;
                case StartupAction::Open:
                {
                    // A path that includes "://" is illegal with all common filesystems, so it is almost certainly a URL
                    // This way all cURL supported protocols, like http, ftp, scp and smb are automatically handled
                    if (strstr(gOpenRCT2StartupActionPath, "://") != nullptr)
                    {
#ifndef DISABLE_HTTP
                        // Download park and open it using its temporary filename
                        auto data = DownloadPark(gOpenRCT2StartupActionPath);
                        if (data.empty())
                        {
                            title_load();
                            break;
                        }

                        auto ms = MemoryStream(data.data(), data.size(), MEMORY_ACCESS::READ);
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
                        catch (const std::exception& ex)
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

                        if (gNetworkStartAddress.empty())
                        {
                            gNetworkStartAddress = gConfigNetwork.listen_address;
                        }

                        if (gCustomPassword.empty())
                        {
                            _network.SetPassword(gConfigNetwork.default_password.c_str());
                        }
                        else
                        {
                            _network.SetPassword(gCustomPassword);
                        }
                        _network.BeginServer(gNetworkStartPort, gNetworkStartAddress);
                    }
                    else
#endif // DISABLE_NETWORK
                    {
                        game_load_scripts();
                        game_notify_map_changed();
                    }
                    break;
                }
                case StartupAction::Edit:
                    if (String::SizeOf(gOpenRCT2StartupActionPath) == 0)
                    {
                        Editor::Load();
                    }
                    else if (!Editor::LoadLandscape(gOpenRCT2StartupActionPath))
                    {
                        title_load();
                    }
                    break;
                default:
                    break;
            }

#ifndef DISABLE_NETWORK
            if (gNetworkStart == NETWORK_MODE_CLIENT)
            {
                if (gNetworkStartPort == 0)
                {
                    gNetworkStartPort = gConfigNetwork.default_port;
                }
                _network.BeginClient(gNetworkStartHost, gNetworkStartPort);
            }
#endif // DISABLE_NETWORK

            _stdInOutConsole.Start();
            RunGameLoop();
        }

        bool ShouldDraw()
        {
            if (gOpenRCT2Headless)
                return false;
            if (_uiContext->IsMinimised())
                return false;
            return true;
        }

        bool ShouldRunVariableFrame()
        {
            if (!ShouldDraw())
                return false;
            if (!gConfigGeneral.uncap_fps)
                return false;
            if (gGameSpeed > 4)
                return false;
            return true;
        }

        /**
         * Run the main game loop until the finished flag is set.
         */
        void RunGameLoop()
        {
            PROFILED_FUNCTION();

            log_verbose("begin openrct2 loop");
            _finished = false;

#ifndef __EMSCRIPTEN__
            _variableFrame = ShouldRunVariableFrame();
            do
            {
                RunFrame();
            } while (!_finished);
#else
            emscripten_set_main_loop_arg(
                [](void* vctx) -> {
                    auto ctx = reinterpret_cast<Context*>(vctx);
                    ctx->RunFrame();
                },
                this, 0, 1);
#endif // __EMSCRIPTEN__
            log_verbose("finish openrct2 loop");
        }

        void RunFrame()
        {
            PROFILED_FUNCTION();

            const auto deltaTime = _timer.GetElapsedTimeAndRestart().count();

            // Make sure we catch the state change and reset it.
            bool useVariableFrame = ShouldRunVariableFrame();
            if (_variableFrame != useVariableFrame)
            {
                _variableFrame = useVariableFrame;

                // Switching from variable to fixed frame requires reseting
                // of entity positions back to end of tick positions
                auto& tweener = EntityTweener::Get();
                tweener.Restore();
                tweener.Reset();
            }

            UpdateTimeAccumulators(deltaTime);

            if (useVariableFrame)
            {
                RunVariableFrame(deltaTime);
            }
            else
            {
                RunFixedFrame(deltaTime);
            }
        }

        void UpdateTimeAccumulators(float deltaTime)
        {
            // Ticks
            float scaledDeltaTime = deltaTime * _timeScale;
            _ticksAccumulator = std::min(_ticksAccumulator + scaledDeltaTime, GAME_UPDATE_MAX_THRESHOLD);

            // Real Time.
            _realtimeAccumulator = std::min(_realtimeAccumulator + deltaTime, GAME_UPDATE_MAX_THRESHOLD);
            while (_realtimeAccumulator >= GAME_UPDATE_TIME_MS)
            {
                gCurrentRealTimeTicks++;
                _realtimeAccumulator -= GAME_UPDATE_TIME_MS;
            }
        }

        void RunFixedFrame(float deltaTime)
        {
            PROFILED_FUNCTION();

            _uiContext->ProcessMessages();

            if (_ticksAccumulator < GAME_UPDATE_TIME_MS)
            {
                const auto sleepTimeSec = (GAME_UPDATE_TIME_MS - _ticksAccumulator);
                Platform::Sleep(static_cast<uint32_t>(sleepTimeSec * 1000.f));
                return;
            }

            while (_ticksAccumulator >= GAME_UPDATE_TIME_MS)
            {
                Tick();

                // Always run this at a fixed rate, Update can cause multiple ticks if the game is speed up.
                window_update_all();

                _ticksAccumulator -= GAME_UPDATE_TIME_MS;
            }

            if (ShouldDraw())
            {
                Draw();
            }
        }

        void RunVariableFrame(float deltaTime)
        {
            PROFILED_FUNCTION();

            const bool shouldDraw = ShouldDraw();
            auto& tweener = EntityTweener::Get();

            _uiContext->ProcessMessages();

            while (_ticksAccumulator >= GAME_UPDATE_TIME_MS)
            {
                // Get the original position of each sprite
                if (shouldDraw)
                    tweener.PreTick();

                Tick();

                // Always run this at a fixed rate, Update can cause multiple ticks if the game is speed up.
                window_update_all();

                _ticksAccumulator -= GAME_UPDATE_TIME_MS;

                // Get the next position of each sprite
                if (shouldDraw)
                    tweener.PostTick();
            }

            if (shouldDraw)
            {
                const float alpha = std::min(_ticksAccumulator / GAME_UPDATE_TIME_MS, 1.0f);
                tweener.Tween(alpha);

                Draw();
            }
        }

        void Draw()
        {
            PROFILED_FUNCTION();

            _drawingEngine->BeginDraw();
            _painter->Paint(*_drawingEngine);
            _drawingEngine->EndDraw();
        }

        void Tick()
        {
            PROFILED_FUNCTION();

            // TODO: This variable has been never "variable" in time, some code expects
            // this to be 40Hz (25 ms). Refactor this once the UI is decoupled.
            gCurrentDeltaTime = static_cast<uint32_t>(GAME_UPDATE_TIME_MS * 1000.0f);

            if (game_is_not_paused())
            {
                gPaletteEffectFrame += gCurrentDeltaTime;
            }

            date_update_real_time_of_day();

            if (gIntroState != IntroState::None)
            {
                intro_update();
            }
            else if ((gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !gOpenRCT2Headless)
            {
                _titleScreen->Tick();
            }
            else
            {
                _gameState->Tick();
            }

#ifdef __ENABLE_DISCORD__
            if (_discordService != nullptr)
            {
                _discordService->Tick();
            }
#endif

            chat_update();
#ifdef ENABLE_SCRIPTING
            _scriptEngine.Tick();
#endif
            _stdInOutConsole.ProcessEvalQueue();
            _uiContext->Tick();
        }

        /**
         * Ensure that the custom user content folders are present
         */
        void EnsureUserContentDirectoriesExist()
        {
            EnsureDirectoriesExist(
                DIRBASE::USER,
                {
                    DIRID::OBJECT,
                    DIRID::SAVE,
                    DIRID::SCENARIO,
                    DIRID::TRACK,
                    DIRID::LANDSCAPE,
                    DIRID::HEIGHTMAP,
                    DIRID::PLUGIN,
                    DIRID::THEME,
                    DIRID::SEQUENCE,
                    DIRID::REPLAY,
                    DIRID::LOG_DESYNCS,
                    DIRID::CRASH,
                });
        }

        void EnsureDirectoriesExist(const DIRBASE dirBase, const std::initializer_list<DIRID>& dirIds)
        {
            for (const auto& dirId : dirIds)
            {
                auto path = _env->GetDirectoryPath(dirBase, dirId);
                if (!Platform::EnsureDirectoryExists(path.c_str()))
                    log_error("Unable to create directory '%s'.", path.c_str());
            }
        }

        /**
         * Copy saved games and landscapes to user directory
         */
        void CopyOriginalUserFilesOver()
        {
            CopyOriginalUserFilesOver(DIRID::SAVE, "*.sv6");
            CopyOriginalUserFilesOver(DIRID::LANDSCAPE, "*.sc6");
        }

        void CopyOriginalUserFilesOver(DIRID dirid, const std::string& pattern)
        {
            auto src = _env->GetDirectoryPath(DIRBASE::RCT2, dirid);
            auto dst = _env->GetDirectoryPath(DIRBASE::USER, dirid);
            CopyOriginalUserFilesOver(src, dst, pattern);
        }

        void CopyOriginalUserFilesOver(const std::string& srcRoot, const std::string& dstRoot, const std::string& pattern)
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
                if (!Path::DirectoryExists(dstDirectory.c_str()))
                {
                    Console::WriteLine("Creating directory '%s'", dstDirectory.c_str());
                    if (!Platform::EnsureDirectoryExists(dstDirectory.c_str()))
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
        }

#ifndef DISABLE_HTTP
        std::vector<uint8_t> DownloadPark(const std::string& url)
        {
            // Download park to buffer in memory
            Http::Request request;
            request.url = url;
            request.method = Http::Method::GET;

            Http::Response res;
            try
            {
                res = Do(request);
                if (res.status != Http::Status::Ok)
                    throw std::runtime_error("bad http status");
            }
            catch (std::exception& e)
            {
                Console::Error::WriteLine("Failed to download '%s', cause %s", request.url.c_str(), e.what());
                return {};
            }

            std::vector<uint8_t> parkData;
            parkData.resize(res.body.size());
            std::memcpy(parkData.data(), res.body.c_str(), parkData.size());
            return parkData;
        }
#endif

        bool HasNewVersionInfo() const override
        {
            return _hasNewVersionInfo;
        }

        const NewVersionInfo* GetNewVersionInfo() const override
        {
            return &_newVersionInfo;
        }

        void SetTimeScale(float newScale) override
        {
            _timeScale = std::clamp(newScale, GAME_MIN_TIME_SCALE, GAME_MAX_TIME_SCALE);
        }

        float GetTimeScale() const override
        {
            return _timeScale;
        }
    };

    Context* Context::Instance = nullptr;

    std::unique_ptr<IContext> CreateContext()
    {
        return CreateContext(CreatePlatformEnvironment(), CreateDummyAudioContext(), CreateDummyUiContext());
    }

    std::unique_ptr<IContext> CreateContext(
        const std::shared_ptr<IPlatformEnvironment>& env, const std::shared_ptr<Audio::IAudioContext>& audioContext,
        const std::shared_ptr<IUiContext>& uiContext)
    {
        return std::make_unique<Context>(env, audioContext, uiContext);
    }

    IContext* GetContext()
    {
        return Context::Instance;
    }

} // namespace OpenRCT2

void context_init()
{
    GetContext()->GetUiContext()->GetWindowManager()->Init();
}

bool context_load_park_from_file(const utf8* path)
{
    return GetContext()->LoadParkFromFile(path);
}

bool context_load_park_from_stream(void* stream)
{
    return GetContext()->LoadParkFromStream(static_cast<IStream*>(stream), "");
}

void openrct2_write_full_version_info(utf8* buffer, size_t bufferSize)
{
    String::Set(buffer, bufferSize, gVersionInfoFull);
}

void openrct2_finish()
{
    GetContext()->Finish();
}

void context_setcurrentcursor(CursorID cursor)
{
    GetContext()->GetUiContext()->SetCursor(cursor);
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

ScreenCoordsXY context_get_cursor_position()
{
    return GetContext()->GetUiContext()->GetCursorPosition();
}

ScreenCoordsXY context_get_cursor_position_scaled()
{
    auto cursorCoords = context_get_cursor_position();
    // Compensate for window scaling.
    return { static_cast<int32_t>(std::ceil(cursorCoords.x / gConfigGeneral.window_scale)),
             static_cast<int32_t>(std::ceil(cursorCoords.y / gConfigGeneral.window_scale)) };
}

void context_set_cursor_position(const ScreenCoordsXY& cursorPosition)
{
    GetContext()->GetUiContext()->SetCursorPosition(cursorPosition);
}

const CursorState* context_get_cursor_state()
{
    return GetContext()->GetUiContext()->GetCursorState();
}

const uint8_t* context_get_keys_state()
{
    return GetContext()->GetUiContext()->GetKeysState();
}

const uint8_t* context_get_keys_pressed()
{
    return GetContext()->GetUiContext()->GetKeysPressed();
}

TextInputSession* context_start_text_input(utf8* buffer, size_t maxLength)
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
    return GetContext()->GetUiContext()->SetFullscreenMode(static_cast<FULLSCREEN_MODE>(mode));
}

void context_recreate_window()
{
    GetContext()->GetUiContext()->RecreateWindow();
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

rct_window* context_open_window(rct_windowclass wc)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenWindow(wc);
}

rct_window* context_open_window_view(rct_windowclass wc)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenView(wc);
}

rct_window* context_open_detail_window(uint8_t type, int32_t id)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenDetails(type, id);
}

rct_window* context_open_intent(Intent* intent)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->OpenIntent(intent);
}

void context_broadcast_intent(Intent* intent)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->BroadcastIntent(*intent);
}

void context_force_close_window_by_class(rct_windowclass windowClass)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    windowManager->ForceClose(windowClass);
}

rct_window* context_show_error(rct_string_id title, rct_string_id message, const Formatter& args)
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    return windowManager->ShowError(title, message, args);
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

const utf8* context_get_path_legacy(int32_t pathId)
{
    static utf8 result[MAX_PATH];
    auto path = GetContext()->GetPathLegacy(pathId);
    String::Set(result, sizeof(result), path.c_str());
    return result;
}

bool ContextOpenCommonFileDialog(utf8* outFilename, OpenRCT2::Ui::FileDialogDesc& desc, size_t outSize)
{
    try
    {
        std::string result = GetContext()->GetUiContext()->ShowFileDialog(desc);
        String::Set(outFilename, outSize, result.c_str());
        return !result.empty();
    }
    catch (const std::exception& ex)
    {
        log_error(ex.what());
        outFilename[0] = '\0';
        return false;
    }
}

u8string ContextOpenCommonFileDialog(OpenRCT2::Ui::FileDialogDesc& desc)
{
    try
    {
        return GetContext()->GetUiContext()->ShowFileDialog(desc);
    }
    catch (const std::exception& ex)
    {
        log_error(ex.what());
        return u8string{};
    }
}
