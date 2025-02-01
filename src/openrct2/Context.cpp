/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __EMSCRIPTEN__
    #include <cassert>
    #include <emscripten.h>
#endif // __EMSCRIPTEN__

#include "AssetPackManager.h"
#include "Context.h"
#include "Editor.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "GameStateSnapshots.h"
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
#include "interface/StdInOutConsole.h"
#include "interface/Viewport.h"
#include "localisation/Formatter.h"
#include "localisation/Localisation.Date.h"
#include "localisation/LocalisationService.h"
#include "network/DiscordService.h"
#include "network/NetworkBase.h"
#include "network/network.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "paint/Painter.h"
#include "park/ParkFile.h"
#include "platform/Crash.h"
#include "platform/Platform.h"
#include "profiling/Profiling.h"
#include "rct2/RCT2.h"
#include "ride/TrackData.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/Scenario.h"
#include "scenario/ScenarioRepository.h"
#include "scenes/game/GameScene.h"
#include "scenes/intro/IntroScene.h"
#include "scenes/preloader/PreloaderScene.h"
#include "scenes/title/TitleScene.h"
#include "scenes/title/TitleSequenceManager.h"
#include "scripting/HookEngine.h"
#include "scripting/ScriptEngine.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "world/Park.h"

#include <chrono>
#include <cmath>
#include <exception>
#include <future>
#include <iterator>
#include <memory>
#include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Localisation;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Scripting;
using namespace OpenRCT2::Ui;

using OpenRCT2::Audio::IAudioContext;

namespace OpenRCT2
{
    namespace
    {
        using namespace std::chrono_literals;

        static constexpr auto kForcedUpdateInterval = 25ms;
    } // namespace

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
        std::unique_ptr<AssetPackManager> _assetPackManager;
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

        // Scenes
        std::unique_ptr<PreloaderScene> _preloaderScene;
        std::unique_ptr<IntroScene> _introScene;
        std::unique_ptr<TitleScene> _titleScene;
        std::unique_ptr<GameScene> _gameScene;
        IScene* _activeScene = nullptr;

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

