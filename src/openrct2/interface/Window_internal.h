/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    rct_window_event_list* event_handlers{};
    rct_viewport* viewport{};
    uint64_t disabled_widgets{};
    uint64_t pressed_widgets{};
    uint64_t hold_down_widgets{};
    rct_widget* widgets{};
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
        int16_t picked_peep_old_x; // staff/guest window: peep x gets set to 0x8000 on pickup, this is the old value
        int16_t vehicleIndex;      // Ride window: selected car when setting vehicle colours
        int16_t numberOfStaff;     // Used in park window.
        int16_t var_48C;
    };
    uint16_t frame_no{};              // updated every tic for motion in windows sprites
    uint16_t list_information_type{}; // 0 for none, Used as current position of marquee in window_peep
    union
    {
        int16_t picked_peep_frame; // Animation frame of picked peep in staff window and guest window
        int16_t var_492;
    };
    union
    {
        uint32_t highlighted_item;
        uint16_t ride_colour;
        ResearchItem* research_item;
        const scenario_index_entry* highlighted_scenario;
        uint16_t var_496;
    };
    int16_t selected_tab{};
    int16_t var_4AE{};
    EntityId viewport_target_sprite{ EntityId::GetNull() };
    ScreenCoordsXY savedViewPos{};
    rct_windowclass classification{};
    colour_t colours[6]{};
    VisibilityCache visibility{};
    EntityId viewport_smart_follow_sprite{ EntityId::GetNull() }; // Handles setting viewport target sprite etc

    void SetLocation(const CoordsXYZ& coords);
    void ScrollToViewport();
    void Invalidate();
    void RemoveViewport();

    rct_window() = default;
    virtual ~rct_window() = default;

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
    virtual void OnDraw(rct_drawpixelinfo& dpi)
    {
    }
    virtual void OnDrawWidget(rct_widgetindex widgetIndex, rct_drawpixelinfo& dpi)
    {
    }
    virtual OpenRCT2String OnTooltip(rct_widgetindex widgetIndex, rct_string_id fallback)
    {
        return { fallback, {} };
    }
    virtual void OnMouseDown(rct_widgetindex widgetIndex)
    {
    }
    virtual void OnMouseUp(rct_widgetindex widgetIndex)
    {
    }
    virtual void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex)
    {
    }
    virtual void OnTextInput(rct_widgetindex widgetIndex, std::string_view text)
    {
    }
    virtual ScreenSize OnScrollGetSize(int32_t scrollIndex)
    {
        return {};
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
    virtual void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi)
    {
    }
    virtual void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
    }
    virtual void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
    }
    virtual void OnToolDrag(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
    {
    }
    virtual void OnToolUp(rct_widgetindex, const ScreenCoordsXY&)
    {
    }
    virtual void OnToolAbort(rct_widgetindex widgetIndex)
    {
    }
    virtual void OnViewportRotate()
    {
    }
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

// rct2: 0x01420078
extern std::list<std::shared_ptr<rct_window>> g_window_list;
