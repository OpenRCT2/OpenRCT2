/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Window.h"

#include <list>
#include <memory>

enum class TileInspectorPage : int16_t;

struct ResearchItem;
struct rct_object_entry;

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

/**
 * Window structure
 * size: 0x4C0
 */
struct rct_window
{
    WindowEventList* event_handlers{};
    rct_viewport* viewport{};
    uint64_t disabled_widgets{};
    uint64_t pressed_widgets{};
    uint64_t hold_down_widgets{};
    Widget* widgets{};
    ScreenCoordsXY windowPos;
    int16_t width{};
    int16_t height{};
    int16_t min_width{};
    int16_t max_width{};
    int16_t min_height{};
    int16_t max_height{};
    union
    {
        rct_windownumber number{};
        RideId rideId;
    };
    uint16_t flags{};
    rct_scroll scrolls[3];
    uint32_t list_item_positions[1024]{};
    uint16_t no_list_items{};     // 0 for no items
    int16_t selected_list_item{}; // -1 for none selected
    std::optional<Focus> focus;
    union
    {
        campaign_variables campaign;
        new_ride_variables new_ride;
        ride_variables ride;
        track_list_variables track_list;
        error_variables error;
        void* custom_info;
    };
    union
    {
        int16_t page;
        TileInspectorPage tileInspectorPage;
    };
    union
    {
        int16_t picked_peep_old_x; // staff window: peep x gets set to 0x8000 on pickup, this is the old value
        int16_t vehicleIndex;      // Ride window: selected car when setting vehicle colours
        int16_t var_48C;
    };
    uint16_t frame_no{};              // updated every tic for motion in windows sprites
    uint16_t list_information_type{}; // 0 for none
    union
    {
        int16_t picked_peep_frame; // Animation frame of picked peep in staff window and guest window
        int16_t var_492;
    };
    union
    {
        uint16_t ride_colour;
        const scenario_index_entry* highlighted_scenario;
    };
    int16_t selected_tab{};
    int16_t var_4AE{};
    EntityId viewport_target_sprite{ EntityId::GetNull() };
    ScreenCoordsXY savedViewPos{};
    WindowClass classification{};
    colour_t colours[6]{};
    VisibilityCache visibility{};
    EntityId viewport_smart_follow_sprite{ EntityId::GetNull() }; // Handles setting viewport target sprite etc

    void SetLocation(const CoordsXYZ& coords);
    void ScrollToViewport();
    void Invalidate();
    void RemoveViewport();

    rct_window() = default;
    rct_window(rct_window&) = delete;
    virtual ~rct_window() = default;

    rct_window& operator=(const rct_window&) = delete;

    virtual bool IsLegacy()
    {
        return true;
    }

    // Events
    virtual void OnOpen()
    {
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
    virtual void OnDraw(DrawPixelInfo& dpi)
    {
    }
    virtual void OnDrawWidget(WidgetIndex widgetIndex, DrawPixelInfo& dpi)
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
    virtual void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi)
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
    virtual void OnUnknown5()
    {
    }

    void ResizeFrame();
    void ResizeFrameWithPage();
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

// rct2: 0x01420078
extern std::list<std::shared_ptr<rct_window>> g_window_list;
