/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WindowManager.h"

#include "input/Input.h"
#include "input/KeyboardShortcuts.h"
#include "interface/Theme.h"
#include "windows/Window.h"

#include <openrct2-ui/windows/Window.h>
#include <openrct2/Input.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/interface/Viewport.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Sprite.h>

using namespace OpenRCT2::Ui;

class WindowManager final : public IWindowManager
{
public:
    void Init() override
    {
        theme_manager_initialise();
        window_guest_list_init_vars();
        window_new_ride_init_vars();
    }

    rct_window* OpenWindow(rct_windowclass wc) override
    {
        switch (wc)
        {
            case WC_ABOUT:
                return window_about_open();
            case WC_BOTTOM_TOOLBAR:
                return window_game_bottom_toolbar_open();
            case WC_CHANGELOG:
                return window_changelog_open();
            case WC_CHEATS:
                return window_cheats_open();
            case WC_CLEAR_SCENERY:
                return window_clear_scenery_open();
            case WC_CUSTOM_CURRENCY_CONFIG:
                return custom_currency_window_open();
            case WC_DEBUG_PAINT:
                return window_debug_paint_open();
            case WC_EDITOR_INVENTION_LIST:
                return window_editor_inventions_list_open();
            case WC_EDITOR_OBJECT_SELECTION:
                return window_editor_object_selection_open();
            case WC_EDTIOR_OBJECTIVE_OPTIONS:
                return window_editor_objective_options_open();
            case WC_EDITOR_SCENARIO_OPTIONS:
                return window_editor_scenario_options_open();
            case WC_FINANCES:
                return window_finances_open();
            case WC_FOOTPATH:
                return window_footpath_open();
            case WC_GUEST_LIST:
                return window_guest_list_open();
            case WC_LAND:
                return window_land_open();
            case WC_LAND_RIGHTS:
                return window_land_rights_open();
            case WC_MAIN_WINDOW:
                return window_main_open();
            case WC_MAP:
                return window_map_open();
            case WC_MAPGEN:
                return window_mapgen_open();
            case WC_MULTIPLAYER:
                return window_multiplayer_open();
            case WC_MUSIC_CREDITS:
                return window_music_credits_open();
            case WC_CONSTRUCT_RIDE:
                return window_new_ride_open();
            case WC_PARK_INFORMATION:
                return window_park_entrance_open();
            case WC_RECENT_NEWS:
                return window_news_open();
            case WC_RIDE_CONSTRUCTION:
                return window_ride_construction_open();
            case WC_RESEARCH:
                return window_research_open();
            case WC_RIDE_LIST:
                return window_ride_list_open();
            case WC_NOTIFICATION_OPTIONS:
                return window_news_options_open();
            case WC_OPTIONS:
                return window_options_open();
            case WC_SAVE_PROMPT:
                return window_save_prompt_open();
            case WC_SCENERY:
                return window_scenery_open();
            case WC_SERVER_LIST:
                return window_server_list_open();
            case WC_SERVER_START:
                return window_server_start_open();
            case WC_KEYBOARD_SHORTCUT_LIST:
                return window_shortcut_keys_open();
            case WC_STAFF_LIST:
                return window_staff_list_open();
            case WC_THEMES:
                return window_themes_open();
            case WC_TILE_INSPECTOR:
                return window_tile_inspector_open();
            case WC_TITLE_EXIT:
                return window_title_exit_open();
            case WC_TITLE_LOGO:
                return window_title_logo_open();
            case WC_TITLE_MENU:
                return window_title_menu_open();
            case WC_TITLE_OPTIONS:
                return window_title_options_open();
            case WC_TOP_TOOLBAR:
                return window_top_toolbar_open();
            case WC_VIEW_CLIPPING:
                return window_view_clipping_open();
            case WC_VIEWPORT:
                return window_viewport_open();
            case WC_WATER:
                return window_water_open();
            case WC_NETWORK:
                return window_network_open();
            default:
                Console::Error::WriteLine("Unhandled window class (%d)", wc);
                return nullptr;
        }
    }

