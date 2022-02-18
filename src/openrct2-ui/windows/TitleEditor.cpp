/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../title/TitleSequencePlayer.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/sprites.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequence.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>

using namespace OpenRCT2;

enum WindowTitleEditorTab
{
    WINDOW_TITLE_EDITOR_TAB_PRESETS,
    WINDOW_TITLE_EDITOR_TAB_SAVES,
    WINDOW_TITLE_EDITOR_TAB_SCRIPT,
    WINDOW_TITLE_EDITOR_TAB_COUNT
};

static void WindowTitleEditorClose(rct_window* w);
static void WindowTitleEditorMouseup(rct_window* w, rct_widgetindex widgetIndex);
static void WindowTitleEditorResize(rct_window* w);
static void WindowTitleEditorMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget);
static void WindowTitleEditorDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void WindowTitleEditorUpdate(rct_window* w);
static void WindowTitleEditorScrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height);
static void WindowTitleEditorScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowTitleEditorScrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowTitleEditorTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text);
static void WindowTitleEditorInvalidate(rct_window* w);
static void WindowTitleEditorPaint(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowTitleEditorScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex);
static void WindowTitleEditorScrollpaintSaves(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowTitleEditorScrollpaintCommands(rct_window* w, rct_drawpixelinfo* dpi);
static void WindowTitleEditorDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w);
static void WindowTitleEditorLoadSequence(size_t index);
static ITitleSequencePlayer* WindowTitleEditorGetPlayer();
static bool WindowTitleEditorCheckCanEdit();
static void WindowTitleEditorAddParkCallback(int32_t result, const utf8* path);
static void WindowTitleEditorRenamePark(size_t index, const utf8* name);

static rct_window_event_list window_title_editor_events([](auto& events) {
    events.close = &WindowTitleEditorClose;
    events.mouse_up = &WindowTitleEditorMouseup;
    events.resize = &WindowTitleEditorResize;
    events.mouse_down = &WindowTitleEditorMousedown;
    events.dropdown = &WindowTitleEditorDropdown;
    events.update = &WindowTitleEditorUpdate;
    events.get_scroll_size = &WindowTitleEditorScrollgetsize;
    events.scroll_mousedown = &WindowTitleEditorScrollmousedown;
    events.scroll_mouseover = &WindowTitleEditorScrollmouseover;
    events.text_input = &WindowTitleEditorTextinput;
    events.invalidate = &WindowTitleEditorInvalidate;
    events.paint = &WindowTitleEditorPaint;
    events.scroll_paint = &WindowTitleEditorScrollpaint;
});

enum WindowTitleEditorWidgetIdx
{
    WIDX_TITLE_EDITOR_BACKGROUND,
    WIDX_TITLE_EDITOR_TITLE,
    WIDX_TITLE_EDITOR_CLOSE,
    WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL,
    WIDX_TITLE_EDITOR_PRESETS_TAB,
    WIDX_TITLE_EDITOR_SAVES_TAB,
    WIDX_TITLE_EDITOR_SCRIPT_TAB,
    WIDX_TITLE_EDITOR_LIST,

    // Presets Tab
    WIDX_TITLE_EDITOR_PRESETS,
    WIDX_TITLE_EDITOR_PRESETS_DROPDOWN,
    WIDX_TITLE_EDITOR_NEW_BUTTON,
    WIDX_TITLE_EDITOR_DUPLICATE_BUTTON,
    WIDX_TITLE_EDITOR_DELETE_BUTTON,
    WIDX_TITLE_EDITOR_RENAME_BUTTON,

    // Saves Tab
    WIDX_TITLE_EDITOR_ADD_SAVE,
    WIDX_TITLE_EDITOR_REMOVE_SAVE,
    WIDX_TITLE_EDITOR_RENAME_SAVE,
    WIDX_TITLE_EDITOR_LOAD_SAVE,

    // Script Tab
    WIDX_TITLE_EDITOR_INSERT,
    WIDX_TITLE_EDITOR_EDIT,
    WIDX_TITLE_EDITOR_DELETE,
    // WIDX_TITLE_EDITOR_RELOAD,
    WIDX_TITLE_EDITOR_SKIP_TO,

    WIDX_TITLE_EDITOR_MOVE_DOWN,
    WIDX_TITLE_EDITOR_MOVE_UP,

    WIDX_TITLE_EDITOR_REPLAY,
    WIDX_TITLE_EDITOR_STOP,
    WIDX_TITLE_EDITOR_PLAY,
    WIDX_TITLE_EDITOR_SKIP,
};

// Increase BW if certain languages do not fit
// BW should be a multiple of 4
static constexpr const rct_string_id WINDOW_TITLE = STR_TITLE_EDITOR_TITLE;
static constexpr const int32_t WW = 320;
static constexpr const int32_t WH = 270;
static constexpr const int32_t WH2 = 127;
static constexpr const ScreenSize ACTION_BTN = { 91, 14 };
static constexpr const ScreenSize EDIT_BTN = { 72, 14 };
static constexpr const ScreenSize PLAYBACK_BTN = { 18, 17 };
static constexpr const ScreenSize REORDER_BTN = { 36, 14 };

