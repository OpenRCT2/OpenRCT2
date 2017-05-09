#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <memory>
#include <string>
#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileStream.hpp"
#include "core/Guard.hpp"
#include "core/String.hpp"
#include "FileClassifier.h"
#include "network/network.h"
#include "object/ObjectRepository.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "platform/crash.h"
#include "PlatformEnvironment.h"
#include "ride/TrackDesignRepository.h"
#include "scenario/ScenarioRepository.h"
#include "title/TitleScreen.h"
#include "title/TitleSequenceManager.h"
#include "Version.h"

extern "C"
{
    #include "audio/audio.h"
    #include "config/Config.h"
    #include "editor.h"
    #include "game.h"
    #include "interface/chat.h"
    #include "interface/themes.h"
    #include "intro.h"
    #include "localisation/localisation.h"
    #include "network/http.h"
    #include "object_list.h"
    #include "platform/platform.h"
    #include "rct1.h"
    #include "rct2.h"
    #include "rct2/interop.h"
}

// The game update interval in milliseconds, (1000 / 40fps) = 25ms
constexpr uint32 UPDATE_TIME_MS = 25;

extern "C"
{
    sint32 gExitCode;
    sint32 gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
    utf8 gOpenRCT2StartupActionPath[512] = { 0 };
    utf8 gExePath[MAX_PATH];
    utf8 gCustomUserDataPath[MAX_PATH] = { 0 };
    utf8 gCustomOpenrctDataPath[MAX_PATH] = { 0 };
    utf8 gCustomRCT2DataPath[MAX_PATH] = { 0 };
    utf8 gCustomPassword[MAX_PATH] = { 0 };

    // This should probably be changed later and allow a custom selection of things to initialise like SDL_INIT
    bool gOpenRCT2Headless = false;

    bool gOpenRCT2ShowChangelog;
    bool gOpenRCT2SilentBreakpad;

#ifndef DISABLE_NETWORK
    // OpenSSL's message digest context used for calculating sprite checksums
    EVP_MD_CTX * gHashCTX = nullptr;
#endif // DISABLE_NETWORK
}

namespace OpenRCT2
{
    static IPlatformEnvironment * _env = nullptr;
    static bool _isWindowMinimised;
    static uint32 _isWindowMinimisedLastCheckTick;
    static uint32 _lastTick;
    static uint32 _uncapTick;

    /** If set, will end the OpenRCT2 game loop. Intentionally private to this module so that the flag can not be set back to false. */
    static bool _finished;

    static bool ShouldRunVariableFrame();
    static void RunGameLoop();
    static void RunFixedFrame();
    static void RunVariableFrame();

    static bool OpenParkAutoDetectFormat(const utf8 * path);
}

