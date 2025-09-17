/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/Formatter.h"
#include "Colour.h"
#include "ScrollArea.h"
#include "Window.h"

#include <list>
#include <memory>
#include <span>
#include <variant>
#include <vector>

enum class TileInspectorPage : int16_t;

struct ResearchItem;
struct RCTObjectEntry;

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

namespace OpenRCT2
{
    // TODO: move to Viewport.h?
    struct Focus
    {
        using CoordinateFocus = CoordsXYZ;
        using EntityFocus = EntityId;

        ZoomLevel zoom{};
        std::variant<CoordinateFocus, EntityFocus> data;

        template<typename T>
        constexpr explicit Focus(T newValue, ZoomLevel newZoom = {})
        {
            data = newValue;
            zoom = newZoom;
        }

        CoordsXYZ GetPos() const;

        constexpr bool operator==(const Focus& other) const
        {
            if (zoom != other.zoom)
            {
                return false;
            }
            return data == other.data;
        }
        constexpr bool operator!=(const Focus& other) const
        {
            return !(*this == other);
        }
    };

    /**
     * Window structure
     * size: 0x4C0
     */
    struct WindowBase
    {
        Viewport* viewport{};
        uint64_t disabled_widgets{};
        uint64_t pressed_widgets{};
        uint64_t hold_down_widgets{};
        std::vector<Widget> widgets{};
        ScreenCoordsXY windowPos;
        int16_t width{};
        int16_t height{};
        int16_t min_width{};
        int16_t max_width{};
        int16_t min_height{};
        int16_t max_height{};
        union
        {
            WindowNumber number{};
            RideId rideId;
        };
        WindowFlags flags{};
        OpenRCT2::ScrollArea scrolls[3];
        uint16_t no_list_items{};     // 0 for no items
        int16_t selected_list_item{}; // -1 for none selected
        std::optional<Focus> focus;
        union
        {
            int16_t page{};
            TileInspectorPage tileInspectorPage;
        };
        uint16_t frame_no{};              // updated every tic for motion in windows sprites
        uint16_t list_information_type{}; // 0 for none
        int16_t picked_peep_frame;        // Animation frame of picked peep in staff window and guest window
        int16_t selected_tab{};
        EntityId viewport_target_sprite{ EntityId::GetNull() };
        ScreenCoordsXY savedViewPos{};
        WindowClass classification{};
        ColourWithFlags colours[6]{};
        bool isVisible = true;
        EntityId viewport_smart_follow_sprite{ EntityId::GetNull() }; // Handles setting viewport target sprite etc

        void SetViewportLocation(const CoordsXYZ& coords);
        void Invalidate();
        void RemoveViewport();
        void SetWidgets(const std::span<const Widget> newWidgets);
        void ResizeFrame();

        int16_t getTitleBarTargetHeight() const;
        int16_t getTitleBarCurrentHeight() const;
        int16_t getTitleBarDiffTarget() const;
        int16_t getTitleBarDiffNormal() const;

        WindowBase() = default;
        WindowBase(WindowBase&) = delete;
        virtual ~WindowBase() = default;

        WindowBase& operator=(const WindowBase&) = delete;

        constexpr bool canBeResized() const
        {
            return (flags & WF_RESIZABLE) && (min_width != max_width || min_height != max_height);
        }

        // Events
        virtual void OnOpen()
        {
        }
        virtual bool CanClose()
        {
            return true;
        }
        virtual void OnClose()
        {
        }
        virtual void OnResize()
        {
        }
        virtual void OnUpdate()
        {
        }
        virtual void OnPeriodicUpdate()
        {
        }
        virtual void OnPrepareDraw()
        {
        }
        virtual void OnDraw(RenderTarget& rt)
        {
        }
        virtual void OnDrawWidget(WidgetIndex widgetIndex, RenderTarget& rt)
        {
        }
        virtual OpenRCT2String OnTooltip(WidgetIndex widgetIndex, StringId fallback)
        {
            return { fallback, {} };
        }
        virtual void OnMouseDown(WidgetIndex widgetIndex)
        {
        }
        virtual void OnMouseUp(WidgetIndex widgetIndex)
        {
        }
        virtual void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex)
        {
        }
        virtual void OnTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
        }
        virtual ScreenSize OnScrollGetSize(int32_t scrollIndex)
        {
            return {};
        }
        virtual void OnScrollSelect(int32_t scrollIndex, int32_t scrollAreaType)
        {
        }
        virtual void OnScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt)
        {
        }
        virtual void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void OnToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY&)
        {
        }
        virtual void OnToolAbort(WidgetIndex widgetIndex)
        {
        }
        virtual void OnViewportRotate()
        {
        }
        virtual void OnMoved(const ScreenCoordsXY&)
        {
        }
        virtual CursorID OnCursor(WidgetIndex, const ScreenCoordsXY&, CursorID);
        virtual void OnLanguageChange()
        {
        }
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

    // rct2: 0x01420078
    extern std::vector<std::unique_ptr<WindowBase>> g_window_list;
} // namespace OpenRCT2
