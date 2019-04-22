/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Editor.h"

#include "Context.h"
#include "EditorObjectSelectionSession.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "audio/audio.h"
#include "interface/Viewport.h"
#include "localisation/Localisation.h"
#include "localisation/LocalisationService.h"
#include "management/NewsItem.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "peep/Staff.h"
#include "rct1/RCT1.h"
#include "scenario/Scenario.h"
#include "ui/UiContext.h"
#include "ui/WindowManager.h"
#include "util/Util.h"
#include "windows/Intent.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Park.h"
#include "world/Scenery.h"

#include <algorithm>
#include <array>
#include <vector>

using namespace OpenRCT2;

namespace Editor
{
    static std::array<std::vector<uint8_t>, OBJECT_TYPE_COUNT> _editorSelectedObjectFlags;

    static void ConvertSaveToScenarioCallback(int32_t result, const utf8* path);
    static void SetAllLandOwned();
    static bool LoadLandscapeFromSV4(const char* path);
    static bool LoadLandscapeFromSC4(const char* path);
    static void FinaliseMainView();
    static bool ReadS6(const char* path);
    static void ClearMapForEditing(bool fromSave);

    static void object_list_load()
    {
        // Scan objects if necessary
        auto context = GetContext();
        const auto& localisationService = context->GetLocalisationService();
        auto& objectRepository = context->GetObjectRepository();
        objectRepository.LoadOrConstruct(localisationService.GetCurrentLanguage());

        // Reset loaded objects to just defaults
        auto& objectManager = context->GetObjectManager();
        objectManager.UnloadAll();
        objectManager.LoadDefaultObjects();
    }

    /**
     *
     *  rct2: 0x0066FFE1
     */
    void Load()
    {
        audio_stop_all_music_and_sounds();
        object_manager_unload_all_objects();
        object_list_load();
        OpenRCT2::GetContext()->GetGameState()->InitAll(150);
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gS6Info.category = SCENARIO_CATEGORY_OTHER;
        viewport_init_all();
        rct_window* mainWindow = context_open_window_view(WV_EDITOR_MAIN);
        window_set_location(mainWindow, 2400, 2400, 112);
        load_palette();
        gScreenAge = 0;
        gScenarioName = language_get_string(STR_MY_NEW_SCENARIO);
    }

    /**
     *
     *  rct2: 0x00672781
     */
    void ConvertSaveToScenario()
    {
        tool_cancel();
        auto intent = Intent(WC_LOADSAVE);
        intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
        intent.putExtra(INTENT_EXTRA_CALLBACK, (void*)ConvertSaveToScenarioCallback);
        context_open_intent(&intent);
    }

    static void ConvertSaveToScenarioCallback(int32_t result, const utf8* path)
    {
        if (result != MODAL_RESULT_OK)
        {
            return;
        }

        if (!context_load_park_from_file(path))
        {
            return;
        }

        if (gParkFlags & PARK_FLAGS_NO_MONEY)
        {
            gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
        }
        else
        {
            gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
        }
        gParkFlags |= PARK_FLAGS_NO_MONEY;

        safe_strcpy(gS6Info.name, gScenarioName.c_str(), sizeof(gS6Info.name));
        safe_strcpy(gS6Info.details, gScenarioDetails.c_str(), sizeof(gS6Info.details));
        gS6Info.objective_type = gScenarioObjectiveType;
        gS6Info.objective_arg_1 = gScenarioObjectiveYear;
        gS6Info.objective_arg_2 = gScenarioObjectiveCurrency;
        gS6Info.objective_arg_3 = gScenarioObjectiveNumGuests;
        climate_reset(gClimate);

        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        gS6Info.editor_step = EDITOR_STEP_OBJECTIVE_SELECTION;
        gS6Info.category = SCENARIO_CATEGORY_OTHER;
        viewport_init_all();
        news_item_init_queue();
        context_open_window_view(WV_EDITOR_MAIN);
        FinaliseMainView();
        gScreenAge = 0;
    }

