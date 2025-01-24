/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    // clang-format off
    static constexpr Widget window_map_tooltip_widgets[] = {
        MakeWidget({0, 0}, {200, 30}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    };
    // clang-format on

    static ScreenCoordsXY _lastCursor;
    static int32_t _cursorHoldDuration;

    static void WindowMapTooltipOpen();

    static Formatter _mapTooltipArgs;

    class MapTooltip final : public Window
    {
    public:
        void OnOpen() override
        {
            SetWidgets(window_map_tooltip_widgets);
        }

        void OnUpdate() override
        {
            Invalidate();
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            StringId stringId;
            std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(StringId));
            if (stringId == kStringIdNone)
            {
                return;
            }

            auto stringCoords = windowPos + ScreenCoordsXY{ width / 2, height / 2 };
            DrawTextWrapped(dpi, stringCoords, width, STR_MAP_TOOLTIP_STRINGID, _mapTooltipArgs, { TextAlignment::CENTRE });
        }
    };

    void SetMapTooltip(Formatter& ft)
    {
        _mapTooltipArgs = ft;
    }

    const Formatter& GetMapTooltip()
    {
        return _mapTooltipArgs;
    }

    void WindowMapTooltipUpdateVisibility()
    {
        if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
        {
            // The map tooltip is drawn by the bottom toolbar
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::BottomToolbar);
            return;
        }

        const CursorState* state = ContextGetCursorState();
        auto cursor = state->position;
        auto cursorChange = cursor - _lastCursor;

        // Check for cursor movement
        _cursorHoldDuration++;
        if (abs(cursorChange.x) > 5 || abs(cursorChange.y) > 5 || (InputTestFlag(INPUT_FLAG_5))
            || InputGetState() == InputState::ViewportRight)
            _cursorHoldDuration = 0;

        _lastCursor = cursor;

        // Show or hide tooltip
        StringId stringId;
        std::memcpy(&stringId, _mapTooltipArgs.Data(), sizeof(StringId));

        auto& im = GetInputManager();
        auto* wm = GetWindowManager();
        if (_cursorHoldDuration < 25 || stringId == kStringIdNone || im.IsModifierKeyPressed(ModifierKey::ctrl)
            || im.IsModifierKeyPressed(ModifierKey::shift) || wm->FindByClass(WindowClass::Error) != nullptr)
        {
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::MapTooltip);
        }
        else
        {
            WindowMapTooltipOpen();
        }
    }

    static void WindowMapTooltipOpen()
    {
        constexpr int32_t width = 200;
        constexpr int32_t height = 44;
        const CursorState* state = ContextGetCursorState();
        auto pos = state->position + ScreenCoordsXY{ -width / 2, 15 };

        auto* windowMgr = GetWindowManager();
        if (auto w = windowMgr->FindByClass(WindowClass::MapTooltip))
        {
            w->Invalidate();
            w->windowPos = pos;
            w->width = width;
            w->height = height;
        }
        else
        {
            w = windowMgr->Create<MapTooltip>(
                WindowClass::MapTooltip, pos, width, height, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
        }
    }
} // namespace OpenRCT2::Ui::Windows
