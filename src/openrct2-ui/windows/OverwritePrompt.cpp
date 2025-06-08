/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <SDL_keycode.h>
#include <openrct2-ui/interface/FileBrowser.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/ui/WindowManager.h>
#include <string>

struct TrackDesign;

namespace OpenRCT2::Ui::Windows
{
    constexpr int32_t OVERWRITE_WW = 200;
    constexpr int32_t OVERWRITE_WH = 100;

    enum
    {
        WIDX_OVERWRITE_BACKGROUND,
        WIDX_OVERWRITE_TITLE,
        WIDX_OVERWRITE_CLOSE,
        WIDX_OVERWRITE_OVERWRITE,
        WIDX_OVERWRITE_CANCEL
    };

    // clang-format off
    static constexpr auto window_overwrite_prompt_widgets = makeWidgets(
        makeWindowShim(STR_FILEBROWSER_OVERWRITE_TITLE, OVERWRITE_WW, OVERWRITE_WH),
        MakeWidget({                10, OVERWRITE_WH - 20 }, { 84, 11 }, WindowWidgetType::Button, WindowColour::Primary, STR_FILEBROWSER_OVERWRITE_TITLE),
        MakeWidget({ OVERWRITE_WW - 95, OVERWRITE_WH - 20 }, { 85, 11 }, WindowWidgetType::Button, WindowColour::Primary, STR_SAVE_PROMPT_CANCEL)
    );
    // clang-format on

    class OverwritePromptWindow final : public Window
    {
        std::string _name;
        std::string _path;
        LoadSaveAction _action;
        LoadSaveType _type;
        TrackDesign* _trackDesign;

    public:
        OverwritePromptWindow(
            const std::string_view name, const std::string_view path, LoadSaveAction action, LoadSaveType type,
            TrackDesign* trackDesignPtr)
            : _name(name)
            , _path(path)
            , _action(action)
            , _type(type)
            , _trackDesign(trackDesignPtr)
        {
        }

        void OnOpen() override
        {
            SetWidgets(window_overwrite_prompt_widgets);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_OVERWRITE_OVERWRITE:
                {
                    FileBrowser::Select(_path.c_str(), _action, _type, _trackDesign);

                    // As the LoadSaveWindow::Select function can change the order of the
                    // windows we can't use WindowClose(w).
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::LoadsaveOverwritePrompt);
                    break;
                }

                case WIDX_OVERWRITE_CANCEL:
                case WIDX_OVERWRITE_CLOSE:
                    Close();
                    break;
            }
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);

            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<char*>(_name.c_str());

            ScreenCoordsXY stringCoords(windowPos.x + width / 2, windowPos.y + (height / 2) - 3);
            DrawTextWrapped(rt, stringCoords, width - 4, STR_FILEBROWSER_OVERWRITE_PROMPT, ft, { TextAlignment::CENTRE });
        }
    };

    WindowBase* WindowOverwritePromptOpen(
        const std::string_view name, const std::string_view path, LoadSaveAction action, LoadSaveType type,
        TrackDesign* trackDesignPtr)
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::LoadsaveOverwritePrompt);

        return windowMgr->Create<OverwritePromptWindow>(
            WindowClass::LoadsaveOverwritePrompt, OVERWRITE_WW, OVERWRITE_WH,
            WF_TRANSPARENT | WF_STICK_TO_FRONT | WF_CENTRE_SCREEN, name, path, action, type, trackDesignPtr);
    }

    void WindowLoadSaveOverwritePromptInputKey(WindowBase* w, uint32_t keycode)
    {
        if (w->classification != WindowClass::LoadsaveOverwritePrompt)
        {
            return;
        }

        auto promptWindow = static_cast<OverwritePromptWindow*>(w);

        if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
        {
            promptWindow->OnMouseUp(WIDX_OVERWRITE_OVERWRITE);
        }
        else if (keycode == SDLK_ESCAPE)
        {
            promptWindow->OnMouseUp(WIDX_OVERWRITE_CANCEL);
        }
    }
} // namespace OpenRCT2::Ui::Windows