static constexpr int32_t SCROLL_WIDTH = 350;

// clang-format off
static rct_widget window_title_editor_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({   0,  43}, { 320,  84}, WindowWidgetType::Resize,        WindowColour::Secondary             ), // tab content panel
    MakeTab   ({   3,  17}, STR_THEMES_TAB_SETTINGS_TIP                                         ), // presets tab
    MakeTab   ({  34,  17}, STR_TITLE_EDITOR_SAVES_TAB_TIP                                      ), // saves tab
    MakeTab   ({  65,  17}, STR_TITLE_EDITOR_SCRIPT_TAB_TIP                                     ), // script tab
    MakeWidget({  89,  48}, { 228, 219}, WindowWidgetType::Scroll,        WindowColour::Secondary, SCROLL_BOTH), // command/save list

    // Presets Tab
    MakeWidget     ({ 125,  60},  { 175,  14}, WindowWidgetType::DropdownMenu,        WindowColour::Secondary                                                                                   ), // Preset title sequences
    MakeWidget     ({ 288,  61},  {  11,  10}, WindowWidgetType::Button,          WindowColour::Secondary, STR_DROPDOWN_GLYPH                                                               ),
    MakeWidget     ({  10,  82},   ACTION_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_CREATE,    STR_TITLE_EDITOR_ACTION_CREATE_SEQUENCE_TIP   ), // Create button
    MakeWidget     ({  10, 102},   ACTION_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ACTION_DUPLICATE_SEQUENCE_TIP), // Duplicate button
    MakeWidget     ({ 110,  82},   ACTION_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TRACK_MANAGE_DELETE,           STR_TITLE_EDITOR_ACTION_DELETE_SEQUENCE_TIP   ), // Delete button
    MakeWidget     ({ 210,  82},   ACTION_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TRACK_MANAGE_RENAME,           STR_TITLE_EDITOR_ACTION_RENAME_SEQUENCE_TIP   ), // Rename button

    // Saves Tab
    MakeWidget     ({   8,  52},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_ADD,       STR_TITLE_EDITOR_ACTION_ADD_TIP               ), // Add
    MakeWidget     ({   8,  70},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_REMOVE,    STR_TITLE_EDITOR_ACTION_REMOVE_TIP            ), // Remove
    MakeWidget     ({   8,  88},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TRACK_MANAGE_RENAME,           STR_TITLE_EDITOR_ACTION_RENAME_TIP            ), // Rename
    MakeWidget     ({   8, 106},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_LOAD,      STR_TITLE_EDITOR_ACTION_LOAD_TIP              ), // Load

    // Script Tab
    MakeWidget     ({   8,  52},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_INSERT,    STR_TITLE_EDITOR_ACTION_INSERT_TIP            ), // Insert
    MakeWidget     ({   8,  70},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_EDIT,      STR_TITLE_EDITOR_ACTION_EDIT_TIP              ), // Edit
    MakeWidget     ({   8,  88},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TRACK_MANAGE_DELETE,           STR_TITLE_EDITOR_ACTION_DELETE_TIP            ), // Delete
    // MakeWidget  ({   8, 106},     EDIT_BTN, WindowWidgetType::DropdownMenu_BUTTON, WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_RELOAD,    STR_TITLE_EDITOR_ACTION_RELOAD_TIP            ), // Reload
    MakeWidget     ({   8, 106},     EDIT_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_TITLE_EDITOR_ACTION_SKIP_TO,   STR_TITLE_EDITOR_ACTION_SKIP_TO_TIP           ), // Skip to

    MakeWidget     ({   8, 142},  REORDER_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_DOWN,                          STR_TITLE_EDITOR_ACTION_MOVE_DOWN_TIP         ), // Move down
    MakeWidget     ({  44, 142},  REORDER_BTN, WindowWidgetType::Button,          WindowColour::Secondary, STR_UP,                            STR_TITLE_EDITOR_ACTION_MOVE_UP_TIP           ), // Move up

    MakeWidget     ({   8, 238}, PLAYBACK_BTN, WindowWidgetType::ImgBtn,          WindowColour::Secondary, SPR_G2_TITLE_RESTART,              STR_TITLE_EDITOR_ACTION_REPLAY_TIP            ), // Replay
    MakeWidget     ({  26, 238}, PLAYBACK_BTN, WindowWidgetType::ImgBtn,          WindowColour::Secondary, SPR_G2_TITLE_STOP,                 STR_TITLE_EDITOR_ACTION_STOP_TIP              ), // Stop
    MakeWidget     ({  44, 238}, PLAYBACK_BTN, WindowWidgetType::ImgBtn,          WindowColour::Secondary, SPR_G2_TITLE_PLAY,                 STR_TITLE_EDITOR_ACTION_PLAY_TIP              ), // Play
    MakeWidget     ({  62, 238}, PLAYBACK_BTN, WindowWidgetType::ImgBtn,          WindowColour::Secondary, SPR_G2_TITLE_SKIP,                 STR_TITLE_EDITOR_ACTION_SKIP_TIP              ), // Skip
    WIDGETS_END,
};
// clang-format on

