/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_STRING;
    static constexpr int32_t WH = 44;
    static constexpr int32_t WW = 250;
    static constexpr int32_t WH_DELETE_PROMPT = 74;
    static constexpr int32_t WW_DELETE_PROMPT = 250;
    static constexpr int32_t TrackDesignNameMaxLength = 127;

#pragma region Widgets

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_RENAME,
        WIDX_DELETE,

        WIDX_PROMPT_DELETE = 3,
        WIDX_PROMPT_CANCEL = 4,
    };

    // clang-format off
    static constexpr Widget _trackManageWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({ 10, 24}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_RENAME),
        MakeWidget({130, 24}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_DELETE),
    };

    static constexpr Widget _trackDeletePromptWidgets[] = {
        WINDOW_SHIM(STR_DELETE_FILE, WW_DELETE_PROMPT, WH_DELETE_PROMPT),
        MakeWidget({ 10, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_DELETE),
        MakeWidget({130, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_CANCEL             ),
    };
    // clang-format on

#pragma endregion

    class TrackDesignManageWindow final : public Window
    {
    private:
        TrackDesignFileRef* _trackDesignFileReference{ nullptr };

    public:
        TrackDesignManageWindow(TrackDesignFileRef* tdFileRef)
            : _trackDesignFileReference(tdFileRef)
        {
        }

        void OnOpen() override;
        void OnClose() override;
        void OnMouseUp(WidgetIndex widgetIndex) override;
        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override;
        void OnDraw(DrawPixelInfo& dpi) override;

        void OnResize() override
        {
            ResizeFrame();
        }
    };

    class TrackDeletePromptWindow final : public Window
    {
    private:
        TrackDesignFileRef* _trackDesignFileReference{ nullptr };

    public:
        TrackDeletePromptWindow(TrackDesignFileRef* tdFileRef)
            : _trackDesignFileReference(tdFileRef)
        {
        }

        void OnOpen() override;
        void OnMouseUp(WidgetIndex widgetIndex) override;
        void OnDraw(DrawPixelInfo& dpi) override;

        void OnResize() override
        {
            ResizeFrame();
        }
    };

    static void WindowTrackDeletePromptOpen(TrackDesignFileRef* tdFileRef);

    /**
     *
     *  rct2: 0x006D348F
     */
    WindowBase* TrackManageOpen(TrackDesignFileRef* tdFileRef)
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::ManageTrackDesign);
        auto trackDesignManageWindow = std::make_unique<TrackDesignManageWindow>(tdFileRef);

        auto* window = windowMgr->Create(
            std::move(trackDesignManageWindow), WindowClass::ManageTrackDesign, {}, WW, WH,
            WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_CENTRE_SCREEN | WF_AUTO_POSITION);

        return window;
    }

    void TrackDesignManageWindow::OnOpen()
    {
        SetWidgets(_trackManageWidgets);
        WindowInitScrollWidgets(*this);

        WindowTrackDesignListSetBeingUpdated(true);
    }

    void TrackDesignManageWindow::OnClose()
    {
        WindowTrackDesignListSetBeingUpdated(false);
    }

    void TrackDesignManageWindow::OnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseByClass(WindowClass::TrackDeletePrompt);
                Close();
                break;
            }
            case WIDX_RENAME:
                WindowTextInputRawOpen(
                    this, widgetIndex, STR_TRACK_DESIGN_RENAME_TITLE, STR_TRACK_DESIGN_RENAME_DESC, {},
                    _trackDesignFileReference->name.c_str(), TrackDesignNameMaxLength);
                break;
            case WIDX_DELETE:
                WindowTrackDeletePromptOpen(_trackDesignFileReference);
                break;
        }
    }

    void TrackDesignManageWindow::OnTextInput(WidgetIndex widgetIndex, std::string_view text)
    {
        if (widgetIndex != WIDX_RENAME)
        {
            return;
        }
        else if (text.empty())
        {
            ContextShowError(STR_CANT_RENAME_TRACK_DESIGN, kStringIdNone, {});
            return;
        }
        else if (!Platform::IsFilenameValid(text))
        {
            ContextShowError(STR_CANT_RENAME_TRACK_DESIGN, STR_NEW_NAME_CONTAINS_INVALID_CHARACTERS, {});
            return;
        }

        if (TrackRepositoryRename(_trackDesignFileReference->path, std::string(text)))
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::TrackDeletePrompt);
            Close();
            WindowTrackDesignListReloadTracks();
        }
        else
        {
            ContextShowError(STR_CANT_RENAME_TRACK_DESIGN, STR_ANOTHER_FILE_EXISTS_WITH_NAME_OR_FILE_IS_WRITE_PROTECTED, {});
        }
    }

    void TrackDesignManageWindow::OnDraw(DrawPixelInfo& dpi)
    {
        Formatter::Common().Add<const utf8*>(_trackDesignFileReference->name.c_str());
        DrawWidgets(dpi);
    }

    /**
     *
     *  rct2: 0x006D35CD
     */
    static void WindowTrackDeletePromptOpen(TrackDesignFileRef* tdFileRef)
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::TrackDeletePrompt);

        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        auto trackDeletePromptWindow = std::make_unique<TrackDeletePromptWindow>(tdFileRef);

        windowMgr->Create(
            std::move(trackDeletePromptWindow), WindowClass::TrackDeletePrompt,
            ScreenCoordsXY(
                std::max(kTopToolbarHeight + 1, (screenWidth - WW_DELETE_PROMPT) / 2), (screenHeight - WH_DELETE_PROMPT) / 2),
            WW_DELETE_PROMPT, WH_DELETE_PROMPT, WF_STICK_TO_FRONT | WF_TRANSPARENT);
    }

    void TrackDeletePromptWindow::OnOpen()
    {
        SetWidgets(_trackDeletePromptWidgets);
        WindowInitScrollWidgets(*this);
    }

    void TrackDeletePromptWindow::OnMouseUp(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
            case WIDX_PROMPT_CANCEL:
                Close();
                break;
            case WIDX_PROMPT_DELETE:
                // tdPath has to be saved before window is closed, as that would blank it out.
                auto tdPath = _trackDesignFileReference->path;
                Close();
                if (TrackRepositoryDelete(tdPath))
                {
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::ManageTrackDesign);
                    WindowTrackDesignListReloadTracks();
                }
                else
                {
                    ContextShowError(STR_CANT_DELETE_TRACK_DESIGN, STR_FILE_IS_WRITE_PROTECTED_OR_LOCKED, {});
                }
                break;
        }
    }

    void TrackDeletePromptWindow::OnDraw(DrawPixelInfo& dpi)
    {
        DrawWidgets(dpi);

        auto ft = Formatter();
        ft.Add<const utf8*>(_trackDesignFileReference->name.c_str());
        DrawTextWrapped(
            dpi, { windowPos.x + (WW_DELETE_PROMPT / 2), windowPos.y + ((WH_DELETE_PROMPT / 2) - 9) }, (WW_DELETE_PROMPT - 4),
            STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK, ft, { TextAlignment::CENTRE });
    }
} // namespace OpenRCT2::Ui::Windows
