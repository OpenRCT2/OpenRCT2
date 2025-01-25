/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../Input.h"
#include "../../OpenRCT2.h"
#include "../../audio/audio.h"
#include "../../config/Config.h"
#include "../../core/Console.hpp"
#include "../../drawing/Text.h"
#include "../../interface/Screenshot.h"
#include "../../interface/Viewport.h"
#include "../../network/NetworkBase.h"
#include "../../network/network.h"
#include "../../scenario/Scenario.h"
#include "../../scenario/ScenarioRepository.h"
#include "../../ui/UiContext.h"
#include "../../ui/WindowManager.h"
#include "../../util/Util.h"
#include "../../windows/Intent.h"
#include "TitleSequence.h"
#include "TitleSequenceManager.h"
#include "TitleSequencePlayer.h"

using namespace OpenRCT2;

// TODO Remove when no longer required.
bool gPreviewingTitleSequenceInGame;

ITitleSequencePlayer* TitleScene::GetSequencePlayer()
{
    return _sequencePlayer;
}

size_t TitleScene::GetCurrentSequence()
{
    return _currentSequence;
}

bool TitleScene::PreviewSequence(size_t value)
{
    _currentSequence = value;
    _previewingSequence = TryLoadSequence(true);
    if (_previewingSequence)
    {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            gPreviewingTitleSequenceInGame = true;
        }
    }
    else
    {
        _currentSequence = TitleGetConfigSequence();
        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        {
            TryLoadSequence();
        }
    }
    return _previewingSequence;
}

void TitleScene::StopPreviewingSequence()
{
    if (_previewingSequence)
    {
        WindowBase* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            WindowUnfollowSprite(*mainWindow);
        }
        _previewingSequence = false;
        _currentSequence = TitleGetConfigSequence();
        gPreviewingTitleSequenceInGame = false;
    }
}

bool TitleScene::IsPreviewingSequence()
{
    return _previewingSequence;
}

void TitleScene::Load()
{
    LOG_VERBOSE("TitleScene::Load()");

    if (GameIsPaused())
    {
        PauseToggle();
    }

    gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
    gScreenAge = 0;
    gCurrentLoadedPath.clear();

#ifndef DISABLE_NETWORK
    GetContext().GetNetwork().Close();
#endif
    gameStateInitAll(GetGameState(), kDefaultMapSize);
    ViewportInitAll();
    ContextOpenWindow(WindowClass::MainWindow);

    TitleInitialise();

    if (_sequencePlayer != nullptr)
    {
        // Force the title sequence to load / update so we
        // don't see a blank screen for a split second.
        _loadedTitleSequenceId = SIZE_MAX;
        TryLoadSequence();
        _sequencePlayer->Update();
    }

    Audio::PlayTitleMusic();

    CreateWindows();

    if (gOpenRCT2ShowChangelog)
    {
        gOpenRCT2ShowChangelog = false;
        ContextOpenWindow(WindowClass::Changelog);
    }

    LOG_VERBOSE("TitleScene::Load() finished");
}

void TitleScene::Tick()
{
    gInUpdateCode = true;

    ScreenshotCheck();
    TitleHandleKeyboardInput();

    if (GameIsNotPaused())
    {
        TryLoadSequence();
        _sequencePlayer->Update();

        int32_t numUpdates = 1;
        if (gGameSpeed > 1)
        {
            numUpdates = 1 << (gGameSpeed - 1);
        }
        for (int32_t i = 0; i < numUpdates; i++)
        {
            gameStateUpdateLogic();
        }
        UpdatePaletteEffects();
        // update_weather_animation();
    }

    InputSetFlag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

    ContextHandleInput();

    gInUpdateCode = false;
}

void TitleScene::Stop()
{
    Audio::StopAll();
}