static size_t _selectedTitleSequence = SIZE_MAX;
static bool _isSequenceReadOnly;
static std::unique_ptr<TitleSequence> _editingTitleSequence;
static const utf8* _sequenceName;

static utf8* _renameSavePath = nullptr;

static int16_t _window_title_editor_highlighted_index;

static int32_t window_title_editor_tab_animation_loops[] = {
    64,
    1,
    28,
};
static int32_t window_title_editor_tab_animation_divisor[] = {
    4,
    1,
    4,
};
static int32_t window_title_editor_tab_sprites[] = {
    SPR_TAB_RIDE_0,
    SPR_FLOPPY,
    SPR_TAB_STATS_0,
};

void WindowTitleEditorOpen(int32_t tab)
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_TITLE_EDITOR);
    if (window != nullptr)
        return;

    if (_selectedTitleSequence == SIZE_MAX)
        _selectedTitleSequence = title_get_config_sequence();

    window = WindowCreateAutoPos(WW, WH2, &window_title_editor_events, WC_TITLE_EDITOR, WF_10 | WF_RESIZABLE);
    window->widgets = window_title_editor_widgets;

    WindowInitScrollWidgets(window);
    window->list_information_type = 0;

    window->selected_tab = tab;
    window->selected_list_item = -1;
    _window_title_editor_highlighted_index = -1;
    window->scrolls[0].v_top = 0;
    window->scrolls[0].h_left = 0;

    window->min_width = WW;
    window->min_height = WH;
    window->max_width = 500;
    window->max_height = 450;

    if (_selectedTitleSequence >= title_sequence_manager_get_count())
        _selectedTitleSequence = 0;

    WindowTitleEditorLoadSequence(_selectedTitleSequence);
}

static void WindowTitleEditorClose(rct_window* w)
{
    title_stop_previewing_sequence();

    // Close the related windows
    window_close_by_class(WC_TITLE_COMMAND_EDITOR);

    _editingTitleSequence = nullptr;
    _sequenceName = nullptr;

    SafeFree(_renameSavePath);
}