    /**
     *
     *  rct2: 0x00672957
     */
    void LoadTrackDesigner()
    {
        audio_stop_all_music_and_sounds();
        gScreenFlags = SCREEN_FLAGS_TRACK_DESIGNER;
        gScreenAge = 0;

        object_manager_unload_all_objects();
        object_list_load();
        OpenRCT2::GetContext()->GetGameState()->InitAll(150);
        SetAllLandOwned();
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        viewport_init_all();
        rct_window* mainWindow = context_open_window_view(WV_EDITOR_MAIN);
        window_set_location(mainWindow, 2400, 2400, 112);
        load_palette();
    }

    /**
     *
     *  rct2: 0x006729FD
     */
    void LoadTrackManager()
    {
        audio_stop_all_music_and_sounds();
        gScreenFlags = SCREEN_FLAGS_TRACK_MANAGER;
        gScreenAge = 0;

        object_manager_unload_all_objects();
        object_list_load();
        OpenRCT2::GetContext()->GetGameState()->InitAll(150);
        SetAllLandOwned();
        gS6Info.editor_step = EDITOR_STEP_OBJECT_SELECTION;
        viewport_init_all();
        rct_window* mainWindow = context_open_window_view(WV_EDITOR_MAIN);
        window_set_location(mainWindow, 2400, 2400, 112);
        load_palette();
    }

    /**
     *
     *  rct2: 0x0068ABEC
     */
    static void SetAllLandOwned()
    {
        int32_t mapSize = gMapSize;

        game_do_command(64, 1, 64, 2, GAME_COMMAND_SET_LAND_OWNERSHIP, (mapSize - 3) * 32, (mapSize - 3) * 32);
    }

    /**
     *
     *  rct2: 0x006758C0
     */
    bool LoadLandscape(const utf8* path)
    {
        // #4996: Make sure the object selection window closes here to prevent unload objects
        //        after we have loaded a new park.
        window_close_all();

        uint32_t extension = get_file_extension_type(path);
        switch (extension)
        {
            case FILE_EXTENSION_SC6:
            case FILE_EXTENSION_SV6:
                return ReadS6(path);
            case FILE_EXTENSION_SC4:
                return LoadLandscapeFromSC4(path);
            case FILE_EXTENSION_SV4:
                return LoadLandscapeFromSV4(path);
            default:
                return false;
        }
    }

    /**
     *
     *  rct2: 0x006A2B02
     */
    static bool LoadLandscapeFromSV4(const char* path)
    {
        load_from_sv4(path);
        ClearMapForEditing(true);

        gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
        gScreenAge = 0;
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        viewport_init_all();
        context_open_window_view(WV_EDITOR_MAIN);
        FinaliseMainView();
        return true;
    }

    static bool LoadLandscapeFromSC4(const char* path)
    {
        load_from_sc4(path);
        ClearMapForEditing(false);

        gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
        gScreenAge = 0;
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        viewport_init_all();
        context_open_window_view(WV_EDITOR_MAIN);
        FinaliseMainView();
        return true;
    }

    /**
     *
     *  rct2: 0x006758FE
     */
    static bool ReadS6(const char* path)
    {
        auto extension = path_get_extension(path);
        if (_stricmp(extension, ".sc6") == 0)
        {
            load_from_sc6(path);
        }
        else if (_stricmp(extension, ".sv6") == 0 || _stricmp(extension, ".sv7") == 0)
        {
            load_from_sv6(path);
        }

        ClearMapForEditing(true);

        gS6Info.editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
        gScreenAge = 0;
        gScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR;
        viewport_init_all();
        context_open_window_view(WV_EDITOR_MAIN);
        FinaliseMainView();
        return true;
    }