extern "C"
{
    void openrct2_write_full_version_info(utf8 * buffer, size_t bufferSize)
    {
        String::Set(buffer, bufferSize, gVersionInfoFull);
    }

    static void openrct2_set_exe_path()
    {
        platform_get_exe_path(gExePath, sizeof(gExePath));
        log_verbose("Setting exe path to %s", gExePath);
    }

    bool openrct2_initialise()
    {
#ifndef DISABLE_NETWORK
        gHashCTX = EVP_MD_CTX_create();
        Guard::Assert(gHashCTX != nullptr, "EVP_MD_CTX_create failed");
#endif // DISABLE_NETWORK

        crash_init();

        // Sets up the environment OpenRCT2 is running in, e.g. directory paths
        OpenRCT2::_env = OpenRCT2::SetupEnvironment();
        if (OpenRCT2::_env == nullptr)
        {
            return false;
        }

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

        // TODO add configuration option to allow multiple instances
        // if (!gOpenRCT2Headless && !platform_lock_single_instance()) {
        // 	log_fatal("OpenRCT2 is already running.");
        // 	return false;
        // }

        IObjectRepository * objRepo = CreateObjectRepository(OpenRCT2::_env);
        ITrackDesignRepository * tdRepo = CreateTrackDesignRepository(OpenRCT2::_env);
        IScenarioRepository * scenarioRepo = CreateScenarioRepository(OpenRCT2::_env);

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
        objRepo->LoadOrConstruct();

        // TODO Like objects, this can take a while if there are a lot of track designs
        //      its also really something really we might want to do in the background
        //      as its not required until the player wants to place a new ride.
        tdRepo->Scan();

        scenarioRepo->Scan();
        TitleSequenceManager::Scan();

        if (!gOpenRCT2Headless)
        {
            audio_init();
            audio_populate_devices();
        }

        http_init();
        theme_manager_initialise();

        rct2_interop_setup_hooks();

        if (!rct2_init())
        {
            return false;
        }

        chat_init();

        rct2_copy_original_user_files_over();
        return true;
    }

    /**
     * Launches the game, after command line arguments have been parsed and processed.
     */
    void openrct2_launch()
    {
        if (openrct2_initialise())
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
                bool parkLoaded = false;
                // A path that includes "://" is illegal with all common filesystems, so it is almost certainly a URL
                // This way all cURL supported protocols, like http, ftp, scp and smb are automatically handled
                if (strstr(gOpenRCT2StartupActionPath, "://") != nullptr)
                {
#ifndef DISABLE_HTTP
                    // Download park and open it using its temporary filename
                    char tmpPath[MAX_PATH];
                    if (!http_download_park(gOpenRCT2StartupActionPath, tmpPath))
                    {
                        title_load();
                        break;
                    }

                    parkLoaded = OpenRCT2::OpenParkAutoDetectFormat(tmpPath);
#endif
                }
                else
                {
                    parkLoaded = rct2_open_file(gOpenRCT2StartupActionPath);
                }

                if (!parkLoaded)
                {
                    Console::Error::WriteLine("Failed to load '%s'", gOpenRCT2StartupActionPath);
                    title_load();
                    break;
                }

                gScreenFlags = SCREEN_FLAGS_PLAYING;

#ifndef DISABLE_NETWORK
                if (gNetworkStart == NETWORK_MODE_SERVER)
                {
                    if (gNetworkStartPort == 0)
                    {
                        gNetworkStartPort = gConfigNetwork.default_port;
                    }

                    if (String::IsNullOrEmpty(gCustomPassword))
                    {
                        network_set_password(gConfigNetwork.default_password);
                    }
                    else
                    {
                        network_set_password(gCustomPassword);
                    }
                    network_begin_server(gNetworkStartPort);
                }
#endif // DISABLE_NETWORK
                break;
            }
            case STARTUP_ACTION_EDIT:
                if (String::SizeOf(gOpenRCT2StartupActionPath) == 0)
                {
                    editor_load();
                }
                else if (!editor_load_landscape(gOpenRCT2StartupActionPath))
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

            OpenRCT2::RunGameLoop();
        }
        openrct2_dispose();
    }

    void openrct2_dispose()
    {
        network_close();
        http_dispose();
        language_close_all();
        rct2_dispose();
        config_release();
#ifndef DISABLE_NETWORK
        EVP_MD_CTX_destroy(gHashCTX);
#endif // DISABLE_NETWORK
        rct2_interop_dispose();
        platform_free();
    }

    /**
     * Causes the OpenRCT2 game loop to finish.
     */
    void openrct2_finish()
    {
        OpenRCT2::_finished = true;
    }

    bool check_file_path(sint32 pathId)
    {
        const utf8 * path = get_file_path(pathId);
        switch (pathId) {
        case PATH_ID_G1:
            if (!File::Exists(path))
            {
                Console::Error::WriteLine("Unable to find '%s'", path);
                return false;
            }
            break;
        case PATH_ID_CUSTOM1:
        case PATH_ID_CUSTOM2:
            if (File::Exists(path))
            {
                try
                {
                    auto fs = FileStream(path, FILE_MODE_OPEN);
                    sint32 index = 36 + (pathId - PATH_ID_CUSTOM1);
                    gRideMusicInfoList[index]->length = fs.GetLength();
                }
                catch (const Exception &)
                {
                }
            }
            break;
        }
        return true;
    }
}

namespace OpenRCT2
{
    IPlatformEnvironment * SetupEnvironment()
    {
        utf8 userPath[MAX_PATH];
        platform_resolve_openrct_data_path();
        platform_resolve_user_data_path();
        platform_get_user_directory(userPath, NULL, sizeof(userPath));
        if (!platform_ensure_directory_exists(userPath))
        {
            Console::Error::WriteLine("Could not create user directory (do you have write access to your documents folder?)");
            return nullptr;
        }
        openrct2_set_exe_path();

        config_set_defaults();
        if (!config_open_default())
        {
            if (!config_find_or_browse_install_directory())
            {
                gConfigGeneral.last_run_version = String::Duplicate(OPENRCT2_VERSION);
                config_save_default();
                utf8 path[MAX_PATH];
                config_get_default_path(path, sizeof(path));
                Console::Error::WriteLine("An RCT2 install directory must be specified! Please edit \"game_path\" in %s.", path);
                return nullptr;
            }
            config_save_default();
        }

        if (!rct2_init_directories())
        {
            return nullptr;
        }
        if (!rct2_startup_checks())
        {
            return nullptr;
        }

        utf8 path[260];
        std::string basePaths[4];
        basePaths[(size_t)DIRBASE::RCT1] = String::ToStd(gConfigGeneral.rct1_path);
        basePaths[(size_t)DIRBASE::RCT2] = String::ToStd(gConfigGeneral.rct2_path);
        platform_get_openrct_data_path(path, sizeof(path));
        basePaths[(size_t)DIRBASE::OPENRCT2] = std::string(path);
        platform_get_user_directory(path, nullptr, sizeof(path));
        basePaths[(size_t)DIRBASE::USER] = std::string(path);

        IPlatformEnvironment * env = CreatePlatformEnvironment(basePaths);
        return env;
    }