static void WindowTitleEditorMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    bool commandEditorOpen = (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr);
    switch (widgetIndex)
    {
        case WIDX_TITLE_EDITOR_CLOSE:
            window_close(w);
            break;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Presets tab
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        case WIDX_TITLE_EDITOR_NEW_BUTTON:
            if (!commandEditorOpen)
            {
                WindowTextInputOpen(
                    w, widgetIndex, STR_TITLE_EDITOR_ACTION_CREATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, {}, STR_NONE, 0,
                    64);
            }
            break;
        case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
            if (!commandEditorOpen && _editingTitleSequence != nullptr)
            {
                WindowTextInputOpen(
                    w, widgetIndex, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, {}, STR_STRING,
                    reinterpret_cast<uintptr_t>(_sequenceName), 64);
            }
            break;
        case WIDX_TITLE_EDITOR_DELETE_BUTTON:
            if (WindowTitleEditorCheckCanEdit())
            {
                title_sequence_manager_delete(_selectedTitleSequence);
                WindowTitleEditorLoadSequence(0);
            }
            break;
        case WIDX_TITLE_EDITOR_RENAME_BUTTON:
            if (WindowTitleEditorCheckCanEdit() && _editingTitleSequence != nullptr)
            {
                WindowTextInputOpen(
                    w, widgetIndex, STR_TRACK_MANAGE_RENAME, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, {}, STR_STRING,
                    reinterpret_cast<uintptr_t>(_sequenceName), 64);
            }
            break;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Editor tab
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        case WIDX_TITLE_EDITOR_ADD_SAVE:
            if (!_isSequenceReadOnly && !title_is_previewing_sequence() && !commandEditorOpen)
            {
                auto intent = Intent(WC_LOADSAVE);
                intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME);
                intent.putExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<void*>(WindowTitleEditorAddParkCallback));
                context_open_intent(&intent);
            }
            break;
        case WIDX_TITLE_EDITOR_REMOVE_SAVE:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1)
                {
                    TitleSequenceRemovePark(*_editingTitleSequence, w->selected_list_item);
                    if (w->selected_list_item >= static_cast<int16_t>(_editingTitleSequence->Saves.size()))
                    {
                        w->selected_list_item--;
                    }
                }
            }
            break;
        case WIDX_TITLE_EDITOR_RENAME_SAVE:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1)
                {
                    WindowTextInputOpen(
                        w, widgetIndex, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SAVE, {}, STR_STRING,
                        reinterpret_cast<uintptr_t>(_editingTitleSequence->Saves[w->selected_list_item].c_str()), 52 - 1);
                }
            }
            break;
        case WIDX_TITLE_EDITOR_LOAD_SAVE:
            if (w->selected_list_item >= 0 && w->selected_list_item < static_cast<int16_t>(_editingTitleSequence->Saves.size()))
            {
                auto handle = TitleSequenceGetParkHandle(*_editingTitleSequence, w->selected_list_item);
                bool isScenario = ParkImporter::ExtensionIsScenario(handle->HintPath);
                try
                {
                    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
                    auto parkImporter = ParkImporter::Create(handle->HintPath);
                    auto result = parkImporter->LoadFromStream(handle->Stream.get(), isScenario);
                    objectMgr.LoadObjects(result.RequiredObjects);
                    parkImporter->Import();

                    if (isScenario)
                        scenario_begin();
                    else
                        game_load_init();

                    WindowTitleEditorOpen(WINDOW_TITLE_EDITOR_TAB_SAVES);
                }
                catch (const std::exception&)
                {
                    context_show_error(ERROR_TYPE_FILE_LOAD, STR_FILE_CONTAINS_INVALID_DATA, {});
                }
            }
            break;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Commands tab
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        case WIDX_TITLE_EDITOR_INSERT:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1)
                    WindowTitleCommandEditorOpen(_editingTitleSequence.get(), w->selected_list_item + 1, true);
                else
                    WindowTitleCommandEditorOpen(
                        _editingTitleSequence.get(), static_cast<int32_t>(_editingTitleSequence->Commands.size()), true);
            }
            break;
        case WIDX_TITLE_EDITOR_EDIT:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1
                    && w->selected_list_item < static_cast<int16_t>(_editingTitleSequence->Commands.size()))
                {
                    WindowTitleCommandEditorOpen(_editingTitleSequence.get(), w->selected_list_item, false);
                }
            }
            break;
        case WIDX_TITLE_EDITOR_DELETE:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1
                    && w->selected_list_item < static_cast<int16_t>(_editingTitleSequence->Commands.size()))
                {
                    _editingTitleSequence->Commands.erase(_editingTitleSequence->Commands.begin() + w->selected_list_item);
                    if (w->selected_list_item >= static_cast<int16_t>(_editingTitleSequence->Commands.size()))
                    {
                        w->selected_list_item--;
                    }
                    TitleSequenceSave(*_editingTitleSequence);
                }
            }
            break;
        case WIDX_TITLE_EDITOR_SKIP_TO:
        {
            int32_t position = w->selected_list_item;
            if (title_is_previewing_sequence() && position != -1
                && position < static_cast<int32_t>(_editingTitleSequence->Commands.size()))
            {
                auto player = WindowTitleEditorGetPlayer();
                player->Seek(position);
                player->Update();
            }
            break;
        }
        case WIDX_TITLE_EDITOR_MOVE_DOWN:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item != -1
                    && w->selected_list_item < static_cast<int16_t>(_editingTitleSequence->Commands.size()) - 1)
                {
                    std::swap(
                        _editingTitleSequence->Commands[w->selected_list_item],
                        _editingTitleSequence->Commands[w->selected_list_item + 1]);
                    w->selected_list_item++;
                    TitleSequenceSave(*_editingTitleSequence);
                }
            }
            break;
        case WIDX_TITLE_EDITOR_MOVE_UP:
            if (WindowTitleEditorCheckCanEdit())
            {
                if (w->selected_list_item > 0
                    && w->selected_list_item < static_cast<int16_t>(_editingTitleSequence->Commands.size()))
                {
                    std::swap(
                        _editingTitleSequence->Commands[w->selected_list_item - 1],
                        _editingTitleSequence->Commands[w->selected_list_item]);
                    w->selected_list_item--;
                    TitleSequenceSave(*_editingTitleSequence);
                }
            }
            break;
        case WIDX_TITLE_EDITOR_REPLAY:
            if (title_is_previewing_sequence())
            {
                auto player = WindowTitleEditorGetPlayer();
                player->Reset();
                player->Update();
            }
            break;
        case WIDX_TITLE_EDITOR_STOP:
            if (title_is_previewing_sequence())
            {
                title_stop_previewing_sequence();
            }
            break;
        case WIDX_TITLE_EDITOR_PLAY:
            if (!title_is_previewing_sequence() || _selectedTitleSequence != title_get_current_sequence())
            {
                if (!title_preview_sequence(_selectedTitleSequence))
                {
                    context_show_error(STR_ERR_FAILED_TO_LOAD_TITLE_SEQUENCE, STR_NONE, {});
                }
                else if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
                {
                    gPreviewingTitleSequenceInGame = true;
                }
            }
            break;
        case WIDX_TITLE_EDITOR_SKIP:
            if (title_is_previewing_sequence())
            {
                auto player = WindowTitleEditorGetPlayer();
                int32_t position = player->GetCurrentPosition() + 1;
                if (position >= static_cast<int32_t>(_editingTitleSequence->Commands.size()))
                {
                    position = 0;
                }
                player->Seek(position);
                player->Update();
            }
            break;
    }
}

static void WindowTitleEditorResize(rct_window* w)
{
    if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_PRESETS)
        window_set_resize(w, WW, WH2, 500, WH2);
    else
        window_set_resize(w, WW, WH, 500, 580);
}