void TitleScene::ChangePresetSequence(size_t preset)
{
    size_t count = TitleSequenceManager::GetCount();
    if (preset >= count)
    {
        return;
    }

    const utf8* configId = TitleSequenceManager::GetConfigID(preset);
    Config::Get().interface.CurrentTitleSequencePreset = configId;

    if (!_previewingSequence)
        _currentSequence = preset;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateAll();
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 */
void TitleScene::CreateWindows()
{
    ContextOpenWindow(WindowClass::TitleMenu);
    ContextOpenWindow(WindowClass::TitleExit);
    ContextOpenWindow(WindowClass::TitleOptions);
    ContextOpenWindow(WindowClass::TitleLogo);
    ContextOpenWindow(WindowClass::TitleVersion);
    WindowResizeGui(ContextGetWidth(), ContextGetHeight());
}

void TitleScene::TitleInitialise()
{
    if (_sequencePlayer == nullptr)
    {
        _sequencePlayer = GetContext().GetUiContext()->GetTitleSequencePlayer();
    }
    if (Config::Get().interface.RandomTitleSequence)
    {
        const size_t total = TitleSequenceManager::GetCount();
        if (total > 0)
        {
            bool RCT1Installed = false, RCT1AAInstalled = false, RCT1LLInstalled = false;
            uint32_t RCT1Count = 0;
            const size_t scenarioCount = ScenarioRepositoryGetCount();

            for (size_t s = 0; s < scenarioCount; s++)
            {
                const ScenarioSource sourceGame = ScenarioRepositoryGetByIndex(s)->SourceGame;
                switch (sourceGame)
                {
                    case ScenarioSource::RCT1:
                        RCT1Count++;
                        break;
                    case ScenarioSource::RCT1_AA:
                        RCT1AAInstalled = true;
                        break;
                    case ScenarioSource::RCT1_LL:
                        RCT1LLInstalled = true;
                        break;
                    default:
                        break;
                }
            }

            // Mega Park can show up in the scenario list even if RCT1 has been uninstalled, so it must be greater than 1
            RCT1Installed = RCT1Count > 1;

            int32_t random = 0;
            bool safeSequence = false;
            const std::string RCT1String = LanguageGetString(STR_TITLE_SEQUENCE_RCT1);
            const std::string RCT1AAString = LanguageGetString(STR_TITLE_SEQUENCE_RCT1_AA);
            const std::string RCT1LLString = LanguageGetString(STR_TITLE_SEQUENCE_RCT1_AA_LL);

            // Ensure the random sequence chosen isn't from RCT1 or expansion if the player doesn't have it installed
            while (!safeSequence)
            {
                random = UtilRand() % static_cast<int32_t>(total);
                const utf8* scName = TitleSequenceManager::GetName(random);
                if (scName == RCT1String)
                {
                    safeSequence = RCT1Installed;
                }
                else if (scName == RCT1AAString)
                {
                    safeSequence = RCT1AAInstalled;
                }
                else if (scName == RCT1LLString)
                {
                    safeSequence = RCT1LLInstalled;
                }
                else
                {
                    safeSequence = true;
                }
            }
            ChangePresetSequence(random);
        }
    }
    size_t seqId = TitleGetConfigSequence();
    if (seqId == SIZE_MAX)
    {
        seqId = TitleSequenceManager::GetIndexForConfigID("*OPENRCT2");
        if (seqId == SIZE_MAX)
        {
            seqId = 0;
        }
    }
    ChangePresetSequence(static_cast<int32_t>(seqId));
}

bool TitleScene::TryLoadSequence(bool loadPreview)
{
    if (_loadedTitleSequenceId != _currentSequence || loadPreview)
    {
        if (_sequencePlayer == nullptr)
        {
            _sequencePlayer = GetContext().GetUiContext()->GetTitleSequencePlayer();
        }

        size_t numSequences = TitleSequenceManager::GetCount();
        if (numSequences > 0)
        {
            size_t targetSequence = _currentSequence;
            do
            {
                if (_sequencePlayer->Begin(targetSequence) && _sequencePlayer->Update())
                {
                    _loadedTitleSequenceId = targetSequence;
                    if (targetSequence != _currentSequence && !loadPreview)
                    {
                        // Forcefully change the preset to a preset that works.
                        const utf8* configId = TitleSequenceManager::GetConfigID(targetSequence);
                        Config::Get().interface.CurrentTitleSequencePreset = configId;
                    }
                    _currentSequence = targetSequence;
                    GfxInvalidateScreen();
                    return true;
                }
                targetSequence = (targetSequence + 1) % numSequences;
            } while (targetSequence != _currentSequence && !loadPreview);
        }
        Console::Error::WriteLine("Unable to play any title sequences.");
        _sequencePlayer->Eject();
        _currentSequence = SIZE_MAX;
        _loadedTitleSequenceId = SIZE_MAX;
        if (!loadPreview)
        {
            gameStateInitAll(GetGameState(), kDefaultMapSize);
            GameNotifyMapChanged();
        }
        return false;
    }
    return true;
}

void TitleCreateWindows()
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        titleScene->CreateWindows();
    }
}

void* TitleGetSequencePlayer()
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        return titleScene->GetSequencePlayer();
    }
    return nullptr;
}

void TitleSequenceChangePreset(size_t preset)
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        titleScene->ChangePresetSequence(preset);
    }
}

size_t TitleGetConfigSequence()
{
    return TitleSequenceManager::GetIndexForConfigID(Config::Get().interface.CurrentTitleSequencePreset.c_str());
}

size_t TitleGetCurrentSequence()
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        return titleScene->GetCurrentSequence();
    }
    return 0;
}

bool TitlePreviewSequence(size_t value)
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        return titleScene->PreviewSequence(value);
    }
    return false;
}

void TitleStopPreviewingSequence()
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        titleScene->StopPreviewingSequence();
    }
}

bool TitleIsPreviewingSequence()
{
    auto* context = OpenRCT2::GetContext();
    auto* titleScene = static_cast<TitleScene*>(context->GetTitleScene());
    if (titleScene != nullptr)
    {
        return titleScene->IsPreviewingSequence();
    }
    return false;
}