    static void ClearMapForEditing(bool fromSave)
    {
        map_remove_all_rides();

        //
        for (auto& banner : gBanners)
        {
            if (banner.type == 255)
            {
                banner.flags &= ~BANNER_FLAG_LINKED_TO_RIDE;
            }
        }

        //
        {
            int32_t i;
            Ride* ride;
            FOR_ALL_RIDES (i, ride)
            {
                user_string_free(ride->name);
            }
        }

        ride_init_all();

        //
        for (int32_t i = 0; i < MAX_SPRITES; i++)
        {
            auto peep = get_sprite(i)->AsPeep();
            if (peep != nullptr)
            {
                user_string_free(peep->name_string_idx);
            }
        }

        reset_sprite_list();
        staff_reset_modes();
        gNumGuestsInPark = 0;
        gNumGuestsHeadingForPark = 0;
        gNumGuestsInParkLastWeek = 0;
        gGuestChangeModifier = 0;
        if (fromSave)
        {
            research_populate_list_random();

            if (gParkFlags & PARK_FLAGS_NO_MONEY)
            {
                gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
            }
            else
            {
                gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
            }
            gParkFlags |= PARK_FLAGS_NO_MONEY;

            if (gParkEntranceFee == 0)
            {
                gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
            }
            else
            {
                gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
            }

            gParkFlags &= ~PARK_FLAGS_SPRITES_INITIALISED;

            gGuestInitialCash = std::clamp(gGuestInitialCash, (money16)MONEY(10, 00), (money16)MAX_ENTRANCE_FEE);

            gInitialCash = std::min(gInitialCash, 100000);
            finance_reset_cash_to_initial();

            gBankLoan = std::clamp(gBankLoan, MONEY(0, 00), MONEY(5000000, 00));

            gMaxBankLoan = std::clamp(gMaxBankLoan, MONEY(0, 00), MONEY(5000000, 00));

            gBankLoanInterestRate = std::clamp<uint8_t>(gBankLoanInterestRate, 5, 80);
        }

        climate_reset(gClimate);

        news_item_init_queue();
    }