static void WindowTitleEditorMousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_TITLE_EDITOR_PRESETS_TAB:
        case WIDX_TITLE_EDITOR_SAVES_TAB:
        case WIDX_TITLE_EDITOR_SCRIPT_TAB:
        {
            int32_t newSelectedTab = widgetIndex - WIDX_TITLE_EDITOR_PRESETS_TAB;
            if (w->selected_tab != newSelectedTab)
            {
                w->selected_tab = newSelectedTab;
                w->selected_list_item = -1;
                _window_title_editor_highlighted_index = -1;
                w->scrolls[0].v_top = 0;
                w->frame_no = 0;
                window_event_resize_call(w);
                w->Invalidate();
            }
            break;
        }
        case WIDX_TITLE_EDITOR_PRESETS_DROPDOWN:
            if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr)
            {
                context_show_error(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE, {});
            }
            else
            {
                int32_t numItems = static_cast<int32_t>(title_sequence_manager_get_count());
                for (int32_t i = 0; i < numItems; i++)
                {
                    gDropdownItems[i].Format = STR_OPTIONS_DROPDOWN_ITEM;
                    gDropdownItems[i].Args = reinterpret_cast<uintptr_t>(title_sequence_manager_get_name(i));
                }

                widget--;
                WindowDropdownShowTextCustomWidth(
                    { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0,
                    Dropdown::Flag::StayOpen, numItems, widget->width() - 3);
                Dropdown::SetChecked(static_cast<int32_t>(_selectedTitleSequence), true);
            }
            break;
    }
}

static void WindowTitleEditorDropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    if (widgetIndex == WIDX_TITLE_EDITOR_PRESETS_DROPDOWN)
    {
        WindowTitleEditorLoadSequence(dropdownIndex);
        w->Invalidate();
    }
}

static void WindowTitleEditorUpdate(rct_window* w)
{
    w->frame_no++;
    if (w->frame_no >= window_title_editor_tab_animation_loops[w->selected_tab])
    {
        w->frame_no = 0;
    }

    if (!WidgetIsHighlighted(w, WIDX_TITLE_EDITOR_LIST))
    {
        _window_title_editor_highlighted_index = -1;
        widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
    }

    widget_invalidate(w, WIDX_TITLE_EDITOR_PRESETS_TAB + w->selected_tab);
}

static void WindowTitleEditorScrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    size_t lineCount = 1;
    if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SAVES)
        lineCount = _editingTitleSequence->Saves.size();
    else if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SCRIPT)
        lineCount = _editingTitleSequence->Commands.size();

    *height = static_cast<int32_t>(lineCount * SCROLLABLE_ROW_HEIGHT);
    int32_t i = *height - window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom
        + window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].top + 21;
    if (i < 0)
    {
        i = 0;
    }
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        w->Invalidate();
    }

    *width = SCROLL_WIDTH;
}

static void WindowTitleEditorScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    w->selected_list_item = -1;
    switch (w->selected_tab)
    {
        case WINDOW_TITLE_EDITOR_TAB_SAVES:
            if (index < static_cast<int32_t>(_editingTitleSequence->Saves.size()))
            {
                w->selected_list_item = index;
                widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
            }
            break;
        case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
            if (index < static_cast<int32_t>(_editingTitleSequence->Commands.size()))
            {
                w->selected_list_item = index;
                widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
            }
            break;
    }
}

static void WindowTitleEditorScrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    switch (w->selected_tab)
    {
        case WINDOW_TITLE_EDITOR_TAB_SAVES:
            if (index < static_cast<int32_t>(_editingTitleSequence->Saves.size()))
                _window_title_editor_highlighted_index = static_cast<int16_t>(index);
            break;
        case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
            if (index < static_cast<int32_t>(_editingTitleSequence->Commands.size()))
                _window_title_editor_highlighted_index = static_cast<int16_t>(index);
            break;
    }
    widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
}

static void WindowTitleEditorTextinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (str_is_null_or_empty(text))
        return;

    switch (widgetIndex)
    {
        case WIDX_TITLE_EDITOR_NEW_BUTTON:
        case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
        case WIDX_TITLE_EDITOR_RENAME_BUTTON:
            if (filename_valid_characters(text))
            {
                if (title_sequence_manager_get_index_for_name(text) == SIZE_MAX)
                {
                    if (!title_sequence_manager_is_name_reserved(text))
                    {
                        if (widgetIndex == WIDX_TITLE_EDITOR_NEW_BUTTON)
                        {
                            size_t newIndex = title_sequence_manager_create(text);
                            WindowTitleEditorLoadSequence(newIndex);
                        }
                        else if (widgetIndex == WIDX_TITLE_EDITOR_DUPLICATE_BUTTON)
                        {
                            size_t newIndex = title_sequence_manager_duplicate(_selectedTitleSequence, text);
                            WindowTitleEditorLoadSequence(newIndex);
                        }
                        else
                        {
                            size_t newIndex = title_sequence_manager_rename(_selectedTitleSequence, text);
                            WindowTitleEditorLoadSequence(newIndex);
                        }
                        config_save_default();
                        w->Invalidate();
                    }
                    else
                    {
                        context_show_error(STR_ERROR_RESERVED_NAME, STR_NONE, {});
                    }
                }
                else
                {
                    context_show_error(STR_ERROR_EXISTING_NAME, STR_NONE, {});
                }
            }
            else
            {
                context_show_error(STR_ERROR_INVALID_CHARACTERS, STR_NONE, {});
            }
            break;
        case WIDX_TITLE_EDITOR_RENAME_SAVE:
            WindowTitleEditorRenamePark(w->selected_list_item, text);
            break;
    }
}