    rct_window* OpenView(uint8_t view) override
    {
        switch (view)
        {
            case WV_PARK_AWARDS:
                return window_park_awards_open();
            case WV_PARK_RATING:
                return window_park_rating_open();
            case WV_PARK_OBJECTIVE:
                return window_park_objective_open();
            case WV_PARK_GUESTS:
                return window_park_guests_open();
            case WV_FINANCES_RESEARCH:
                return window_finances_research_open();
            case WV_RIDE_RESEARCH:
                if (gConfigInterface.toolbar_show_research)
                {
                    return this->OpenWindow(WC_RESEARCH);
                }
                else
                {
                    return window_new_ride_open_research();
                }
            case WV_MAZE_CONSTRUCTION:
                return window_maze_construction_open();
            case WV_NETWORK_PASSWORD:
                return window_network_status_open_password();
            case WV_EDITOR_BOTTOM_TOOLBAR:
                return window_editor_bottom_toolbar_open();
            case WV_EDITOR_MAIN:
                return window_editor_main_open();
            default:
                return nullptr;
        }
    }

    rct_window* OpenDetails(uint8_t type, int32_t id) override
    {
        switch (type)
        {
            case WD_BANNER:
                return window_banner_open(id);
            case WD_DEMOLISH_RIDE:
                return window_ride_demolish_prompt_open(id);
            case WD_REFURBISH_RIDE:
                return window_ride_refurbish_prompt_open(id);
            case WD_NEW_CAMPAIGN:
                return window_new_campaign_open(id);
            case WD_SIGN:
                return window_sign_open(id);
            case WD_SIGN_SMALL:
                return window_sign_small_open(id);

            case WD_PLAYER:
                return window_player_open(id);

            default:
                return nullptr;
        }
    }

    rct_window* ShowError(rct_string_id title, rct_string_id message) override
    {
        return window_error_open(title, message);
    }

