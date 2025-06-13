/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/network/Network.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr ScreenSize kWindowSizeSave = { 260, 54 };
    static constexpr ScreenSize kWindowSizeQuit = { 177, 38 };

    enum WindowSavePromptWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_LABEL,
        WIDX_SAVE,
        WIDX_DONT_SAVE,
        WIDX_CANCEL
    };

    // clang-format off
    static constexpr auto _savePromptWidgets = makeWidgets(
        makeWindowShim(kStringIdNone, kWindowSizeSave),
        makeWidget({  2, 19}, {256, 12}, WidgetType::labelCentred, WindowColour::primary, kStringIdEmpty                ), // question/label
        makeWidget({  8, 35}, { 78, 14}, WidgetType::button,        WindowColour::primary, STR_SAVE_PROMPT_SAVE     ), // save
        makeWidget({ 91, 35}, { 78, 14}, WidgetType::button,        WindowColour::primary, STR_SAVE_PROMPT_DONT_SAVE), // don't save
        makeWidget({174, 35}, { 78, 14}, WidgetType::button,        WindowColour::primary, STR_SAVE_PROMPT_CANCEL   ) // cancel
    );
    // clang-format on

    enum WindowQuitPromptWidgetIdx
    {
        WQIDX_BACKGROUND,
        WQIDX_TITLE,
        WQIDX_CLOSE,
        WQIDX_OK,
        WQIDX_CANCEL
    };

    // clang-format off
    static constexpr auto _quitPromptWidgets = makeWidgets(
        makeWindowShim(STR_QUIT_GAME_PROMPT_TITLE, kWindowSizeQuit),
        makeWidget({ 8, 19}, {78, 14}, WidgetType::button, WindowColour::primary, STR_OK    ), // ok
        makeWidget({91, 19}, {78, 14}, WidgetType::button, WindowColour::primary, STR_CANCEL)  // cancel
    );
    // clang-format on

    static constexpr StringId window_save_prompt_labels[][2] = {
        { STR_LOAD_GAME_PROMPT_TITLE, STR_SAVE_BEFORE_LOADING },
        { STR_QUIT_GAME_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING },
        { STR_QUIT_GAME_2_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING_2 },
        { STR_NEW_GAME, STR_SAVE_BEFORE_QUITTING },
    };

    static void WindowSavePromptCallback(ModalResult result, const utf8* path)
    {
        if (result == ModalResult::ok)
        {
            GameLoadOrQuitNoSavePrompt();
        }
    }

    class SavePromptWindow final : public Window
    {
    private:
        PromptMode _promptMode;

    public:
        SavePromptWindow(PromptMode promptMode)
            : _promptMode(promptMode)
        {
        }

        void OnOpen() override
        {
            bool canSave = !(isInTrackDesignerOrManager());

            if (canSave)
                SetWidgets(_savePromptWidgets);
            else
                SetWidgets(_quitPromptWidgets);

            InitScrollWidgets();

            // Pause the game if not network play.
            if (NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused |= GAME_PAUSED_MODAL;
                Audio::StopAll();
            }

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::TopToolbar);

            if (canSave)
            {
                StringId stringId = window_save_prompt_labels[EnumValue(_promptMode)][0];
                if (stringId == STR_LOAD_GAME_PROMPT_TITLE && gLegacyScene == LegacyScene::scenarioEditor)
                {
                    stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
                }
                else if (stringId == STR_QUIT_GAME_PROMPT_TITLE && gLegacyScene == LegacyScene::scenarioEditor)
                {
                    stringId = STR_QUIT_SCENARIO_EDITOR;
                }
                widgets[WIDX_TITLE].text = stringId;
                widgets[WIDX_LABEL].text = window_save_prompt_labels[EnumValue(_promptMode)][1];
            }
        }

        void OnClose() override
        {
            // Unpause the game
            if (NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused &= ~GAME_PAUSED_MODAL;
                Audio::Resume();
            }

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::TopToolbar);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            if (gLegacyScene == LegacyScene::titleSequence || gLegacyScene == LegacyScene::trackDesigner
                || gLegacyScene == LegacyScene::trackDesignsManager)
            {
                switch (widgetIndex)
                {
                    case WQIDX_OK:
                        GameLoadOrQuitNoSavePrompt();
                        break;
                    case WQIDX_CLOSE:
                    case WQIDX_CANCEL:
                        Close();
                        break;
                }
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_SAVE:
                {
                    std::unique_ptr<Intent> intent;

                    if (isInEditorMode())
                    {
                        intent = std::make_unique<Intent>(WindowClass::Loadsave);
                        intent->PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::save);
                        intent->PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::landscape);
                        intent->PutExtra(INTENT_EXTRA_PATH, getGameState().scenarioName);
                    }
                    else
                    {
                        intent = CreateSaveGameAsIntent();
                    }
                    Close();
                    intent->PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(WindowSavePromptCallback));
                    ContextOpenIntent(intent.get());
                    break;
                }
                case WIDX_DONT_SAVE:
                    GameLoadOrQuitNoSavePrompt();
                    return;
                case WIDX_CLOSE:
                case WIDX_CANCEL:
                    Close();
                    return;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);
        }
    };

    WindowBase* SavePromptOpen()
    {
        PromptMode prompt_mode = gSavePromptMode;
        if (prompt_mode == PromptMode::quit)
        {
            prompt_mode = PromptMode::saveBeforeQuit;
        }

        // do not show save prompt if we're in the title demo and click on load game
        if (gLegacyScene == LegacyScene::titleSequence)
        {
            GameLoadOrQuitNoSavePrompt();
            return nullptr;
        }

        if (!Config::Get().general.ConfirmationPrompt)
        {
            /* game_load_or_quit_no_save_prompt() will exec requested task and close this window
             * immediately again.
             * TODO restructure these functions when we're sure game_load_or_quit_no_save_prompt()
             * and game_load_or_quit() are not called by the original binary anymore.
             */

            if (gScreenAge < 3840 && NetworkGetMode() == NETWORK_MODE_NONE)
            {
                GameLoadOrQuitNoSavePrompt();
                return nullptr;
            }
        }

        auto* windowMgr = GetWindowManager();

        // Check if window is already open
        auto* window = windowMgr->BringToFrontByClass(WindowClass::SavePrompt);
        if (window != nullptr)
        {
            windowMgr->Close(*window);
        }

        if (EnumValue(prompt_mode) >= std::size(window_save_prompt_labels))
        {
            LOG_WARNING("Invalid save prompt mode %u", prompt_mode);
            return nullptr;
        }

        auto windowSize = kWindowSizeSave;
        if (isInTrackDesignerOrManager())
        {
            windowSize = kWindowSizeQuit;
        }

        auto savePromptWindow = std::make_unique<SavePromptWindow>(prompt_mode);
        return windowMgr->Create(
            std::move(savePromptWindow), WindowClass::SavePrompt, {}, windowSize,
            WF_TRANSPARENT | WF_STICK_TO_FRONT | WF_CENTRE_SCREEN | WF_AUTO_POSITION);
    }
} // namespace OpenRCT2::Ui::Windows