    /**
     *
     *  rct2: 0x0067009A
     */
    void OpenWindowsForCurrentStep()
    {
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        {
            return;
        }

        switch (gS6Info.editor_step)
        {
            case EDITOR_STEP_OBJECT_SELECTION:
                if (window_find_by_class(WC_EDITOR_OBJECT_SELECTION))
                {
                    return;
                }

                if (window_find_by_class(WC_INSTALL_TRACK))
                {
                    return;
                }

                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    object_manager_unload_all_objects();
                }

                context_open_window(WC_EDITOR_OBJECT_SELECTION);
                break;
            case EDITOR_STEP_INVENTIONS_LIST_SET_UP:
                if (window_find_by_class(WC_EDITOR_INVENTION_LIST))
                {
                    return;
                }

                context_open_window(WC_EDITOR_INVENTION_LIST);
                break;
            case EDITOR_STEP_OPTIONS_SELECTION:
                if (window_find_by_class(WC_EDITOR_SCENARIO_OPTIONS))
                {
                    return;
                }

                context_open_window(WC_EDITOR_SCENARIO_OPTIONS);
                break;
            case EDITOR_STEP_OBJECTIVE_SELECTION:
                if (window_find_by_class(WC_EDTIOR_OBJECTIVE_OPTIONS))
                {
                    return;
                }

                context_open_window(WC_EDTIOR_OBJECTIVE_OPTIONS);
                break;
        }
    }

    static void FinaliseMainView()
    {
        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        windowManager->SetMainView(gSavedViewX, gSavedViewY, gSavedViewZoom, gSavedViewRotation);

        reset_all_sprite_quadrant_placements();
        scenery_set_default_placement_configuration();

        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_NEW_RIDES));

        gWindowUpdateTicks = 0;
        load_palette();

        windowManager->BroadcastIntent(Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD));
    }

    /**
     *
     *  rct2: 0x006AB9B8
     */
    int32_t CheckObjectSelection()
    {
        bool isTrackDesignerManager = gScreenFlags & (SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER);

        if (!isTrackDesignerManager)
        {
            if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PATHS))
            {
                gGameCommandErrorText = STR_AT_LEAST_ONE_PATH_OBJECT_MUST_BE_SELECTED;
                return OBJECT_TYPE_PATHS;
            }
        }

        if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_RIDE))
        {
            gGameCommandErrorText = STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED;
            return OBJECT_TYPE_RIDE;
        }

        if (!isTrackDesignerManager)
        {
            if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_PARK_ENTRANCE))
            {
                gGameCommandErrorText = STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED;
                return OBJECT_TYPE_PARK_ENTRANCE;
            }

            if (!editor_check_object_group_at_least_one_selected(OBJECT_TYPE_WATER))
            {
                gGameCommandErrorText = STR_WATER_TYPE_MUST_BE_SELECTED;
                return OBJECT_TYPE_WATER;
            }
        }

        return -1;
    }

    /**
     *
     *  rct2: 0x0066FEAC
     */
    bool CheckPark()
    {
        int32_t parkSize = park_calculate_size();
        if (parkSize == 0)
        {
            gGameCommandErrorText = STR_PARK_MUST_OWN_SOME_LAND;
            return false;
        }

        if (gParkEntrances.empty())
        {
            gGameCommandErrorText = STR_NO_PARK_ENTRANCES;
            return false;
        }

        for (const auto& parkEntrance : gParkEntrances)
        {
            int32_t x = parkEntrance.x;
            int32_t y = parkEntrance.y;
            int32_t z = parkEntrance.z / 8;
            int32_t direction = direction_reverse(parkEntrance.direction);

            switch (footpath_is_connected_to_map_edge(x, y, z, direction, 0))
            {
                case FOOTPATH_SEARCH_NOT_FOUND:
                    gGameCommandErrorText = STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH;
                    return false;
                case FOOTPATH_SEARCH_INCOMPLETE:
                case FOOTPATH_SEARCH_TOO_COMPLEX:
                    gGameCommandErrorText = STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX;
                    return false;
                case FOOTPATH_SEARCH_SUCCESS:
                    // Run the search again and unown the path
                    footpath_is_connected_to_map_edge(x, y, z, direction, (1 << 5));
                    break;
            }
        }

        if (gPeepSpawns.empty())
        {
            gGameCommandErrorText = STR_PEEP_SPAWNS_NOT_SET;
            return false;
        }

        return true;
    }

    void GameCommandEditScenarioOptions(
        [[maybe_unused]] int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi,
        [[maybe_unused]] int32_t* edi, [[maybe_unused]] int32_t* ebp)
    {
        if (!(*ebx & GAME_COMMAND_FLAG_APPLY))
        {
            *ebx = 0;
            return;
        }

        switch (*ecx)
        {
            case EDIT_SCENARIOOPTIONS_SETNOMONEY:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (*edx != 0)
                    {
                        gParkFlags |= PARK_FLAGS_NO_MONEY_SCENARIO;
                    }
                    else
                    {
                        gParkFlags &= ~PARK_FLAGS_NO_MONEY_SCENARIO;
                    }
                }
                else
                {
                    if (*edx != 0)
                    {
                        gParkFlags |= PARK_FLAGS_NO_MONEY;
                    }
                    else
                    {
                        gParkFlags &= ~PARK_FLAGS_NO_MONEY;
                    }
                    // Invalidate all windows that have anything to do with finance
                    window_invalidate_by_class(WC_RIDE);
                    window_invalidate_by_class(WC_PEEP);
                    window_invalidate_by_class(WC_PARK_INFORMATION);
                    window_invalidate_by_class(WC_FINANCES);
                    window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
                    window_invalidate_by_class(WC_TOP_TOOLBAR);
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETINITIALCASH:
                gInitialCash = std::clamp(*edx, MONEY(0, 00), MONEY(1000000, 00));
                gCash = gInitialCash;
                window_invalidate_by_class(WC_FINANCES);
                window_invalidate_by_class(WC_BOTTOM_TOOLBAR);
                break;
            case EDIT_SCENARIOOPTIONS_SETINITIALLOAN:
                gBankLoan = std::clamp(*edx, MONEY(0, 00), MONEY(5000000, 00));
                gMaxBankLoan = std::max(gBankLoan, gMaxBankLoan);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case EDIT_SCENARIOOPTIONS_SETMAXIMUMLOANSIZE:
                gMaxBankLoan = std::clamp(*edx, MONEY(0, 00), MONEY(5000000, 00));
                gBankLoan = std::min(gBankLoan, gMaxBankLoan);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case EDIT_SCENARIOOPTIONS_SETANNUALINTERESTRATE:
                gBankLoanInterestRate = std::clamp(*edx, 0, 80);
                window_invalidate_by_class(WC_FINANCES);
                break;
            case EDIT_SCENARIOOPTIONS_SETFORBIDMARKETINGCAMPAIGNS:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_MARKETING_CAMPAIGN;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETAVERAGECASHPERGUEST:
                gGuestInitialCash = std::clamp(*edx, MONEY(0, 00), MONEY(1000, 00));
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTINITIALHAPPINESS:
                gGuestInitialHappiness = std::clamp(*edx, 40, 250);
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTINITIALHUNGER:
                gGuestInitialHunger = std::clamp(*edx, 40, 250);
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTINITIALTHIRST:
                gGuestInitialThirst = std::clamp(*edx, 40, 250);
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTSPREFERLESSINTENSERIDES:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_PREF_LESS_INTENSE_RIDES;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTSPREFERMOREINTENSERIDES:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_PREF_MORE_INTENSE_RIDES;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETCOSTTOBUYLAND:
                gLandPrice = std::clamp(*edx, MONEY(5, 00), MONEY(200, 00));
                break;
            case EDIT_SCENARIOOPTIONS_SETCOSTTOBUYCONSTRUCTIONRIGHTS:
                gConstructionRightsPrice = std::clamp(*edx, MONEY(5, 00), MONEY(200, 00));
                break;
            case EDIT_SCENARIOOPTIONS_SETPARKCHARGEMETHOD:
                if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
                {
                    if (*edx == 0)
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(0, 00);
                    }
                    else if (*edx == 1)
                    {
                        gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(10, 00);
                    }
                    else
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                        gParkEntranceFee = MONEY(10, 00);
                    }
                }
                else
                {
                    if (*edx == 0)
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else if (*edx == 1)
                    {
                        gParkFlags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags &= ~PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    else
                    {
                        gParkFlags |= PARK_FLAGS_PARK_FREE_ENTRY;
                        gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
                    }
                    window_invalidate_by_class(WC_PARK_INFORMATION);
                    window_invalidate_by_class(WC_RIDE);
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETPARKCHARGEENTRYFEE:
                gParkEntranceFee = std::clamp(*edx, MONEY(0, 00), MAX_ENTRANCE_FEE);
                window_invalidate_by_class(WC_PARK_INFORMATION);
                break;
            case EDIT_SCENARIOOPTIONS_SETFORBIDTREEREMOVAL:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_TREE_REMOVAL;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETFORBIDLANDSCAPECHANGES:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_LANDSCAPE_CHANGES;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETFORBIDHIGHCONSTRUCTION:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETPARKRATINGHIGHERDIFFICULTLEVEL:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_DIFFICULT_PARK_RATING;
                }
                break;
            case EDIT_SCENARIOOPTIONS_SETGUESTGENERATIONHIGHERDIFFICULTLEVEL:
                if (*edx != 0)
                {
                    gParkFlags |= PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                else
                {
                    gParkFlags &= ~PARK_FLAGS_DIFFICULT_GUEST_GENERATION;
                }
                break;
        }
        window_invalidate_by_class(WC_EDITOR_SCENARIO_OPTIONS);
        *ebx = 0;
    }

    uint8_t GetSelectedObjectFlags(int32_t objectType, size_t index)
    {
        uint8_t result = 0;
        auto& list = _editorSelectedObjectFlags[objectType];
        if (list.size() > index)
        {
            result = list[index];
        }
        return result;
    }

    void ClearSelectedObject(int32_t objectType, size_t index, uint32_t flags)
    {
        auto& list = _editorSelectedObjectFlags[objectType];
        if (list.size() <= index)
        {
            list.resize(index + 1);
        }
        list[index] &= ~flags;
    }

    void SetSelectedObject(int32_t objectType, size_t index, uint32_t flags)
    {
        auto& list = _editorSelectedObjectFlags[objectType];
        if (list.size() <= index)
        {
            list.resize(index + 1);
        }
        list[index] |= flags;
    }
} // namespace Editor

void editor_open_windows_for_current_step()
{
    Editor::OpenWindowsForCurrentStep();
}

void game_command_edit_scenario_options(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
    Editor::GameCommandEditScenarioOptions(eax, ebx, ecx, edx, esi, edi, ebp);
}
