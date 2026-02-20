/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    static constexpr StringId kWindowTitle = STR_STRING;
    static constexpr ScreenSize kWindowSize = { 250, 44 };
    static constexpr ScreenSize kWindowSizeDeletePrompt = { 250, 74 };
    static constexpr int32_t kTrackDesignNameMaxLength = 127;

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
    static constexpr auto _trackManageWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ 10, 24}, {110, 12}, WidgetType::button, WindowColour::primary, STR_TRACK_MANAGE_RENAME),
        makeWidget({130, 24}, {110, 12}, WidgetType::button, WindowColour::primary, STR_TRACK_MANAGE_DELETE)
    );

    static constexpr auto _trackDeletePromptWidgets = makeWidgets(
        makeWindowShim(STR_DELETE_FILE, kWindowSizeDeletePrompt),
        makeWidget({ 10, 54}, {110, 12}, WidgetType::button, WindowColour::primary, STR_TRACK_MANAGE_DELETE),
        makeWidget({130, 54}, {110, 12}, WidgetType::button, WindowColour::primary, STR_CANCEL             )
    );
    // clang-format on

#pragma endregion

    static void WindowTrackDeletePromptOpen(TrackDesignFileRef* tdFileRef);

    class TrackDesignManageWindow final : public Window
    {
    private:
        TrackDesignFileRef* _trackDesignFileReference{ nullptr };

    public:
        TrackDesignManageWindow(TrackDesignFileRef* tdFileRef)
            : _trackDesignFileReference(tdFileRef)
        {
        }

        void onOpen() override
        {
            setWidgets(_trackManageWidgets);
            WindowInitScrollWidgets(*this);

            WindowTrackDesignListSetBeingUpdated(true);
        }

        void onClose() override
        {
            WindowTrackDesignListSetBeingUpdated(false);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                {
                    auto* windowMgr = GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::trackDeletePrompt);
                    close();
                    break;
                }
                case WIDX_RENAME:
                    WindowTextInputRawOpen(
                        this, widgetIndex, STR_TRACK_DESIGN_RENAME_TITLE, STR_TRACK_DESIGN_RENAME_DESC, {},
                        _trackDesignFileReference->name.c_str(), kTrackDesignNameMaxLength);
                    break;
                case WIDX_DELETE:
                    WindowTrackDeletePromptOpen(_trackDesignFileReference);
                    break;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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
                auto* windowMgr = GetWindowManager();
                windowMgr->CloseByClass(WindowClass::trackDeletePrompt);
                close();
                WindowTrackDesignListReloadTracks();
            }
            else
            {
                ContextShowError(
                    STR_CANT_RENAME_TRACK_DESIGN, STR_ANOTHER_FILE_EXISTS_WITH_NAME_OR_FILE_IS_WRITE_PROTECTED, {});
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            Formatter::Common().Add<const utf8*>(_trackDesignFileReference->name.c_str());
            drawWidgets(rt);
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

        void onOpen() override
        {
            setWidgets(_trackDeletePromptWidgets);
            WindowInitScrollWidgets(*this);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                case WIDX_PROMPT_CANCEL:
                    close();
                    break;
                case WIDX_PROMPT_DELETE:
                    // tdPath has to be saved before window is closed, as that would blank it out.
                    auto tdPath = _trackDesignFileReference->path;
                    close();
                    if (TrackRepositoryDelete(tdPath))
                    {
                        auto* windowMgr = GetWindowManager();
                        windowMgr->CloseByClass(WindowClass::manageTrackDesign);
                        WindowTrackDesignListReloadTracks();
                    }
                    else
                    {
                        ContextShowError(STR_CANT_DELETE_TRACK_DESIGN, STR_FILE_IS_WRITE_PROTECTED_OR_LOCKED, {});
                    }
                    break;
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            const auto titleBarBottom = widgets[WIDX_TITLE].bottom;
            const auto buttonTop = widgets[WIDX_PROMPT_DELETE].top;
            const auto fontHeight = FontGetLineHeight(FontStyle::medium);

            const auto maxMessageHeight = buttonTop - titleBarBottom;
            const auto messageTop = titleBarBottom + (maxMessageHeight - fontHeight) / 2;

            auto ft = Formatter();
            ft.Add<const utf8*>(_trackDesignFileReference->name.c_str());
            DrawTextWrapped(
                rt, windowPos + ScreenCoordsXY{ width / 2, messageTop }, (width - 4),
                STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK, ft, { TextAlignment::centre });
        }
    };
    /**
     *
     *  rct2: 0x006D348F
     */
    WindowBase* TrackManageOpen(TrackDesignFileRef* tdFileRef)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::manageTrackDesign);
        auto trackDesignManageWindow = std::make_unique<TrackDesignManageWindow>(tdFileRef);

        auto* window = windowMgr->Create(
            std::move(trackDesignManageWindow), WindowClass::manageTrackDesign, {}, kWindowSize,
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::centreScreen, WindowFlag::autoPosition });

        return window;
    }

    /**
     *
     *  rct2: 0x006D35CD
     */
    static void WindowTrackDeletePromptOpen(TrackDesignFileRef* tdFileRef)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::trackDeletePrompt);

        auto trackDeletePromptWindow = std::make_unique<TrackDeletePromptWindow>(tdFileRef);

        windowMgr->Create(
            std::move(trackDeletePromptWindow), WindowClass::trackDeletePrompt, {}, kWindowSizeDeletePrompt,
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::autoPosition, WindowFlag::centreScreen });
    }
} // namespace OpenRCT2::Ui::Windows
