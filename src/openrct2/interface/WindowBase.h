/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/Formatter.h"
#include "ColourWithFlags.h"
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
    constexpr uint8_t kDefaultWindowFlashCountdown = 3;

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
        uint64_t disabledWidgets{};
        uint64_t pressedWidgets{};
        uint64_t holdDownWidgets{};
        std::vector<Widget> widgets{};
        ScreenCoordsXY windowPos;
        int16_t width{};
        int16_t height{};
        int16_t minWidth{};
        int16_t maxWidth{};
        int16_t minHeight{};
        int16_t maxHeight{};
        union
        {
            WindowNumber number{};
            RideId rideId;
        };
        WindowFlags flags{};
        /**
         * Set to 3 when the window should flash and decremented per tick.
         */
        uint8_t flashTimer{};
        ScrollArea scrolls[3];
        uint16_t numListItems{};    // 0 for no items
        int16_t selectedListItem{}; // -1 for none selected
        std::optional<Focus> focus;
        union
        {
            int16_t page{};
            TileInspectorPage tileInspectorPage;
        };
        uint16_t currentFrame{};        // updated every tic for motion in windows sprites
        uint16_t listInformationType{}; // 0 for none
        int16_t pickedPeepFrame;        // Animation frame of picked peep in staff window and guest window
        int16_t selectedTab{};
        EntityId viewportTargetSprite{ EntityId::GetNull() };
        ScreenCoordsXY savedViewPos{};
        WindowClass classification{};
        ColourWithFlags colours[6]{};
        bool isVisible = true;
        EntityId viewportSmartFollowSprite{ EntityId::GetNull() }; // Handles setting viewport target sprite etc

        void setViewportLocation(const CoordsXYZ& coords);
        void invalidate();
        void removeViewport();
        void setWidgets(std::span<const Widget> newWidgets);
        void resizeFrame();

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
            return flags.has(WindowFlag::resizable) && (minWidth != maxWidth || minHeight != maxHeight);
        }

        // Events
        virtual void onOpen()
        {
        }
        virtual bool canClose()
        {
            return true;
        }
        virtual void onClose()
        {
        }
        virtual void onResize()
        {
        }
        virtual void onUpdate()
        {
        }
        virtual void onPeriodicUpdate()
        {
        }
        virtual void onPrepareDraw()
        {
        }
        virtual void onDraw(Drawing::RenderTarget& rt)
        {
        }
        virtual void onDrawWidget(WidgetIndex widgetIndex, Drawing::RenderTarget& rt)
        {
        }
        virtual OpenRCT2String onTooltip(WidgetIndex widgetIndex, StringId fallback)
        {
            return { fallback, {} };
        }
        virtual void onMouseDown(WidgetIndex widgetIndex)
        {
        }
        virtual void onMouseUp(WidgetIndex widgetIndex)
        {
        }
        virtual void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex)
        {
        }
        virtual void onTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
        }
        virtual ScreenSize onScrollGetSize(int32_t scrollIndex)
        {
            return {};
        }
        virtual void onScrollSelect(int32_t scrollIndex, int32_t scrollAreaType)
        {
        }
        virtual void onScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onScrollDraw(int32_t scrollIndex, Drawing::RenderTarget& rt)
        {
        }
        virtual void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
        }
        virtual void onToolUp(WidgetIndex widgetIndex, const ScreenCoordsXY&)
        {
        }
        virtual void onToolAbort(WidgetIndex widgetIndex)
        {
        }
        virtual void onViewportRotate()
        {
        }
        virtual void onMoved(const ScreenCoordsXY&)
        {
        }
        virtual CursorID onCursor(WidgetIndex, const ScreenCoordsXY&, CursorID);
        virtual void onLanguageChange()
        {
        }

        constexpr int16_t right()
        {
            return windowPos.x + width;
        }

        constexpr int16_t bottom()
        {
            return windowPos.y + height;
        }

        constexpr void flash()
        {
            flashTimer = kDefaultWindowFlashCountdown;
        }
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

    // rct2: 0x01420078
    extern std::vector<std::unique_ptr<WindowBase>> gWindowList;
} // namespace OpenRCT2