    /**
     * Run the main game loop until the finished flag is set.
     */
    static void RunGameLoop()
    {
        log_verbose("begin openrct2 loop");
        _finished = false;
        do
        {
            if (ShouldRunVariableFrame())
            {
                RunVariableFrame();
            }
            else
            {
                RunFixedFrame();
            }
        }
        while (!_finished);
        log_verbose("finish openrct2 loop");
    }

    static bool IsMinimised()
    {
        // Don't check if window is minimised too frequently (every second is fine)
        if (_lastTick > _isWindowMinimisedLastCheckTick + 1000)
        {
            uint32 windowFlags = SDL_GetWindowFlags(gWindow);
            _isWindowMinimised = (windowFlags & SDL_WINDOW_MINIMIZED) ||
                                 (windowFlags & SDL_WINDOW_HIDDEN);
        }
        return _isWindowMinimised;
    }

    static bool ShouldRunVariableFrame()
    {
        if (!gConfigGeneral.uncap_fps) return false;
        if (gGameSpeed > 4) return false;
        if (gOpenRCT2Headless) return false;
        if (IsMinimised()) return false;
        return true;
    }

    static void RunFixedFrame()
    {
        _uncapTick = 0;
        uint32 currentTick = SDL_GetTicks();
        uint32 ticksElapsed = currentTick - _lastTick;
        if (ticksElapsed < UPDATE_TIME_MS)
        {
            SDL_Delay(UPDATE_TIME_MS - ticksElapsed);
            _lastTick += UPDATE_TIME_MS;
        }
        else
        {
            _lastTick = currentTick;
        }
        platform_process_messages();
        rct2_update();
        if (!_isWindowMinimised)
        {
            platform_draw();
        }
    }

    static void RunVariableFrame()
    {
        uint32 currentTick = SDL_GetTicks();
        if (_uncapTick == 0)
        {
            _uncapTick = currentTick;
            sprite_position_tween_reset();
        }

        // Limit number of updates per loop (any long pauses or debugging can make this update for a very long time)
        if (currentTick - _uncapTick > UPDATE_TIME_MS * 60)
        {
            _uncapTick = currentTick - UPDATE_TIME_MS - 1;
        }

        platform_process_messages();

        while (_uncapTick <= currentTick && currentTick - _uncapTick > UPDATE_TIME_MS)
        {
            // Get the original position of each sprite
            sprite_position_tween_store_a();

            // Update the game so the sprite positions update
            rct2_update();

            // Get the next position of each sprite
            sprite_position_tween_store_b();

            _uncapTick += UPDATE_TIME_MS;
        }

        // Tween the position of each sprite from the last position to the new position based on the time between the last
        // tick and the next tick.
        float nudge = 1 - ((float)(currentTick - _uncapTick) / UPDATE_TIME_MS);
        sprite_position_tween_all(nudge);

        platform_draw();

        sprite_position_tween_restore();
    }

    static bool OpenParkAutoDetectFormat(const utf8 * path)
    {
        ClassifiedFile info;
        if (TryClassifyFile(path, &info))
        {
            if (info.Type == FILE_TYPE::SAVED_GAME ||
                info.Type == FILE_TYPE::SCENARIO)
            {
                std::unique_ptr<IParkImporter> parkImporter;
                if (info.Version <= 2)
                {
                    parkImporter.reset(ParkImporter::CreateS4());
                }
                else
                {
                    parkImporter.reset(ParkImporter::CreateS6());
                }

                if (info.Type == FILE_TYPE::SAVED_GAME)
                {
                    try
                    {
                        parkImporter->LoadSavedGame(path);
                        parkImporter->Import();
                        game_fix_save_vars();
                        sprite_position_tween_reset();
                        gScreenAge = 0;
                        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
                        game_load_init();
                        return true;
                    }
                    catch (const Exception &)
                    {
                        Console::Error::WriteLine("Error loading saved game.");
                    }
                }
                else
                {
                    try
                    {
                        parkImporter->LoadScenario(path);
                        parkImporter->Import();
                        game_fix_save_vars();
                        sprite_position_tween_reset();
                        gScreenAge = 0;
                        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
                        scenario_begin();
                        return true;
                    }
                    catch (const Exception &)
                    {
                        Console::Error::WriteLine("Error loading scenario.");
                    }
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
}
