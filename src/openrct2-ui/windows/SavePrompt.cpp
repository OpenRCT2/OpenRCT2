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
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/network/network.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t WH_SAVE = 54;
    static constexpr int32_t WW_SAVE = 260;
    static constexpr int32_t WH_QUIT = 38;
    static constexpr int32_t WW_QUIT = 177;

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
    static Widget _savePromptWidgets[] = {
        WINDOW_SHIM_WHITE(STR_NONE, WW_SAVE, WH_SAVE),
        MakeWidget({  2, 19}, {256, 12}, WindowWidgetType::LabelCentred, WindowColour::Primary, STR_EMPTY                ), // question/label
        MakeWidget({  8, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_SAVE     ), // save
        MakeWidget({ 91, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_DONT_SAVE), // don't save
        MakeWidget({174, 35}, { 78, 14}, WindowWidgetType::Button,        WindowColour::Primary, STR_SAVE_PROMPT_CANCEL   ), // cancel
        kWidgetsEnd,
    };
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
    static Widget _quitPromptWidgets[] = {
        WINDOW_SHIM_WHITE(STR_QUIT_GAME_PROMPT_TITLE, WW_QUIT, WH_QUIT),
        MakeWidget({ 8, 19}, {78, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_OK    ), // ok
        MakeWidget({91, 19}, {78, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_CANCEL), // cancel
        kWidgetsEnd,
    };
    // clang-format on

    static constexpr StringId window_save_prompt_labels[][2] = {
        { STR_LOAD_GAME_PROMPT_TITLE, STR_SAVE_BEFORE_LOADING },
        { STR_QUIT_GAME_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING },
        { STR_QUIT_GAME_2_PROMPT_TITLE, STR_SAVE_BEFORE_QUITTING_2 },
        { STR_NEW_GAME, STR_SAVE_BEFORE_QUITTING },
    };

    static void WindowSavePromptCallback(int32_t result, const utf8* path)
    {
        if (result == MODAL_RESULT_OK)
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
            bool canSave = !(gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER));

            widgets = canSave ? _savePromptWidgets : _quitPromptWidgets;

            InitScrollWidgets();

            // Pause the game if not network play.
            if (NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused |= GAME_PAUSED_MODAL;
                Audio::StopAll();
            }

            WindowInvalidateByClass(WindowClass::TopToolbar);

            if (canSave)
            {
                StringId stringId = window_save_prompt_labels[EnumValue(_promptMode)][0];
                if (stringId == STR_LOAD_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    stringId = STR_LOAD_LANDSCAPE_PROMPT_TITLE;
                }
                else if (stringId == STR_QUIT_GAME_PROMPT_TITLE && gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
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

            WindowInvalidateByClass(WindowClass::TopToolbar);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            if (gScreenFlags & (SCREEN_FLAGS_TITLE_DEMO | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
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

                    if (gScreenFlags & (SCREEN_FLAGS_EDITOR))
                    {
                        intent = std::make_unique<Intent>(WindowClass::Loadsave);
                        intent->PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE);
                        intent->PutExtra(INTENT_EXTRA_PATH, GetGameState().ScenarioName);
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

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
        }

        void OnResize() override
        {
            ResizeFrame();
        }
    };

    WindowBase* SavePromptOpen()
    {
        PromptMode prompt_mode = gSavePromptMode;
        if (prompt_mode == PromptMode::Quit)
        {
            prompt_mode = PromptMode::SaveBeforeQuit;
        }

        // do not show save prompt if we're in the title demo and click on load game
        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
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

        // Check if window is already open
        WindowBase* window = WindowBringToFrontByClass(WindowClass::SavePrompt);
        if (window != nullptr)
        {
            WindowClose(*window);
        }

        if (EnumValue(prompt_mode) >= std::size(window_save_prompt_labels))
        {
            LOG_WARNING("Invalid save prompt mode %u", prompt_mode);
            return nullptr;
        }

        int32_t width = WW_SAVE;
        int32_t height = WH_SAVE;
        if (gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER))
        {
            width = WW_QUIT;
            height = WH_QUIT;
        }

        auto savePromptWindow = std::make_unique<SavePromptWindow>(prompt_mode);
        return WindowCreate(
            std::move(savePromptWindow), WindowClass::SavePrompt, {}, width, height,
            WF_TRANSPARENT | WF_STICK_TO_FRONT | WF_CENTRE_SCREEN | WF_AUTO_POSITION);
    }
} // namespace OpenRCT2::Ui::Windows