        // We keep track of this to perform certain operations differently.
        std::thread::id _mainThreadId{};
        Timer _forcedUpdateTimer;

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
            , _objectRepository(CreateObjectRepository(_env))
            , _objectManager(CreateObjectManager(*_objectRepository))
            , _trackDesignRepository(CreateTrackDesignRepository(_env))
            , _scenarioRepository(CreateScenarioRepository(_env))
            , _replayManager(CreateReplayManager())
            , _gameStateSnapshots(CreateGameStateSnapshots())
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
            _mainThreadId = std::this_thread::get_id();
        }

        ~Context() override
        {
            // NOTE: We must shutdown all systems here before Instance is set back to null.
            //       If objects use GetContext() in their destructor things won't go well.

#ifdef ENABLE_SCRIPTING
            _scriptEngine.StopUnloadRegisterAllPlugins();
#endif

            GameActions::ClearQueue();
            _replayManager->StopRecording(true);
#ifndef DISABLE_NETWORK
            _network.Close();
#endif

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAll();

            // Unload objects after closing all windows, this is to overcome windows like
            // the object selection window which loads objects when closed.
            if (_objectManager != nullptr)
            {
                _objectManager->UnloadAll();
            }

            GfxObjectCheckAllImagesFreed();
            GfxUnloadCsg();
            GfxUnloadG2();
            GfxUnloadG1();
            Audio::Close();

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

        AssetPackManager* GetAssetPackManager() override
        {
            return _assetPackManager.get();
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

        IScene* GetPreloaderScene() override
        {
            if (auto* scene = _preloaderScene.get())
                return scene;

            _preloaderScene = std::make_unique<PreloaderScene>(*this);
            return _preloaderScene.get();
        }

        IScene* GetIntroScene() override
        {
            if (auto* scene = _introScene.get())
                return scene;

            _introScene = std::make_unique<IntroScene>(*this);
            return _introScene.get();
        }

        IScene* GetTitleScene() override
        {
            if (auto* scene = _titleScene.get())
                return scene;

            _titleScene = std::make_unique<TitleScene>(*this);
            return _titleScene.get();
        }

        IScene* GetGameScene() override
        {
            if (auto* scene = _gameScene.get())
                return scene;

            _gameScene = std::make_unique<GameScene>(*this);
            return _gameScene.get();
        }

        IScene* GetEditorScene() override
        {
            // TODO: Implement me.
            return nullptr;
        }

        IScene* GetActiveScene() override
        {
            return _activeScene;
        }

        void SetActiveScene(IScene* screen) override
        {
            if (_activeScene != nullptr)
                _activeScene->Stop();
            _activeScene = screen;
            if (_activeScene)
                _activeScene->Load();
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
            ContextOpenWindow(WindowClass::SavePrompt);
        }

        bool Initialise() final override
        {
            if (_initialised)
            {
                throw std::runtime_error("Context already initialised.");
            }
            _initialised = true;

            CrashInit();

            if (String::equals(Config::Get().general.LastRunVersion, kOpenRCT2Version))
            {
                gOpenRCT2ShowChangelog = false;
            }
            else
            {
                gOpenRCT2ShowChangelog = true;
                Config::Get().general.LastRunVersion = kOpenRCT2Version;
                Config::Save();
            }

            try
            {
                _localisationService->OpenLanguage(Config::Get().general.Language);
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Failed to open configured language: %s", e.what());
                try
                {
                    _localisationService->OpenLanguage(LANGUAGE_ENGLISH_UK);
                }
                catch (const std::exception& eFallback)
                {
                    LOG_FATAL("Failed to open fallback language: %s", eFallback.what());
                    auto uiContext = GetContext()->GetUiContext();
#ifdef __ANDROID__
                    uiContext->ShowMessageBox(
                        "You need to copy some additional files to finish your install.\n\nSee "
                        "https://docs.openrct2.io/en/latest/installing/installing-on-android.html for more details.");
#else
                    uiContext->ShowMessageBox("Failed to load language file!\nYour installation may be damaged.");
#endif
                    return false;
                }
            }

            // TODO add configuration option to allow multiple instances
            // if (!gOpenRCT2Headless && !Platform::LockSingleInstance()) {
            //  LOG_FATAL("OpenRCT2 is already running.");
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

            if (!gOpenRCT2Headless)
            {
                _assetPackManager = std::make_unique<AssetPackManager>();
            }
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

            if (!gOpenRCT2Headless)
            {
                Audio::Init();
                Audio::PopulateDevices();
                Audio::InitRideSoundsAndInfo();
                Audio::gGameSoundsOff = !Config::Get().sound.MasterSoundEnabled;
            }

            ChatInit();
            CopyOriginalUserFilesOver();

            if (!gOpenRCT2NoGraphics)
            {
                if (!LoadBaseGraphics())
                {
                    return false;
                }
                Drawing::LightFx::Init();
            }

            ViewportInitAll();

            ContextInit();

            if (!gOpenRCT2Headless)
            {
                auto* preloaderScene = static_cast<PreloaderScene*>(GetPreloaderScene());
                SetActiveScene(preloaderScene);

                // TODO: preload the title scene in another (parallel) job.
                preloaderScene->AddJob([this]() { InitialiseRepositories(); });
                preloaderScene->AddJob([this]() { InitialiseScriptEngine(); });
            }
            else
            {
                InitialiseRepositories();
                InitialiseScriptEngine();
            }

            return true;
        }

    private:
        void InitialiseRepositories()
        {
            if (!_initialised)
            {
                throw std::runtime_error("Context needs to be initialised first.");
            }

            auto currentLanguage = _localisationService->GetCurrentLanguage();

            OpenProgress(STR_CHECKING_OBJECT_FILES);
            _objectRepository->LoadOrConstruct(currentLanguage);

            OpenProgress(STR_LOADING_GENERIC);
            Audio::LoadAudioObjects();

            if (!gOpenRCT2Headless)
            {
                OpenProgress(STR_CHECKING_ASSET_PACKS);
                _assetPackManager->Scan();
                _assetPackManager->LoadEnabledAssetPacks();
                _assetPackManager->Reload();
            }

            OpenProgress(STR_CHECKING_TRACK_DESIGN_FILES);
            _trackDesignRepository->Scan(currentLanguage);

            OpenProgress(STR_CHECKING_SCENARIO_FILES);
            _scenarioRepository->Scan(currentLanguage);

            OpenProgress(STR_CHECKING_TITLE_SEQUENCES);
            TitleSequenceManager::Scan();

            OpenProgress(STR_LOADING_GENERIC);
        }

        void InitialiseScriptEngine()
        {
#ifdef ENABLE_SCRIPTING
            OpenProgress(STR_LOADING_PLUGIN_ENGINE);
            _scriptEngine.Initialise();
            _uiContext->InitialiseScriptExtensions();

            OpenProgress(STR_LOADING_GENERIC);
#endif
        }

    public:
        void InitialiseDrawingEngine() final override
        {
            assert(_drawingEngine == nullptr);

            _drawingEngineType = Config::Get().general.DrawingEngine;

            auto drawingEngineFactory = _uiContext->GetDrawingEngineFactory();
            auto drawingEngine = drawingEngineFactory->Create(_drawingEngineType, _uiContext);

            if (drawingEngine == nullptr)
            {
                if (_drawingEngineType == DrawingEngine::Software)
                {
                    _drawingEngineType = DrawingEngine::None;
                    LOG_FATAL("Unable to create a drawing engine.");
                    exit(-1);
                }
                else
                {
                    LOG_ERROR("Unable to create drawing engine. Falling back to software.");

                    // Fallback to software
                    Config::Get().general.DrawingEngine = DrawingEngine::Software;
                    Config::Save();
                    DrawingEngineInit();
                }
            }
            else
            {
                try
                {
                    drawingEngine->Initialise();
                    drawingEngine->SetVSync(Config::Get().general.UseVSync);
                    _drawingEngine = std::move(drawingEngine);
                }
                catch (const std::exception& ex)
                {
                    if (_drawingEngineType == DrawingEngine::Software)
                    {
                        _drawingEngineType = DrawingEngine::None;
                        LOG_ERROR(ex.what());
                        LOG_FATAL("Unable to initialise a drawing engine.");
                        exit(-1);
                    }
                    else
                    {
                        LOG_ERROR(ex.what());
                        LOG_ERROR("Unable to initialise drawing engine. Falling back to software.");

                        // Fallback to software
                        Config::Get().general.DrawingEngine = DrawingEngine::Software;
                        Config::Save();
                        DrawingEngineInit();
                    }
                }
            }

            WindowCheckAllValidZoom();
        }

        void DisposeDrawingEngine() final override
        {
            _drawingEngine = nullptr;
        }

        void OpenProgress(StringId captionStringId) override
        {
            auto captionString = _localisationService->GetString(captionStringId);
            auto intent = Intent(INTENT_ACTION_PROGRESS_OPEN);
            intent.PutExtra(INTENT_EXTRA_MESSAGE, captionString);
            ContextOpenIntent(&intent);
        }

        void SetProgress(uint32_t currentProgress, uint32_t totalCount, StringId format = kStringIdNone) override
        {
            if (_forcedUpdateTimer.GetElapsedTime() < kForcedUpdateInterval)
                return;

            _forcedUpdateTimer.Restart();

            auto intent = Intent(INTENT_ACTION_PROGRESS_SET);
            intent.PutExtra(INTENT_EXTRA_PROGRESS_OFFSET, currentProgress);
            intent.PutExtra(INTENT_EXTRA_PROGRESS_TOTAL, totalCount);
            intent.PutExtra(INTENT_EXTRA_STRING_ID, format);
            ContextOpenIntent(&intent);

            // When we call this from the main thread we can pump messages and redraw.
            const auto isMainThread = _mainThreadId == std::this_thread::get_id();

            if (!gOpenRCT2Headless && isMainThread)
            {
                _uiContext->ProcessMessages();
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByClass(WindowClass::ProgressWindow);
                Draw();
            }
        }

        void CloseProgress() override
        {
            auto intent = Intent(INTENT_ACTION_PROGRESS_CLOSE);
            ContextOpenIntent(&intent);
        }

        bool LoadParkFromFile(const u8string& path, bool loadTitleScreenOnFail = false, bool asScenario = false) final override
        {
            LOG_VERBOSE("Context::LoadParkFromFile(%s)", path.c_str());

            struct CrashAdditionalFileRegistration
            {
                CrashAdditionalFileRegistration(const std::string& path)
                {
                    // Register the file for crash upload if it asserts while loading.
                    CrashRegisterAdditionalFile("load_park", path);
                }
                ~CrashAdditionalFileRegistration()
                {
                    // Deregister park file in case it was processed without hitting an assert.
                    CrashUnregisterAdditionalFile("load_park");
                }
            } crash_additional_file_registration(path);

            try
            {
                if (String::iequals(Path::GetExtension(path), ".sea"))
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
                    SetActiveScene(GetTitleScene());
                }
                auto windowManager = _uiContext->GetWindowManager();
                windowManager->ShowError(STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, kStringIdNone, {});
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
                else if (info.Version <= kFileTypeS4Cutoff)
                {
                    // Save is an S4 (RCT1 format)
                    parkImporter = ParkImporter::CreateS4();
                }
                else
                {
                    // Save is an S6 (RCT2 format)
                    parkImporter = ParkImporter::CreateS6(*_objectRepository);
                }

                // Inhibit viewport rendering while we're loading
                WindowSetFlagForAllViewports(VIEWPORT_FLAG_RENDERING_INHIBITED, true);

                OpenProgress(asScenario ? STR_LOADING_SCENARIO : STR_LOADING_SAVED_GAME);
                SetProgress(0, 100, STR_STRING_M_PERCENT);

                auto result = parkImporter->LoadFromStream(stream, info.Type == FILE_TYPE::SCENARIO, false, path.c_str());
                SetProgress(10, 100, STR_STRING_M_PERCENT);

                // From this point onwards the currently loaded park will be corrupted if loading fails
                // so reload the title screen if that happens.
                loadTitleScreenFirstOnFail = true;

                GameUnloadScripts();
                _objectManager->LoadObjects(result.RequiredObjects, true);
                SetProgress(90, 100, STR_STRING_M_PERCENT);

                // TODO: Have a separate GameState and exchange once loaded.
                auto& gameState = ::GetGameState();
                parkImporter->Import(gameState);
                SetProgress(100, 100, STR_STRING_M_PERCENT);

                // Reset viewport rendering inhibition
                WindowSetFlagForAllViewports(VIEWPORT_FLAG_RENDERING_INHIBITED, false);

                gScenarioSavePath = path;
                gCurrentLoadedPath = path;
                gFirstTimeSaving = true;
                GameFixSaveVars();
                MapAnimationAutoCreate();
                EntityTweener::Get().Reset();
                gScreenAge = 0;
                gLastAutoSaveUpdate = kAutosavePause;

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
                    GameLoadInit();
#ifndef DISABLE_NETWORK
                    if (_network.GetMode() == NETWORK_MODE_SERVER)
                    {
                        sendMap = true;
                    }
#endif
                }
                else
                {
                    ScenarioBegin(gameState);
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
                PeepUpdateNames();
#ifndef DISABLE_NETWORK
                if (sendMap)
                {
                    _network.ServerSendMap();
                }
#endif

#ifdef USE_BREAKPAD
                if (_network.GetMode() == NETWORK_MODE_NONE)
                {
                    StartSilentRecord();
                }
#endif
                if (result.SemiCompatibleVersion)
                {
                    auto windowManager = _uiContext->GetWindowManager();
                    auto ft = Formatter();
                    ft.Add<uint32_t>(result.TargetVersion);
                    ft.Add<uint32_t>(OpenRCT2::kParkFileCurrentVersion);
                    windowManager->ShowError(STR_WARNING_PARK_VERSION_TITLE, STR_WARNING_PARK_VERSION_MESSAGE, ft);
                }
                else if (HasObjectsThatUseFallbackImages())
                {
                    Console::Error::WriteLine("Park has objects which require RCT1 linked. Fallback images will be used.");
                    auto windowManager = _uiContext->GetWindowManager();
                    windowManager->ShowError(STR_PARK_USES_FALLBACK_IMAGES_WARNING, kStringIdEmpty, Formatter());
                }

                CloseProgress();
                return true;
            }
            catch (const ObjectLoadException& e)
            {
                Console::Error::WriteLine("Unable to open park: missing objects");

                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    SetActiveScene(GetTitleScene());
                }
                // The path needs to be duplicated as it's a const here
                // which the window function doesn't like
                auto intent = Intent(WindowClass::ObjectLoadError);
                intent.PutExtra(INTENT_EXTRA_PATH, path);
                intent.PutExtra(INTENT_EXTRA_LIST, const_cast<ObjectEntryDescriptor*>(e.MissingObjects.data()));
                intent.PutExtra(INTENT_EXTRA_LIST_COUNT, static_cast<uint32_t>(e.MissingObjects.size()));

                auto windowManager = _uiContext->GetWindowManager();
                windowManager->OpenIntent(&intent);
            }
            catch (const UnsupportedRideTypeException&)
            {
                Console::Error::WriteLine("Unable to open park: unsupported ride types");

                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    SetActiveScene(GetTitleScene());
                }
                auto windowManager = _uiContext->GetWindowManager();
                windowManager->ShowError(STR_FILE_CONTAINS_UNSUPPORTED_RIDE_TYPES, kStringIdNone, {});
            }
            catch (const UnsupportedVersionException& e)
            {
                Console::Error::WriteLine("Unable to open park: unsupported park version");

                if (loadTitleScreenFirstOnFail)
                {
                    SetActiveScene(GetTitleScene());
                }
                auto windowManager = _uiContext->GetWindowManager();
                Formatter ft;
                /*if (e.TargetVersion < kParkFileMinSupportedVersion)
                {
                    ft.Add<uint32_t>(e.TargetVersion);
                    windowManager->ShowError(STR_ERROR_PARK_VERSION_TITLE, STR_ERROR_PARK_VERSION_TOO_OLD_MESSAGE, ft);
                }
                else*/
                {
                    if (e.MinVersion == e.TargetVersion)
                    {
                        ft.Add<uint32_t>(e.TargetVersion);
                        ft.Add<uint32_t>(OpenRCT2::kParkFileCurrentVersion);
                        windowManager->ShowError(STR_ERROR_PARK_VERSION_TITLE, STR_ERROR_PARK_VERSION_TOO_NEW_MESSAGE_2, ft);
                    }
                    else
                    {
                        ft.Add<uint32_t>(e.TargetVersion);
                        ft.Add<uint32_t>(e.MinVersion);
                        ft.Add<uint32_t>(OpenRCT2::kParkFileCurrentVersion);
                        windowManager->ShowError(STR_ERROR_PARK_VERSION_TITLE, STR_ERROR_PARK_VERSION_TOO_NEW_MESSAGE, ft);
                    }
                }
            }
            catch (const std::exception& e)
            {
                // If loading the SV6 or SV4 failed return to the title screen if requested.
                if (loadTitleScreenFirstOnFail)
                {
                    SetActiveScene(GetTitleScene());
                }
                Console::Error::WriteLine(e.what());
            }

            CloseProgress();
            WindowSetFlagForAllViewports(VIEWPORT_FLAG_RENDERING_INHIBITED, false);
            return false;
        }

    private:
        bool HasObjectsThatUseFallbackImages()
        {
            for (auto objectType : getAllObjectTypes())
            {
                auto maxObjectsOfType = static_cast<ObjectEntryIndex>(getObjectEntryGroupCount(objectType));
                for (ObjectEntryIndex i = 0; i < maxObjectsOfType; i++)
                {
                    auto obj = _objectManager->GetLoadedObject(objectType, i);
                    if (obj != nullptr)
                    {
                        if (obj->UsesFallbackImages())
                            return true;
                    }
                }
            }
            return false;
        }

        std::string GetOrPromptRCT2Path()
        {
            auto result = std::string();
            if (gCustomRCT2DataPath.empty())
            {
                // Check install directory
                if (Config::Get().general.RCT2Path.empty() || !Platform::OriginalGameDataExists(Config::Get().general.RCT2Path))
                {
                    LOG_VERBOSE(
                        "install directory does not exist or invalid directory selected, %s",
                        Config::Get().general.RCT2Path.c_str());
                    if (!Config::FindOrBrowseInstallDirectory())
                    {
                        auto path = Config::GetDefaultPath();
                        Console::Error::WriteLine(
                            "An RCT2 install directory must be specified! Please edit \"game_path\" in %s.\n", path.c_str());
                        return std::string();
                    }
                }
                result = Config::Get().general.RCT2Path;
            }
            else
            {
                result = gCustomRCT2DataPath;
            }
            return result;
        }

        // TODO: move function elsewhere?
        bool LoadBaseGraphics()
        {
            if (!GfxLoadG1(*_env))
            {
                return false;
            }
            GfxLoadG2();
            GfxLoadCsg();
            FontSpriteInitialiseCharacters();
            return true;
        }

        void SwitchToStartUpScene()
        {
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
                if ((gOpenRCT2StartupAction == StartupAction::Title) && Config::Get().general.PlayIntro)
                {
                    gOpenRCT2StartupAction = StartupAction::Intro;
                }
            }

            IScene* nextScene{};
            switch (gOpenRCT2StartupAction)
            {
                case StartupAction::Intro:
                {
                    nextScene = GetIntroScene();
                    break;
                }

                case StartupAction::Title:
                {
                    nextScene = GetTitleScene();
                    break;
                }

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
                            nextScene = GetTitleScene();
                            break;
                        }

                        auto ms = MemoryStream(data.data(), data.size(), MEMORY_ACCESS::READ);
                        if (!LoadParkFromStream(&ms, gOpenRCT2StartupActionPath, true))
                        {
                            Console::Error::WriteLine("Failed to load '%s'", gOpenRCT2StartupActionPath);
                            nextScene = GetTitleScene();
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
                                nextScene = GetTitleScene();
                                break;
                            }
                        }
                        catch (const std::exception& ex)
                        {
                            Console::Error::WriteLine("Failed to load '%s'", gOpenRCT2StartupActionPath);
                            Console::Error::WriteLine("%s", ex.what());
                            nextScene = GetTitleScene();
                            break;
                        }
                    }

                    // Successfully loaded a file
                    nextScene = GetGameScene();
                    break;
                }

                case StartupAction::Edit:
                {
                    if (String::sizeOf(gOpenRCT2StartupActionPath) == 0)
                    {
                        Editor::Load();
                        nextScene = GetGameScene();
                    }
                    else if (Editor::LoadLandscape(gOpenRCT2StartupActionPath))
                    {
                        nextScene = GetGameScene();
                    }
                    else
                    {
                        nextScene = GetTitleScene();
                    }
                    break;
                }

                default:
                {
                    nextScene = GetTitleScene();
                }
            }

            SetActiveScene(nextScene);
            InitNetworkGame(nextScene == GetGameScene());
        }

        void InitNetworkGame(bool isGameScene)
        {
            if (isGameScene)
            {
#ifndef DISABLE_NETWORK
                if (gNetworkStart == NETWORK_MODE_SERVER)
                {
                    if (gNetworkStartPort == 0)
                    {
                        gNetworkStartPort = Config::Get().network.DefaultPort;
                    }

                    if (gNetworkStartAddress.empty())
                    {
                        gNetworkStartAddress = Config::Get().network.ListenAddress;
                    }

                    if (gCustomPassword.empty())
                    {
                        _network.SetPassword(Config::Get().network.DefaultPassword.c_str());
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
                    GameLoadScripts();
                    GameNotifyMapChanged();
                }
            }

#ifndef DISABLE_NETWORK
            else if (gNetworkStart == NETWORK_MODE_CLIENT)
            {
                if (gNetworkStartPort == 0)
                {
                    gNetworkStartPort = Config::Get().network.DefaultPort;
                }
                _network.BeginClient(gNetworkStartHost, gNetworkStartPort);
            }
#endif // DISABLE_NETWORK
        }

        /**
         * Launches the game, after command line arguments have been parsed and processed.
         */
        void Launch()
        {
            if (!_versionCheckFuture.valid())
            {
                _versionCheckFuture = std::async(std::launch::async, [this] {
                    _newVersionInfo = GetLatestVersion();
                    if (!String::startsWith(gVersionInfoTag, _newVersionInfo.tag))
                    {
                        _hasNewVersionInfo = true;
                    }
                });
            }

            if (!gOpenRCT2Headless)
            {
                _preloaderScene->SetOnComplete([&]() { SwitchToStartUpScene(); });
            }
            else
            {
                SwitchToStartUpScene();
            }
#ifdef __EMSCRIPTEN__
            emscripten_set_main_loop_arg(
                [](void* vctx) {
                    auto ctx = reinterpret_cast<Context*>(vctx);
                    if (ctx->_finished)
                    {
                        emscripten_cancel_main_loop();
                    }
                    ctx->RunFrame();
                },
                this, 0, 1);
#else
            _stdInOutConsole.Start();
            RunGameLoop();
#endif
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
            if (!Config::Get().general.UncapFPS)
                return false;
            if (gGameSpeed > 4)
                return false;
            return true;
        }

        /**
         * Run the main game loop until the finished flag is set.
         */
#ifndef __EMSCRIPTEN__
        void RunGameLoop()
        {
            PROFILED_FUNCTION();

            LOG_VERBOSE("begin openrct2 loop");
            _finished = false;

            _variableFrame = ShouldRunVariableFrame();
            do
            {
                RunFrame();
            } while (!_finished);
            LOG_VERBOSE("finish openrct2 loop");
        }
#endif // __EMSCRIPTEN__

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
            _ticksAccumulator = std::min(_ticksAccumulator + scaledDeltaTime, kGameUpdateMaxThreshold);

            // Real Time.
            _realtimeAccumulator = std::min(_realtimeAccumulator + deltaTime, kGameUpdateMaxThreshold);
            while (_realtimeAccumulator >= kGameUpdateTimeMS)
            {
                gCurrentRealTimeTicks++;
                _realtimeAccumulator -= kGameUpdateTimeMS;
            }
        }

        void RunFixedFrame(float deltaTime)
        {
            PROFILED_FUNCTION();

            _uiContext->ProcessMessages();

            if (_ticksAccumulator < kGameUpdateTimeMS)
            {
                const auto sleepTimeSec = (kGameUpdateTimeMS - _ticksAccumulator);
                Platform::Sleep(static_cast<uint32_t>(sleepTimeSec * 1000.f));
                return;
            }

            while (_ticksAccumulator >= kGameUpdateTimeMS)
            {
                Tick();

                _ticksAccumulator -= kGameUpdateTimeMS;
            }

            ContextHandleInput();
            WindowUpdateAll();

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

            while (_ticksAccumulator >= kGameUpdateTimeMS)
            {
                // Get the original position of each sprite
                if (shouldDraw)
                    tweener.PreTick();

                Tick();

                _ticksAccumulator -= kGameUpdateTimeMS;

                // Get the next position of each sprite
                if (shouldDraw)
                    tweener.PostTick();
            }

            ContextHandleInput();
            WindowUpdateAll();

            if (shouldDraw)
            {
                const float alpha = std::min(_ticksAccumulator / kGameUpdateTimeMS, 1.0f);
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
            gCurrentDeltaTime = static_cast<uint16_t>(kGameUpdateTimeMS * 1000.0f);

            if (GameIsNotPaused())
            {
                gPaletteEffectFrame += gCurrentDeltaTime;
            }

            DateUpdateRealTimeOfDay();

            if (_activeScene)
                _activeScene->Tick();

#ifdef __ENABLE_DISCORD__
            if (_discordService != nullptr)
            {
                _discordService->Tick();
            }
#endif

            ChatUpdate();
#ifdef ENABLE_SCRIPTING
            if (GetActiveScene() != GetPreloaderScene())
            {
                _scriptEngine.Tick();
            }
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
                if (!Path::CreateDirectory(path))
                    LOG_ERROR("Unable to create directory '%s'.", path.c_str());
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
            LOG_VERBOSE("CopyOriginalUserFilesOver('%s', '%s', '%s')", srcRoot.c_str(), dstRoot.c_str(), pattern.c_str());

            auto scanPattern = Path::Combine(srcRoot, pattern);
            auto scanner = Path::ScanDirectory(scanPattern, true);
            while (scanner->Next())
            {
                auto src = std::string(scanner->GetPath());
                auto dst = Path::Combine(dstRoot, scanner->GetPathRelative());
                auto dstDirectory = Path::GetDirectory(dst);

                // Create the directory if necessary
                if (!Path::CreateDirectory(dstDirectory))
                {
                    Console::Error::WriteLine("Could not create directory %s.", dstDirectory.c_str());
                    break;
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
            _timeScale = std::clamp(newScale, kGameMinTimeScale, kGameMaxTimeScale);
        }

        float GetTimeScale() const override
        {
            return _timeScale;
        }
    };

    Context* Context::Instance = nullptr;

    std::unique_ptr<IContext> CreateContext()
    {
        return CreateContext(CreatePlatformEnvironment(), Audio::CreateDummyAudioContext(), CreateDummyUiContext());
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

void ContextInit()
{
    GetWindowManager()->Init();
}

bool ContextLoadParkFromStream(void* stream)
{
    return GetContext()->LoadParkFromStream(static_cast<IStream*>(stream), "");
}

void OpenRCT2Finish()
{
    GetContext()->Finish();
}

void ContextSetCurrentCursor(CursorID cursor)
{
    GetContext()->GetUiContext()->SetCursor(cursor);
}

void ContextUpdateCursorScale()
{
    GetContext()->GetUiContext()->SetCursorScale(static_cast<uint8_t>(std::round(Config::Get().general.WindowScale)));
}

void ContextHideCursor()
{
    GetContext()->GetUiContext()->SetCursorVisible(false);
}

void ContextShowCursor()
{
    GetContext()->GetUiContext()->SetCursorVisible(true);
}

ScreenCoordsXY ContextGetCursorPosition()
{
    return GetContext()->GetUiContext()->GetCursorPosition();
}

ScreenCoordsXY ContextGetCursorPositionScaled()
{
    auto cursorCoords = ContextGetCursorPosition();
    // Compensate for window scaling.
    return { static_cast<int32_t>(std::ceil(cursorCoords.x / Config::Get().general.WindowScale)),
             static_cast<int32_t>(std::ceil(cursorCoords.y / Config::Get().general.WindowScale)) };
}

void ContextSetCursorPosition(const ScreenCoordsXY& cursorPosition)
{
    GetContext()->GetUiContext()->SetCursorPosition(cursorPosition);
}

const CursorState* ContextGetCursorState()
{
    return GetContext()->GetUiContext()->GetCursorState();
}

const uint8_t* ContextGetKeysState()
{
    return GetContext()->GetUiContext()->GetKeysState();
}

const uint8_t* ContextGetKeysPressed()
{
    return GetContext()->GetUiContext()->GetKeysPressed();
}

TextInputSession* ContextStartTextInput(u8string& buffer, size_t maxLength)
{
    return GetContext()->GetUiContext()->StartTextInput(buffer, maxLength);
}

void ContextStopTextInput()
{
    GetContext()->GetUiContext()->StopTextInput();
}

bool ContextIsInputActive()
{
    return GetContext()->GetUiContext()->IsTextInputActive();
}

void ContextTriggerResize()
{
    return GetContext()->GetUiContext()->TriggerResize();
}

void ContextSetFullscreenMode(int32_t mode)
{
    return GetContext()->GetUiContext()->SetFullscreenMode(static_cast<FULLSCREEN_MODE>(mode));
}

void ContextRecreateWindow()
{
    GetContext()->GetUiContext()->RecreateWindow();
}

int32_t ContextGetWidth()
{
    return GetContext()->GetUiContext()->GetWidth();
}

int32_t ContextGetHeight()
{
    return GetContext()->GetUiContext()->GetHeight();
}

bool ContextHasFocus()
{
    return GetContext()->GetUiContext()->HasFocus();
}

void ContextSetCursorTrap(bool value)
{
    GetContext()->GetUiContext()->SetCursorTrap(value);
}

WindowBase* ContextOpenWindow(WindowClass wc)
{
    auto windowManager = Ui::GetWindowManager();
    return windowManager->OpenWindow(wc);
}

WindowBase* ContextOpenWindowView(uint8_t wc)
{
    auto windowManager = Ui::GetWindowManager();
    return windowManager->OpenView(wc);
}

WindowBase* ContextOpenDetailWindow(uint8_t type, int32_t id)
{
    auto windowManager = Ui::GetWindowManager();
    return windowManager->OpenDetails(type, id);
}

WindowBase* ContextOpenIntent(Intent* intent)
{
    auto windowManager = Ui::GetWindowManager();
    return windowManager->OpenIntent(intent);
}

void ContextBroadcastIntent(Intent* intent)
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->BroadcastIntent(*intent);
}

void ContextForceCloseWindowByClass(WindowClass windowClass)
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->ForceClose(windowClass);
}

WindowBase* ContextShowError(StringId title, StringId message, const Formatter& args, const bool autoClose /* = false */)
{
    auto windowManager = Ui::GetWindowManager();
    return windowManager->ShowError(title, message, args, autoClose);
}

void ContextHandleInput()
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->HandleInput();
}

void ContextInputHandleKeyboard(bool isTitle)
{
    auto windowManager = Ui::GetWindowManager();
    windowManager->HandleKeyboard(isTitle);
}

void ContextQuit()
{
    GetContext()->Quit();
}

u8string ContextOpenCommonFileDialog(OpenRCT2::Ui::FileDialogDesc& desc)
{
    try
    {
        return GetContext()->GetUiContext()->ShowFileDialog(desc);
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR(ex.what());
        return u8string{};
    }
}