static void WindowTitleEditorInvalidate(rct_window* w)
{
    int32_t pressed_widgets = w->pressed_widgets
        & ~((1LL << WIDX_TITLE_EDITOR_PRESETS_TAB) | (1LL << WIDX_TITLE_EDITOR_SAVES_TAB)
            | (1LL << WIDX_TITLE_EDITOR_SCRIPT_TAB));
    rct_widgetindex widgetIndex = w->selected_tab + WIDX_TITLE_EDITOR_PRESETS_TAB;

    w->pressed_widgets = pressed_widgets | (1 << widgetIndex);

    window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WindowWidgetType::Empty;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WindowWidgetType::Empty;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WindowWidgetType::Empty;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WindowWidgetType::Empty;
    // window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WindowWidgetType::Empty;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WindowWidgetType::Empty;

    switch (w->selected_tab)
    {
        case WINDOW_TITLE_EDITOR_TAB_PRESETS:
            window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WindowWidgetType::DropdownMenu;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WindowWidgetType::Button;
            break;
        case WINDOW_TITLE_EDITOR_TAB_SAVES:
            window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WindowWidgetType::Scroll;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WindowWidgetType::ImgBtn;
            break;
        case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
            window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WindowWidgetType::Scroll;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WindowWidgetType::Button;
            // window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WindowWidgetType::DropdownMenu_BUTTON;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WindowWidgetType::Button;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WindowWidgetType::ImgBtn;
            window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WindowWidgetType::ImgBtn;
            break;
    }

    window_title_editor_widgets[WIDX_TITLE_EDITOR_BACKGROUND].right = w->width - 1;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_BACKGROUND].bottom = w->height - 1;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL].right = w->width - 1;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_TAB_CONTENT_PANEL].bottom = w->height - 1;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_TITLE].right = w->width - 2;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_CLOSE].left = w->width - 2 - 0x0B;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].right = w->width - 4;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom = w->height - 16;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].top = w->height - 32;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].bottom = w->height - 16;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].top = w->height - 32;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].bottom = w->height - 16;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].top = w->height - 32;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].bottom = w->height - 16;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].top = w->height - 32;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].bottom = w->height - 16;

    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && gScreenFlags != SCREEN_FLAGS_PLAYING)
        w->disabled_widgets |= (1ULL << WIDX_TITLE_EDITOR_PLAY);
    else
        w->disabled_widgets &= ~(1ULL << WIDX_TITLE_EDITOR_PLAY);
    if (!title_is_previewing_sequence())
        w->disabled_widgets |= (1ULL << WIDX_TITLE_EDITOR_REPLAY) | (1ULL << WIDX_TITLE_EDITOR_STOP)
            | (1ULL << WIDX_TITLE_EDITOR_SKIP) | (1ULL << WIDX_TITLE_EDITOR_SKIP_TO);
    else
        w->disabled_widgets &= ~(
            (1ULL << WIDX_TITLE_EDITOR_REPLAY) | (1ULL << WIDX_TITLE_EDITOR_STOP) | (1ULL << WIDX_TITLE_EDITOR_SKIP)
            | (1ULL << WIDX_TITLE_EDITOR_SKIP_TO));
}

static void WindowTitleEditorPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    // Widgets
    WindowDrawWidgets(w, dpi);
    WindowTitleEditorDrawTabImages(dpi, w);

    // Draw strings
    switch (w->selected_tab)
    {
        case WINDOW_TITLE_EDITOR_TAB_PRESETS:
        {
            DrawTextBasic(
                dpi, w->windowPos + ScreenCoordsXY{ 10, window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top + 1 },
                STR_TITLE_SEQUENCE, {}, { w->colours[1] });

            auto ft = Formatter();
            ft.Add<const char*>(_sequenceName);

            ScreenCoordsXY screenPos = { w->windowPos.x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left + 1,
                                         w->windowPos.y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top };
            auto width = w->windowPos.x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].left
                - window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left - 4;

            DrawTextEllipsised(dpi, screenPos, width, STR_STRING, ft, { w->colours[1] });
            break;
        }
        case WINDOW_TITLE_EDITOR_TAB_SAVES:
            break;
        case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
            break;
    }
}

static void WindowTitleEditorScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    gfx_fill_rect(
        dpi, { { dpi->x, dpi->y }, { dpi->x + dpi->width - 1, dpi->y + dpi->height - 1 } },
        ColourMapA[w->colours[1]].mid_light);
    switch (w->selected_tab)
    {
        case WINDOW_TITLE_EDITOR_TAB_SAVES:
            WindowTitleEditorScrollpaintSaves(w, dpi);
            break;
        case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
            WindowTitleEditorScrollpaintCommands(w, dpi);
            break;
    }
}