    rct_window* OpenIntent(Intent* intent) override
    {
        switch (intent->GetWindowClass())
        {
            case WC_PEEP:
                return window_guest_open((rct_peep*)intent->GetPointerExtra(INTENT_EXTRA_PEEP));
            case WC_FIRE_PROMPT:
                return window_staff_fire_prompt_open((rct_peep*)intent->GetPointerExtra(INTENT_EXTRA_PEEP));
            case WC_INSTALL_TRACK:
                return window_install_track_open(intent->GetStringExtra(INTENT_EXTRA_PATH).c_str());
            case WC_GUEST_LIST:
                return window_guest_list_open_with_filter(
                    intent->GetSIntExtra(INTENT_EXTRA_GUEST_LIST_FILTER), intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WC_LOADSAVE:
            {
                uint32_t type = intent->GetUIntExtra(INTENT_EXTRA_LOADSAVE_TYPE);
                std::string defaultName = intent->GetStringExtra(INTENT_EXTRA_PATH);
                loadsave_callback callback = (loadsave_callback)intent->GetPointerExtra(INTENT_EXTRA_CALLBACK);
                rct_window* w = window_loadsave_open(type, defaultName.c_str(), callback);

                return w;
            }
            case WC_MANAGE_TRACK_DESIGN:
                return window_track_manage_open((track_design_file_ref*)intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
            case WC_NETWORK_STATUS:
            {
                std::string message = intent->GetStringExtra(INTENT_EXTRA_MESSAGE);
                close_callback callback = intent->GetCloseCallbackExtra(INTENT_EXTRA_CALLBACK);
                return window_network_status_open(message.c_str(), callback);
            }
            case WC_OBJECT_LOAD_ERROR:
            {
                std::string path = intent->GetStringExtra(INTENT_EXTRA_PATH);
                const rct_object_entry* objects = (rct_object_entry*)intent->GetPointerExtra(INTENT_EXTRA_LIST);
                size_t count = intent->GetUIntExtra(INTENT_EXTRA_LIST_COUNT);
                window_object_load_error_open(const_cast<utf8*>(path.c_str()), count, objects);

                return nullptr;
            }
            case WC_RIDE:
                return window_ride_main_open(intent->GetSIntExtra(INTENT_EXTRA_RIDE_ID));
            case WC_TRACK_DESIGN_PLACE:
                return window_track_place_open((track_design_file_ref*)intent->GetPointerExtra(INTENT_EXTRA_TRACK_DESIGN));
            case WC_TRACK_DESIGN_LIST:
            {
                ride_list_item rideItem;
                rideItem.type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.entry_index = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                return window_track_list_open(rideItem);
            }
            case WC_SCENARIO_SELECT:
                return window_scenarioselect_open(
                    (scenarioselect_callback)intent->GetPointerExtra(INTENT_EXTRA_CALLBACK), false);
            case WD_VEHICLE:
                return window_ride_open_vehicle((rct_vehicle*)intent->GetPointerExtra(INTENT_EXTRA_VEHICLE));
            case WD_TRACK:
                return window_ride_open_track((TileElement*)intent->GetPointerExtra(INTENT_EXTRA_TILE_ELEMENT));
            case INTENT_ACTION_NEW_RIDE_OF_TYPE:
            {
                // Open ride list window
                auto w = window_new_ride_open();

                // Switch to right tab and scroll to ride location
                ride_list_item rideItem;
                rideItem.type = intent->GetUIntExtra(INTENT_EXTRA_RIDE_TYPE);
                rideItem.entry_index = intent->GetUIntExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX);
                window_new_ride_focus(rideItem);

                return w;
            }
            default:
                Console::Error::WriteLine("Unhandled window class for intent (%d)", intent->GetWindowClass());
                return nullptr;
        }
    }

    void BroadcastIntent(const Intent& intent) override
    {
        switch (intent.GetWindowClass())
        {
            case INTENT_ACTION_MAP:
                window_map_reset();
                break;

            case INTENT_ACTION_REFRESH_NEW_RIDES:
                window_new_ride_init_vars();
                break;

            case INTENT_ACTION_REFRESH_RIDE_LIST:
            {
                auto window = window_find_by_class(WC_RIDE_LIST);
                if (window != nullptr)
                {
                    window_ride_list_refresh_list(window);
                }

                break;
            }

            case INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION:
                window_maze_construction_update_pressed_widgets();
                break;

            case INTENT_ACTION_RIDE_CONSTRUCTION_FOCUS:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
                if (w == nullptr || w->number != rideIndex)
                {
                    window_close_construction_windows();
                    _currentRideIndex = rideIndex;
                    w = OpenWindow(WC_RIDE_CONSTRUCTION);
                }
                else
                {
                    ride_construction_invalidate_current_track();
                    _currentRideIndex = rideIndex;
                }
                break;
            }

            case INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES:
                window_ride_construction_update_enabled_track_pieces();
                break;

            case INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS:
                window_ride_construction_update_active_elements_impl();
                break;

            case INTENT_ACTION_INIT_SCENERY:
                window_scenery_init();
                break;

            case INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG:
                window_scenery_set_default_placement_configuration();
                break;

            case INTENT_ACTION_REFRESH_SCENERY:
                window_scenery_reset_selected_scenery_items();
                break;

            case INTENT_ACTION_INVALIDATE_TICKER_NEWS:
                window_game_bottom_toolbar_invalidate_news_item();
                break;

            case INTENT_ACTION_REFRESH_GUEST_LIST:
                window_guest_list_refresh_list();
                break;

            case INTENT_ACTION_REFRESH_STAFF_LIST:
            {
                auto w = window_find_by_class(WC_STAFF_LIST);
                if (w != nullptr)
                {
                    w->no_list_items = 0;
                }
                break;
            }

            case INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD:
                window_tile_inspector_clear_clipboard();
                break;

            case INTENT_ACTION_INVALIDATE_VEHICLE_WINDOW:
            {
                rct_vehicle* vehicle = static_cast<rct_vehicle*>(intent.GetPointerExtra(INTENT_EXTRA_VEHICLE));
                int32_t viewVehicleIndex;
                Ride* ride;
                rct_window* w;

                w = window_find_by_number(WC_RIDE, vehicle->ride);
                if (w == nullptr)
                    return;

                ride = get_ride(vehicle->ride);
                viewVehicleIndex = w->ride.view - 1;
                if (viewVehicleIndex < 0 || viewVehicleIndex >= ride->num_vehicles)
                    return;

                if (vehicle->sprite_index != ride->vehicles[viewVehicleIndex])
                    return;

                window_invalidate(w);
                break;
            }

            case INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE:
            {
                auto rideIndex = intent.GetUIntExtra(INTENT_EXTRA_RIDE_ID);
                auto w = window_find_by_number(WC_RIDE, rideIndex);
                if (w != nullptr)
                {
                    if (w->page == 4) // WINDOW_RIDE_PAGE_COLOUR
                    {
                        w->vehicleIndex = 0;
                    }
                    window_invalidate(w);
                }
                break;
            }

            case INTENT_ACTION_UPDATE_CLIMATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_CLIMATE;
                window_invalidate_by_class(WC_GUEST_LIST);
                break;

            case INTENT_ACTION_UPDATE_PARK_RATING:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_PARK_RATING;
                window_invalidate_by_class(WC_PARK_INFORMATION);
                break;

            case INTENT_ACTION_UPDATE_DATE:
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_DATE;
                break;

            case INTENT_ACTION_UPDATE_CASH:
                window_invalidate_by_class(WC_FINANCES);
                gToolbarDirtyFlags |= BTM_TB_DIRTY_FLAG_MONEY;
                break;

            case INTENT_ACTION_UPDATE_BANNER:
            {
                uint8_t bannerIndex = static_cast<uint8_t>(intent.GetUIntExtra(INTENT_EXTRA_BANNER_INDEX));

                rct_window* w = window_find_by_number(WC_BANNER, bannerIndex);
                if (w != nullptr)
                {
                    window_invalidate(w);
                }
                break;
            }
            case INTENT_ACTION_UPDATE_RESEARCH:
                window_invalidate_by_class(WC_FINANCES);
                window_invalidate_by_class(WC_RESEARCH);
                break;
        }
    }

    void ForceClose(rct_windowclass windowClass) override
    {
        switch (windowClass)
        {
            case WC_NETWORK_STATUS:
                window_network_status_close();
                break;

            default:
                break;
        }
    }

    void UpdateMapTooltip() override
    {
        window_map_tooltip_update_visibility();
    }

    void HandleInput() override
    {
        game_handle_input();
    }

    void HandleKeyboard(bool isTitle) override
    {
        input_handle_keyboard(isTitle);
    }

    std::string GetKeyboardShortcutString(int32_t shortcut) override
    {
        utf8 buffer[256];
        keyboard_shortcuts_format_string(buffer, sizeof(buffer), shortcut);
        return std::string(buffer);
    }

    void SetMainView(int32_t x, int32_t y, int32_t zoom, int32_t rotation) override
    {
        auto mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            auto viewport = window_get_viewport(mainWindow);
            auto zoomDifference = zoom - viewport->zoom;

            mainWindow->viewport_target_sprite = SPRITE_INDEX_NULL;
            mainWindow->saved_view_x = x;
            mainWindow->saved_view_y = y;
            viewport->zoom = zoom;
            gCurrentRotation = rotation;

            if (zoomDifference != 0)
            {
                viewport->view_width <<= zoomDifference;
                viewport->view_height <<= zoomDifference;
            }
            mainWindow->saved_view_x -= viewport->view_width >> 1;
            mainWindow->saved_view_y -= viewport->view_height >> 1;

            // Make sure the viewport has correct coordinates set.
            viewport_update_position(mainWindow);

            window_invalidate(mainWindow);
        }
    }

    void UpdateMouseWheel() override
    {
        window_all_wheel_input();
    }

    rct_window* GetOwner(const rct_viewport* viewport) override
    {
        for (auto& w : g_window_list)
        {
            if (w->viewport == viewport)
            {
                return w.get();
            }
        }
        return nullptr;
    }
};

IWindowManager* OpenRCT2::Ui::CreateWindowManager()
{
    return new WindowManager();
}