static void WindowTitleEditorScrollpaintSaves(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t currentSaveIndex = -1;
    auto screenCoords = ScreenCoordsXY{ 0, 0 };
    if (_editingTitleSequence == nullptr)
        return;

    for (int32_t i = 0; i < static_cast<int32_t>(_editingTitleSequence->Saves.size());
         i++, screenCoords.y += SCROLLABLE_ROW_HEIGHT)
    {
        bool selected = false;
        bool hover = false;
        auto fillRect = ScreenRect{ screenCoords,
                                    screenCoords + ScreenCoordsXY{ SCROLL_WIDTH + 100, SCROLLABLE_ROW_HEIGHT - 1 } };
        if (i == w->selected_list_item)
        {
            selected = true;
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].dark);
        }
        else if (i == _window_title_editor_highlighted_index || i == currentSaveIndex)
        {
            hover = true;
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].mid_dark);
        }
        else if (i & 1)
        {
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].lighter | 0x1000000);
        }

        auto saveName = _editingTitleSequence->Saves[i].c_str();
        auto ft = Formatter();
        if (selected || hover)
        {
            ft.Add<rct_string_id>(STR_STRING);
        }
        else
        {
            ft.Add<rct_string_id>(STR_BLACK_STRING);
            ft.Add<rct_string_id>(STR_STRING);
        }
        ft.Add<const char*>(saveName);
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 5, 0 }, STR_STRINGID, ft, { w->colours[1] });
    }
}

static void WindowTitleEditorScrollpaintCommands(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t position = -1;
    if (title_is_previewing_sequence() && _selectedTitleSequence == title_get_current_sequence())
    {
        auto player = WindowTitleEditorGetPlayer();
        position = player->GetCurrentPosition();
    }

    auto screenCoords = ScreenCoordsXY{ 0, 0 };
    for (int32_t i = 0; i < static_cast<int32_t>(_editingTitleSequence->Commands.size());
         i++, screenCoords.y += SCROLLABLE_ROW_HEIGHT)
    {
        TitleCommand& command = _editingTitleSequence->Commands[i];
        bool selected = false;
        bool hover = false;
        bool error = false;

        auto fillRect = ScreenRect{ screenCoords,
                                    screenCoords + ScreenCoordsXY{ SCROLL_WIDTH + 100, SCROLLABLE_ROW_HEIGHT - 1 } };
        if (i == w->selected_list_item)
        {
            selected = true;
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].dark);
        }
        else if (i == static_cast<int32_t>(_window_title_editor_highlighted_index) || i == position)
        {
            hover = true;
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].mid_dark);
        }
        else if (i & 1)
        {
            gfx_fill_rect(dpi, fillRect, ColourMapA[w->colours[1]].lighter | 0x1000000);
        }

        if (command.Type == TitleScript::Load && command.SaveIndex == SAVE_INDEX_INVALID)
            error = true;

        auto ft = Formatter();
        if (error)
        {
            ft.Add<rct_string_id>(selected || hover ? STR_LIGHTPINK_STRINGID : STR_RED_STRINGID);
        }
        else
        {
            ft.Add<rct_string_id>(selected || hover ? STR_STRINGID : STR_BLACK_STRING);
        }

        switch (command.Type)
        {
            case TitleScript::Load:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
                if (command.SaveIndex == SAVE_INDEX_INVALID)
                {
                    commandName = STR_TITLE_EDITOR_COMMAND_LOAD_NO_SAVE;
                    ft.Add<rct_string_id>(commandName);
                }
                else
                {
                    ft.Add<rct_string_id>(commandName);
                    ft.Add<const char*>(_editingTitleSequence->Saves[command.SaveIndex].c_str());
                }
                break;
            }
            case TitleScript::Location:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_LOCATION;
                ft.Add<rct_string_id>(commandName);
                ft.Add<uint16_t>(command.Location.X);
                ft.Add<uint16_t>(command.Location.Y);
                break;
            }
            case TitleScript::Rotate:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_ROTATE;
                ft.Add<rct_string_id>(commandName);
                ft.Add<uint16_t>(command.Rotations);
                break;
            }
            case TitleScript::Zoom:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_ZOOM;
                ft.Add<rct_string_id>(commandName);
                ft.Add<uint16_t>(command.Zoom);
                break;
            }
            case TitleScript::Speed:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_SPEED;
                ft.Add<rct_string_id>(commandName);
                ft.Add<rct_string_id>(SpeedNames[command.Speed - 1]);
                break;
            }
            case TitleScript::Follow:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_FOLLOW;
                if (command.Follow.SpriteIndex.IsNull())
                {
                    commandName = STR_TITLE_EDITOR_COMMAND_FOLLOW_NO_SPRITE;
                    ft.Add<rct_string_id>(commandName);
                }
                else
                {
                    ft.Add<rct_string_id>(commandName);
                    ft.Add<utf8*>(command.Follow.SpriteName);
                }
                break;
            }
            case TitleScript::Wait:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_WAIT;
                ft.Add<rct_string_id>(commandName);
                ft.Add<uint16_t>(command.Milliseconds);
                break;
            }
            case TitleScript::Restart:
            {
                auto commandName = STR_TITLE_EDITOR_RESTART;
                ft.Add<rct_string_id>(commandName);
                break;
            }
            case TitleScript::End:
            {
                auto commandName = STR_TITLE_EDITOR_END;
                ft.Add<rct_string_id>(commandName);
                break;
            }
            case TitleScript::LoadSc:
            {
                auto commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
                const char* name = "";
                auto scenario = GetScenarioRepository()->GetByInternalName(command.Scenario);
                if (command.Scenario[0] == '\0')
                {
                    commandName = STR_TITLE_EDITOR_COMMAND_LOAD_NO_SCENARIO;
                }
                else if (scenario != nullptr)
                {
                    name = scenario->name;
                }
                else
                {
                    commandName = STR_TITLE_EDITOR_COMMAND_LOAD_MISSING_SCENARIO;
                }
                ft.Add<rct_string_id>(commandName);
                ft.Add<const char*>(name);
                break;
            }
            default:
            {
                ft.Add<rct_string_id>(STR_NONE);
                log_warning("Unknown command %d", command.Type);
            }
        }
        DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 5, 0 }, STR_STRINGID, ft, { w->colours[1] });
    }
}

static void WindowTitleEditorDrawTabImages(rct_drawpixelinfo* dpi, rct_window* w)
{
    for (int32_t i = 0; i < WINDOW_TITLE_EDITOR_TAB_COUNT; i++)
    {
        auto screenCoords = ScreenCoordsXY{ 0, 0 };
        int32_t spriteId = window_title_editor_tab_sprites[i];
        if (w->selected_tab == i)
        {
            spriteId += w->frame_no / window_title_editor_tab_animation_divisor[w->selected_tab];
        }
        if (i == 1)
        {
            screenCoords = ScreenCoordsXY{ 4, 1 };
        }
        gfx_draw_sprite(
            dpi, ImageId(spriteId),
            w->windowPos + screenCoords
                + ScreenCoordsXY{ w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].left,
                                  w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].top });
    }
}

static void WindowTitleEditorLoadSequence(size_t index)
{
    if (index >= title_sequence_manager_get_count())
        return;

    const char* path = title_sequence_manager_get_path(index);
    auto titleSequence = LoadTitleSequence(path);
    if (titleSequence == nullptr)
    {
        context_show_error(STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, STR_NONE, {});
        return;
    }

    _selectedTitleSequence = index;
    size_t predefinedIndex = title_sequence_manager_get_predefined_index(index);
    _isSequenceReadOnly = (predefinedIndex != SIZE_MAX);
    _sequenceName = title_sequence_manager_get_name(index);
    _editingTitleSequence = std::move(titleSequence);

    window_close_by_class(WC_TITLE_COMMAND_EDITOR);
}

static ITitleSequencePlayer* WindowTitleEditorGetPlayer()
{
    return static_cast<ITitleSequencePlayer*>(title_get_sequence_player());
}

static bool WindowTitleEditorCheckCanEdit()
{
    bool commandEditorOpen = (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr);

    if (_isSequenceReadOnly)
        context_show_error(STR_ERROR_CANT_CHANGE_TITLE_SEQUENCE, STR_NONE, {});
    else if (title_is_previewing_sequence())
        context_show_error(STR_TITLE_EDITOR_ERR_CANT_EDIT_WHILE_PLAYING, STR_TITLE_EDITOR_PRESS_STOP_TO_CONTINUE_EDITING, {});
    else if (commandEditorOpen)
        context_show_error(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE, {});
    else
        return true;

    return false;
}

static bool SaveFilenameExists(const utf8* filename)
{
    auto& seq = _editingTitleSequence;
    for (size_t i = 0; i < seq->Saves.size(); i++)
    {
        const auto& savePath = seq->Saves[i];

        if (_stricmp(savePath.c_str(), filename) == 0)
            return true;
    }
    return false;
}

static void WindowTitleEditorAddParkCallback(int32_t result, const utf8* path)
{
    auto extension = get_file_extension_type(path);
    if (extension != FileExtension::SV4 && extension != FileExtension::SV6 && extension != FileExtension::PARK)
        return;

    const utf8* filename = path_get_filename(path);
    if (SaveFilenameExists(filename))
    {
        free(_renameSavePath);
        _renameSavePath = _strdup(filename);
        rct_window* w = window_find_by_class(WC_TITLE_EDITOR);
        WindowTextInputOpen(
            w, WIDX_TITLE_EDITOR_RENAME_SAVE, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_ERROR_EXISTING_NAME, {}, STR_STRING,
            reinterpret_cast<uintptr_t>(_renameSavePath), 52 - 1);
        return;
    }

    TitleSequenceAddPark(*_editingTitleSequence, path, filename);
}

static void WindowTitleEditorRenamePark(size_t index, const utf8* name)
{
    if (!filename_valid_characters(name))
    {
        context_show_error(STR_ERROR_INVALID_CHARACTERS, STR_NONE, {});
        return;
    }

    for (size_t i = 0; i < _editingTitleSequence->Saves.size(); i++)
    {
        if (i != index)
        {
            const auto& savePath = _editingTitleSequence->Saves[i];
            if (_strcmpi(savePath.c_str(), name) == 0)
            {
                context_show_error(STR_ERROR_EXISTING_NAME, STR_NONE, {});
                return;
            }
        }
    }

    if (TitleSequenceRenamePark(*_editingTitleSequence, index, name))
    {
        TitleSequenceSave(*_editingTitleSequence);
    }
}
