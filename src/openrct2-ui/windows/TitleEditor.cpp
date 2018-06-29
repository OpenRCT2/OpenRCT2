/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/config/Config.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/title/TitleSequence.h>
#include <openrct2/title/TitleSequenceManager.h>
#include <openrct2/title/TitleSequencePlayer.h>
#include <openrct2/Context.h>
#include <openrct2/windows/Intent.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2/scenario/Scenario.h>
#include "../title/TitleSequencePlayer.h"

// clang-format off
enum WINDOW_TITLE_EDITOR_TAB {
    WINDOW_TITLE_EDITOR_TAB_PRESETS,
    WINDOW_TITLE_EDITOR_TAB_SAVES,
    WINDOW_TITLE_EDITOR_TAB_SCRIPT,
    WINDOW_TITLE_EDITOR_TAB_COUNT
};

static void window_title_editor_close(rct_window * w);
static void window_title_editor_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_title_editor_resize(rct_window * w);
static void window_title_editor_mousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget);
static void window_title_editor_dropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_title_editor_update(rct_window * w);
static void window_title_editor_scrollgetsize(rct_window * w, int32_t scrollIndex, int32_t * width, int32_t * height);
static void window_title_editor_scrollmousedown(rct_window * w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_title_editor_scrollmouseover(rct_window * w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_title_editor_textinput(rct_window * w, rct_widgetindex widgetIndex, char * text);
static void window_title_editor_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id * stringId);
static void window_title_editor_invalidate(rct_window * w);
static void window_title_editor_paint(rct_window * w, rct_drawpixelinfo * dpi);
static void window_title_editor_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int32_t scrollIndex);
static void window_title_editor_scrollpaint_saves(rct_window * w, rct_drawpixelinfo * dpi);
static void window_title_editor_scrollpaint_commands(rct_window * w, rct_drawpixelinfo * dpi);
static void window_title_editor_draw_tab_images(rct_drawpixelinfo * dpi, rct_window * w);
static void window_title_editor_load_sequence(size_t index);
static ITitleSequencePlayer * window_title_editor_get_player();
static bool window_title_editor_check_can_edit();
static void window_title_editor_add_park_callback(int32_t result, const utf8 * path);
static void window_title_editor_rename_park(size_t index, const utf8 * name);

static rct_window_event_list window_title_editor_events = {
    window_title_editor_close,
    window_title_editor_mouseup,
    window_title_editor_resize,
    window_title_editor_mousedown,
    window_title_editor_dropdown,
    nullptr,
    window_title_editor_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_title_editor_scrollgetsize,
    window_title_editor_scrollmousedown,
    nullptr,
    window_title_editor_scrollmouseover,
    window_title_editor_textinput,
    nullptr,
    nullptr,
    window_title_editor_tooltip,
    nullptr,
    nullptr,
    window_title_editor_invalidate,
    window_title_editor_paint,
    window_title_editor_scrollpaint,
};

enum WINDOW_TITLE_EDITOR_WIDGET_IDX {
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
    //WIDX_TITLE_EDITOR_RELOAD,
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
#define WW 320
#define WH 270
#define BX 8
#define BW 72
#define BY 52
#define BH 63
#define BS 18
#define SCROLL_WIDTH 350
#define WH2 127

static rct_widget window_title_editor_widgets[] = {
    { WWT_FRAME,            0,  0,      WW-1,   0,      WH2-1,  0xFFFFFFFF,             STR_NONE },                             // panel / background
    { WWT_CAPTION,          0,  1,      WW-2,   1,      14,     STR_TITLE_EDITOR_TITLE, STR_WINDOW_TITLE_TIP },                 // title bar
    { WWT_CLOSEBOX,         0,  WW-13,  WW-3,   2,      13,     STR_CLOSE_X,            STR_CLOSE_WINDOW_TIP },                 // close button
    { WWT_RESIZE,           1,  0,      WW-1,   43,     WH2-1,  0xFFFFFFFF,             STR_NONE },                             // tab content panel
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_THEMES_TAB_SETTINGS_TIP },  // presets tab
    { WWT_TAB,              1,  34,     64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_TITLE_EDITOR_SAVES_TAB_TIP },       // saves tab
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,   STR_TITLE_EDITOR_SCRIPT_TAB_TIP },      // script tab
    { WWT_SCROLL,           1,  BX+BW+9,WW-4,   48,     WH-4,   SCROLL_BOTH,            STR_NONE },                             // command/save list

    // Presets Tab
    { WWT_DROPDOWN,         1,  125,    299,    60,     71,     STR_NONE,                           STR_NONE },                     // Preset title sequences
    { WWT_BUTTON,           1,  288,    298,    61,     70,     STR_DROPDOWN_GLYPH,                 STR_NONE },
    { WWT_BUTTON,           1,  10,     100,    82,     93,     STR_TITLE_EDITOR_ACTION_CREATE,     STR_TITLE_EDITOR_ACTION_CREATE_SEQUENCE_TIP },                      // Create button
    { WWT_BUTTON,           1,  10,     100,    82+20,  93+20,  STR_TITLE_EDITOR_ACTION_DUPLICATE,  STR_TITLE_EDITOR_ACTION_DUPLICATE_SEQUENCE_TIP },                       // Duplicate button
    { WWT_BUTTON,           1,  110,    200,    82,     93,     STR_TRACK_MANAGE_DELETE,            STR_TITLE_EDITOR_ACTION_DELETE_SEQUENCE_TIP },                      // Delete button
    { WWT_BUTTON,           1,  210,    300,    82,     93,     STR_TRACK_MANAGE_RENAME,            STR_TITLE_EDITOR_ACTION_RENAME_SEQUENCE_TIP },                      // Rename button

    // Saves Tab
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY,         BH,         STR_TITLE_EDITOR_ACTION_ADD,        STR_TITLE_EDITOR_ACTION_ADD_TIP }, // Add
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*1),  BH+(BS*1),  STR_TITLE_EDITOR_ACTION_REMOVE,     STR_TITLE_EDITOR_ACTION_REMOVE_TIP }, // Remove
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*2),  BH+(BS*2),  STR_TRACK_MANAGE_RENAME,            STR_TITLE_EDITOR_ACTION_RENAME_TIP }, // Rename
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*3),  BH+(BS*3),  STR_TITLE_EDITOR_ACTION_LOAD,       STR_TITLE_EDITOR_ACTION_LOAD_TIP }, // Load

    // Script Tab
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY,         BH,         STR_TITLE_EDITOR_ACTION_INSERT,     STR_TITLE_EDITOR_ACTION_INSERT_TIP }, // Insert
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*1),  BH+(BS*1),  STR_TITLE_EDITOR_ACTION_EDIT,       STR_TITLE_EDITOR_ACTION_EDIT_TIP }, // Edit
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*2),  BH+(BS*2),  STR_TRACK_MANAGE_DELETE,            STR_TITLE_EDITOR_ACTION_DELETE_TIP }, // Delete
    //{ WWT_DROPDOWN_BUTTON,    1,  BX,     BX+BW-1,BY+(BS*3),  BH+(BS*3),  STR_TITLE_EDITOR_ACTION_RELOAD,     STR_TITLE_EDITOR_ACTION_RELOAD_TIP }, // Reload
    { WWT_BUTTON,           1,  BX,     BX+BW-1,BY+(BS*3),  BH+(BS*3),  STR_TITLE_EDITOR_ACTION_SKIP_TO,    STR_TITLE_EDITOR_ACTION_SKIP_TO_TIP }, // Skip to

    { WWT_BUTTON,           1,  BX,     BX+BW/2-1,BY+(BS*5),BH+(BS*5),  STR_DOWN,   STR_TITLE_EDITOR_ACTION_MOVE_DOWN_TIP }, // Move down
    { WWT_BUTTON,           1,  BX+BW/2,BX+BW-1,BY+(BS*5),  BH+(BS*5),  STR_UP, STR_TITLE_EDITOR_ACTION_MOVE_UP_TIP }, // Move up

    { WWT_IMGBTN,           1,  BX,     BX+BW/4-1,  WH-32,  WH-16,      SPR_G2_TITLE_RESTART,   STR_TITLE_EDITOR_ACTION_REPLAY_TIP }, // Replay
    { WWT_IMGBTN,           1,  BX+BW/4,BX+BW/2-1,  WH-32,  WH-16,      SPR_G2_TITLE_STOP,      STR_TITLE_EDITOR_ACTION_STOP_TIP }, // Stop
    { WWT_IMGBTN,           1,  BX+BW/2,BX+BW*3/4-1,WH-32,  WH-16,      SPR_G2_TITLE_PLAY,      STR_TITLE_EDITOR_ACTION_PLAY_TIP }, // Play
    { WWT_IMGBTN,           1,  BX+BW*3/4,BX+BW,    WH-32,  WH-16,      SPR_G2_TITLE_SKIP,      STR_TITLE_EDITOR_ACTION_SKIP_TIP }, // Skip

    { WIDGETS_END },
};

static size_t _selectedTitleSequence = SIZE_MAX;
static bool _isSequenceReadOnly;
static TitleSequence * _editingTitleSequence = nullptr;
static const utf8 * _sequenceName;

static utf8 * _renameSavePath = nullptr;

static int16_t _window_title_editor_highlighted_index;

static int32_t window_title_editor_tab_animation_loops[] = {
    64,
    1,
    28
};
static int32_t window_title_editor_tab_animation_divisor[] = {
    4,
    1,
    4
};
static int32_t window_title_editor_tab_sprites[] = {
    SPR_TAB_RIDE_0,
    SPR_FLOPPY,
    SPR_TAB_STATS_0
};
// clang-format on

void window_title_editor_open(int32_t tab)
{
    rct_window * window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_TITLE_EDITOR);
    if (window != nullptr)
        return;

    if (_selectedTitleSequence == SIZE_MAX)
        _selectedTitleSequence = title_get_config_sequence();

    window = window_create_auto_pos(WW, WH2, &window_title_editor_events, WC_TITLE_EDITOR, WF_10 | WF_RESIZABLE);
    window->widgets = window_title_editor_widgets;
    window->enabled_widgets =
        (1 << WIDX_TITLE_EDITOR_CLOSE) |
        (1 << WIDX_TITLE_EDITOR_PRESETS_TAB) |
        (1 << WIDX_TITLE_EDITOR_SAVES_TAB) |
        (1 << WIDX_TITLE_EDITOR_SCRIPT_TAB) |

        (1 << WIDX_TITLE_EDITOR_PRESETS) |
        (1 << WIDX_TITLE_EDITOR_PRESETS_DROPDOWN) |
        (1 << WIDX_TITLE_EDITOR_NEW_BUTTON) |
        (1 << WIDX_TITLE_EDITOR_DUPLICATE_BUTTON) |
        (1 << WIDX_TITLE_EDITOR_DELETE_BUTTON) |
        (1 << WIDX_TITLE_EDITOR_RENAME_BUTTON) |

        (1 << WIDX_TITLE_EDITOR_ADD_SAVE) |
        (1 << WIDX_TITLE_EDITOR_REMOVE_SAVE) |
        (1 << WIDX_TITLE_EDITOR_RENAME_SAVE) |
        (1 << WIDX_TITLE_EDITOR_LOAD_SAVE) |

        (1 << WIDX_TITLE_EDITOR_INSERT) |
        (1 << WIDX_TITLE_EDITOR_EDIT) |
        (1 << WIDX_TITLE_EDITOR_DELETE) |
        //(1 << WIDX_TITLE_EDITOR_RELOAD) |
        (1 << WIDX_TITLE_EDITOR_SKIP_TO) |
        (1 << WIDX_TITLE_EDITOR_MOVE_DOWN) |
        (1 << WIDX_TITLE_EDITOR_MOVE_UP) |

        (1 << WIDX_TITLE_EDITOR_PLAY) |
        (1 << WIDX_TITLE_EDITOR_STOP) |
        (1 << WIDX_TITLE_EDITOR_REPLAY) |
        (1 << WIDX_TITLE_EDITOR_SKIP);

    window_init_scroll_widgets(window);
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

    window_title_editor_load_sequence(_selectedTitleSequence);
}

static void window_title_editor_close(rct_window * w)
{
    title_stop_previewing_sequence();

    // Close the related windows
    window_close_by_class(WC_TITLE_COMMAND_EDITOR);

    FreeTitleSequence(_editingTitleSequence);
    _editingTitleSequence = nullptr;
    _sequenceName = nullptr;

    SafeFree(_renameSavePath);
}

static void window_title_editor_mouseup(rct_window * w, rct_widgetindex widgetIndex)
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
            // TODO: This should probably be 'NEW'
            window_text_input_open(w, widgetIndex, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_NONE, 0, 64);
        }
        break;
    case WIDX_TITLE_EDITOR_DUPLICATE_BUTTON:
        if (!commandEditorOpen && _editingTitleSequence != nullptr)
        {
            window_text_input_open(w, widgetIndex, STR_TITLE_EDITOR_ACTION_DUPLICATE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_STRING, (uintptr_t)_sequenceName, 64);
        }
        break;
    case WIDX_TITLE_EDITOR_DELETE_BUTTON:
        if (window_title_editor_check_can_edit())
        {
            title_sequence_manager_delete(_selectedTitleSequence);
            window_title_editor_load_sequence(0);
        }
        break;
    case WIDX_TITLE_EDITOR_RENAME_BUTTON:
        if (window_title_editor_check_can_edit() && _editingTitleSequence != nullptr)
        {
            window_text_input_open(w, widgetIndex, STR_TRACK_MANAGE_RENAME, STR_TITLE_EDITOR_ENTER_NAME_FOR_SEQUENCE, STR_STRING, (uintptr_t)_sequenceName, 64);
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
            intent.putExtra(INTENT_EXTRA_CALLBACK, (void *) window_title_editor_add_park_callback);
            context_open_intent(&intent);
        }
        break;
    case WIDX_TITLE_EDITOR_REMOVE_SAVE:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1)
            {
                TitleSequenceRemovePark(_editingTitleSequence, w->selected_list_item);
                if (w->selected_list_item >= (int16_t)_editingTitleSequence->NumSaves)
                {
                    w->selected_list_item--;
                }
            }
        }
        break;
    case WIDX_TITLE_EDITOR_RENAME_SAVE:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1)
            {
                window_text_input_open(w, widgetIndex, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_TITLE_EDITOR_ENTER_NAME_FOR_SAVE, STR_STRING, (uintptr_t)_editingTitleSequence->Saves[w->selected_list_item], 52 - 1);
            }
        }
        break;
    case WIDX_TITLE_EDITOR_LOAD_SAVE:
        if (w->selected_list_item >= 0 && w->selected_list_item < (int16_t)_editingTitleSequence->NumSaves)
        {
            auto handle = TitleSequenceGetParkHandle(_editingTitleSequence, w->selected_list_item);
            auto stream = (IStream *)handle->Stream;
            auto hintPath = String::ToStd(handle->HintPath);

            bool isScenario = ParkImporter::ExtensionIsScenario(hintPath);
            auto objectMgr = OpenRCT2::GetContext()->GetObjectManager();
            auto parkImporter = std::unique_ptr<IParkImporter>(ParkImporter::Create(hintPath));
            auto result = parkImporter->LoadFromStream(stream, isScenario);
            objectMgr->LoadObjects(result.RequiredObjects.data(), result.RequiredObjects.size());
            parkImporter->Import();

            if (isScenario)
                scenario_begin();
            else
                game_load_init();

            TitleSequenceCloseParkHandle(handle);
            window_title_editor_open(WINDOW_TITLE_EDITOR_TAB_SAVES);
        }
        break;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Commands tab
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    case WIDX_TITLE_EDITOR_INSERT:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1)
                window_title_command_editor_open(_editingTitleSequence, w->selected_list_item + 1, true);
            else
                window_title_command_editor_open(_editingTitleSequence, (int32_t)_editingTitleSequence->NumCommands, true);
        }
        break;
    case WIDX_TITLE_EDITOR_EDIT:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1 && w->selected_list_item < (int16_t)_editingTitleSequence->NumCommands)
            {
                window_title_command_editor_open(_editingTitleSequence, w->selected_list_item, false);
            }
        }
        break;
    case WIDX_TITLE_EDITOR_DELETE:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1 && w->selected_list_item < (int16_t)_editingTitleSequence->NumCommands)
            {
                for (int32_t i = w->selected_list_item; i < (int16_t)_editingTitleSequence->NumCommands - 1; i++)
                {
                    _editingTitleSequence->Commands[i] = _editingTitleSequence->Commands[i + 1];
                }
                _editingTitleSequence->NumCommands--;
                if (w->selected_list_item >= (int16_t)_editingTitleSequence->NumCommands)
                {
                    w->selected_list_item--;
                }
                TitleSequenceSave(_editingTitleSequence);
            }
        }
        break;
    case WIDX_TITLE_EDITOR_SKIP_TO:
    {
        int32_t position = w->selected_list_item;
        if (title_is_previewing_sequence() && position != -1 && position < (int32_t)_editingTitleSequence->NumCommands)
        {
            auto player = window_title_editor_get_player();
            player->Seek(position);
            player->Update();
        }
        break;
    }
    case WIDX_TITLE_EDITOR_MOVE_DOWN:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item != -1 && w->selected_list_item < (int16_t)_editingTitleSequence->NumCommands - 1)
            {
                TitleCommand * a = &_editingTitleSequence->Commands[w->selected_list_item];
                TitleCommand * b = &_editingTitleSequence->Commands[w->selected_list_item + 1];
                TitleCommand tmp = *a;
                *a = *b;
                *b = tmp;
                w->selected_list_item++;
                TitleSequenceSave(_editingTitleSequence);
            }
        }
        break;
    case WIDX_TITLE_EDITOR_MOVE_UP:
        if (window_title_editor_check_can_edit())
        {
            if (w->selected_list_item > 0 && w->selected_list_item < (int16_t)_editingTitleSequence->NumCommands)
            {
                TitleCommand * a = &_editingTitleSequence->Commands[w->selected_list_item - 1];
                TitleCommand * b = &_editingTitleSequence->Commands[w->selected_list_item];
                TitleCommand tmp = *b;
                *b = *a;
                *a = tmp;
                w->selected_list_item--;
                TitleSequenceSave(_editingTitleSequence);
            }
        }
        break;
    case WIDX_TITLE_EDITOR_REPLAY:
        if (title_is_previewing_sequence())
        {
            auto player = window_title_editor_get_player();
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
                context_show_error(STR_ERR_FAILED_TO_LOAD_TITLE_SEQUENCE, STR_NONE);
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
            auto player = window_title_editor_get_player();
            int32_t position = player->GetCurrentPosition() + 1;
            if (position >= (int32_t)_editingTitleSequence->NumCommands)
            {
                position = 0;
            }
            player->Seek(position);
            player->Update();
        }
        break;
    }
}

static void window_title_editor_resize(rct_window * w)
{
    if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_PRESETS)
        window_set_resize(w, WW, WH, 500, WH2);
    else
        window_set_resize(w, WW, WH, 500, 580);
}

static void window_title_editor_mousedown(rct_window * w, rct_widgetindex widgetIndex, rct_widget * widget)
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
            window_invalidate(w);
        }
        break;
    }
    case WIDX_TITLE_EDITOR_PRESETS_DROPDOWN:
        if (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr)
        {
            context_show_error(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE);
        }
        else
        {
            int32_t numItems = (int32_t)title_sequence_manager_get_count();
            for (int32_t i = 0; i < numItems; i++)
            {
                gDropdownItemsFormat[i] = STR_OPTIONS_DROPDOWN_ITEM;
                gDropdownItemsArgs[i] = (uintptr_t)title_sequence_manager_get_name(i);
            }

            widget--;
            window_dropdown_show_text_custom_width(
                w->x + widget->left,
                w->y + widget->top,
                widget->bottom - widget->top + 1,
                w->colours[1],
                0,
                DROPDOWN_FLAG_STAY_OPEN,
                numItems,
                widget->right - widget->left - 3);
            dropdown_set_checked((int32_t)_selectedTitleSequence, true);
        }
        break;
    }
}

static void window_title_editor_dropdown(rct_window * w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    if (widgetIndex == WIDX_TITLE_EDITOR_PRESETS_DROPDOWN)
    {
        window_title_editor_load_sequence(dropdownIndex);
        window_invalidate(w);
    }
}

static void window_title_editor_update(rct_window * w)
{
    w->frame_no++;
    if (w->frame_no >= window_title_editor_tab_animation_loops[w->selected_tab])
    {
        w->frame_no = 0;
    }

    if (!widget_is_highlighted(w, WIDX_TITLE_EDITOR_LIST))
    {
        _window_title_editor_highlighted_index = -1;
        widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
    }

    widget_invalidate(w, WIDX_TITLE_EDITOR_PRESETS_TAB + w->selected_tab);
}

static void window_title_editor_scrollgetsize(rct_window * w, int32_t scrollIndex, int32_t * width, int32_t * height)
{
    size_t lineCount = 1;
    if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SAVES)
        lineCount = _editingTitleSequence->NumSaves;
    else if (w->selected_tab == WINDOW_TITLE_EDITOR_TAB_SCRIPT)
        lineCount = _editingTitleSequence->NumCommands;

    *height = (int32_t)(lineCount * SCROLLABLE_ROW_HEIGHT);
    int32_t i = *height - window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].bottom + window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].top + 21;
    if (i < 0)
    {
        i = 0;
    }
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        window_invalidate(w);
    }

    *width = SCROLL_WIDTH;
}

static void window_title_editor_scrollmousedown(rct_window * w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t index = y / SCROLLABLE_ROW_HEIGHT;
    w->selected_list_item = -1;
    switch (w->selected_tab)
    {
    case WINDOW_TITLE_EDITOR_TAB_SAVES:
        if (index < (int32_t)_editingTitleSequence->NumSaves)
        {
            w->selected_list_item = index;
            widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
        }
        break;
    case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
        if (index < (int32_t)_editingTitleSequence->NumCommands)
        {
            w->selected_list_item = index;
            widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
        }
        break;
    }
}

static void window_title_editor_scrollmouseover(rct_window * w, int32_t scrollIndex, int32_t x, int32_t y)
{
    int32_t index = y / SCROLLABLE_ROW_HEIGHT;
    switch (w->selected_tab)
    {
    case WINDOW_TITLE_EDITOR_TAB_SAVES:
        if (index < (int32_t)_editingTitleSequence->NumSaves)
            _window_title_editor_highlighted_index = (int16_t)index;
        break;
    case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
        if (index < (int32_t)_editingTitleSequence->NumCommands)
            _window_title_editor_highlighted_index = (int16_t)index;
        break;
    }
    widget_invalidate(w, WIDX_TITLE_EDITOR_LIST);
}

static void window_title_editor_textinput(rct_window * w, rct_widgetindex widgetIndex, char * text)
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
                        window_title_editor_load_sequence(newIndex);
                    }
                    else if (widgetIndex == WIDX_TITLE_EDITOR_DUPLICATE_BUTTON)
                    {
                        size_t newIndex = title_sequence_manager_duplicate(_selectedTitleSequence, text);
                        window_title_editor_load_sequence(newIndex);
                    }
                    else
                    {
                        size_t newIndex = title_sequence_manager_rename(_selectedTitleSequence, text);
                        window_title_editor_load_sequence(newIndex);
                    }
                    config_save_default();
                    window_invalidate(w);
                }
                else
                {
                    context_show_error(STR_ERROR_RESERVED_NAME, STR_NONE);
                }
            }
            else
            {
                context_show_error(STR_ERROR_EXISTING_NAME, STR_NONE);
            }
        }
        else
        {
            context_show_error(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
        }
        break;
    case WIDX_TITLE_EDITOR_RENAME_SAVE:
        window_title_editor_rename_park(w->selected_list_item, text);
        break;
    }
}

static void window_title_editor_tooltip(rct_window * w, rct_widgetindex widgetIndex, rct_string_id * stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

static void window_title_editor_invalidate(rct_window * w)
{
    int32_t pressed_widgets = w->pressed_widgets & ~(
            (1LL << WIDX_TITLE_EDITOR_PRESETS_TAB) |
            (1LL << WIDX_TITLE_EDITOR_SAVES_TAB) |
            (1LL << WIDX_TITLE_EDITOR_SCRIPT_TAB)
    );
    rct_widgetindex widgetIndex = w->selected_tab + WIDX_TITLE_EDITOR_PRESETS_TAB;

    w->pressed_widgets = pressed_widgets | (1 << widgetIndex);

    window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_EMPTY;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WWT_EMPTY;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WWT_EMPTY;

    window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_EMPTY;
    //window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_EMPTY;
    window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_EMPTY;

    switch (w->selected_tab)
    {
    case WINDOW_TITLE_EDITOR_TAB_PRESETS:
        window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].type = WWT_DROPDOWN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_NEW_BUTTON].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_DUPLICATE_BUTTON].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE_BUTTON].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_BUTTON].type = WWT_BUTTON;
        break;
    case WINDOW_TITLE_EDITOR_TAB_SAVES:
        window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_ADD_SAVE].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_REMOVE_SAVE].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_RENAME_SAVE].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_LOAD_SAVE].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_IMGBTN;
        break;
    case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
        window_title_editor_widgets[WIDX_TITLE_EDITOR_LIST].type = WWT_SCROLL;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_INSERT].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_EDIT].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_DELETE].type = WWT_BUTTON;
        //window_title_editor_widgets[WIDX_TITLE_EDITOR_RELOAD].type = WWT_DROPDOWN_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP_TO].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_UP].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_MOVE_DOWN].type = WWT_BUTTON;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_PLAY].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_STOP].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_REPLAY].type = WWT_IMGBTN;
        window_title_editor_widgets[WIDX_TITLE_EDITOR_SKIP].type = WWT_IMGBTN;
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
        w->disabled_widgets |= (1 << WIDX_TITLE_EDITOR_PLAY);
    else
        w->disabled_widgets &= ~(1 << WIDX_TITLE_EDITOR_PLAY);
    if (!title_is_previewing_sequence())
        w->disabled_widgets |= (1 << WIDX_TITLE_EDITOR_REPLAY) | (1 << WIDX_TITLE_EDITOR_STOP) | (1 << WIDX_TITLE_EDITOR_SKIP) | (1 << WIDX_TITLE_EDITOR_SKIP_TO);
    else
        w->disabled_widgets &= ~((1 << WIDX_TITLE_EDITOR_REPLAY) | (1 << WIDX_TITLE_EDITOR_STOP) | (1 << WIDX_TITLE_EDITOR_SKIP) | (1 << WIDX_TITLE_EDITOR_SKIP_TO));
}

static void window_title_editor_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
    // Widgets
    window_draw_widgets(w, dpi);
    window_title_editor_draw_tab_images(dpi, w);

    // Draw strings
    switch (w->selected_tab)
    {
    case WINDOW_TITLE_EDITOR_TAB_PRESETS:
        set_format_arg(0, uintptr_t, _sequenceName);
        gfx_draw_string_left(dpi, STR_TITLE_SEQUENCE, nullptr, w->colours[1], w->x + 10, w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top + 1);
        gfx_draw_string_left_clipped(
            dpi,
            STR_STRING,
            gCommonFormatArgs,
            w->colours[1],
            w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left + 1,
            w->y + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].top,
            w->x + window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS_DROPDOWN].left - window_title_editor_widgets[WIDX_TITLE_EDITOR_PRESETS].left - 4);
        break;
    case WINDOW_TITLE_EDITOR_TAB_SAVES:
        break;
    case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
        break;
    }
}

static void window_title_editor_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int32_t scrollIndex)
{
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, ColourMapA[w->colours[1]].mid_light);
    switch (w->selected_tab)
    {
    case WINDOW_TITLE_EDITOR_TAB_SAVES:
        window_title_editor_scrollpaint_saves(w, dpi);
        break;
    case WINDOW_TITLE_EDITOR_TAB_SCRIPT:
        window_title_editor_scrollpaint_commands(w, dpi);
        break;
    }
}

static void window_title_editor_scrollpaint_saves(rct_window * w, rct_drawpixelinfo * dpi)
{
    int32_t currentSaveIndex = -1;
    // ITitleSequencePlayer * player = window_title_editor_get_player();
    // int32_t position = title_sequence_player_get_current_position(player);
    // const TitleCommand * command = &_editingTitleSequence->Commands[position];

    int32_t x = 0;
    int32_t y = 0;
    if (_editingTitleSequence == nullptr)
        return;

    for (int32_t i = 0; i < (int32_t)_editingTitleSequence->NumSaves; i++, y += SCROLLABLE_ROW_HEIGHT)
    {
        bool selected = false;
        bool hover = false;
        if (i == w->selected_list_item)
        {
            selected = true;
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].dark);
        }
        else if (i == _window_title_editor_highlighted_index || i == currentSaveIndex)
        {
            hover = true;
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark);
        }
        else if (i & 1)
        {
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);
        }

        char buffer[256];
        set_format_arg(0, uintptr_t, _editingTitleSequence->Saves[i]);
        if (selected || hover)
        {
            format_string(buffer, 256, STR_STRING, gCommonFormatArgs);
        }
        else
        {
            format_string(buffer + 1, 255, STR_STRING, gCommonFormatArgs);
            buffer[0] = (utf8)(uint8_t)FORMAT_BLACK;
        }
        set_format_arg(0, uintptr_t, &buffer);
        gfx_draw_string_left(dpi, STR_STRING, gCommonFormatArgs, w->colours[1], x + 5, y);
    }
}

static void window_title_editor_scrollpaint_commands(rct_window * w, rct_drawpixelinfo * dpi)
{
    int32_t position = -1;
    if (title_is_previewing_sequence() && _selectedTitleSequence == title_get_current_sequence())
    {
        auto player = window_title_editor_get_player();
        position = player->GetCurrentPosition();
    }

    int32_t x = 0;
    int32_t y = 0;
    for (int32_t i = 0; i < (int32_t)_editingTitleSequence->NumCommands; i++, y += SCROLLABLE_ROW_HEIGHT)
    {
        TitleCommand * command = &_editingTitleSequence->Commands[i];
        bool selected = false;
        bool hover = false;
        bool error = false;
        if (i == w->selected_list_item)
        {
            selected = true;
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].dark);
        }
        else if (i == (int32_t)_window_title_editor_highlighted_index || i == position)
        {
            hover = true;
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark);
        }
        else if (i & 1)
        {
            gfx_fill_rect(dpi, x, y, x + SCROLL_WIDTH + 100, y + SCROLLABLE_ROW_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);
        }

        rct_string_id commandName = STR_NONE;
        switch (command->Type)
        {
        case TITLE_SCRIPT_LOAD:
            commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
            if (command->SaveIndex == SAVE_INDEX_INVALID)
            {
                commandName = STR_TITLE_EDITOR_COMMAND_LOAD_NO_SAVE;
                error = true;
            }
            else
            {
                set_format_arg(0, uintptr_t, _editingTitleSequence->Saves[command->SaveIndex]);
            }
            break;
        case TITLE_SCRIPT_LOADMM:
            commandName = STR_TITLE_EDITOR_COMMAND_LOAD_SFMM;
            break;
        case TITLE_SCRIPT_LOCATION:
            commandName = STR_TITLE_EDITOR_COMMAND_LOCATION;
            set_format_arg(0, uint16_t, command->X);
            set_format_arg(2, uint16_t, command->Y);
            break;
        case TITLE_SCRIPT_ROTATE:
            commandName = STR_TITLE_EDITOR_COMMAND_ROTATE;
            set_format_arg(0, uint16_t, command->Rotations);
            break;
        case TITLE_SCRIPT_ZOOM:
            commandName = STR_TITLE_EDITOR_COMMAND_ZOOM;
            set_format_arg(0, uint16_t, command->Zoom);
            break;
        case TITLE_SCRIPT_SPEED:
            commandName = STR_TITLE_EDITOR_COMMAND_SPEED;
            set_format_arg(0, rct_string_id, SpeedNames[command->Speed - 1]);
            break;
        case TITLE_SCRIPT_FOLLOW:
            commandName = STR_TITLE_EDITOR_COMMAND_FOLLOW;
            if (command->SpriteIndex == SPRITE_INDEX_NULL)
            {
                commandName = STR_TITLE_EDITOR_COMMAND_FOLLOW_NO_SPRITE;
            }
            else
            {
                set_format_arg(0, uintptr_t, (uintptr_t)command->SpriteName);
            }
            break;
        case TITLE_SCRIPT_WAIT:
            commandName = STR_TITLE_EDITOR_COMMAND_WAIT;
            set_format_arg(0, uint16_t, command->Milliseconds);
            break;
        case TITLE_SCRIPT_RESTART:
            commandName = STR_TITLE_EDITOR_RESTART;
            break;
        case TITLE_SCRIPT_END:
            commandName = STR_TITLE_EDITOR_END;
            break;
        case TITLE_SCRIPT_LOADRCT1:
        {
            commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
            const char * name = "";
            source_desc desc;
            if (scenario_get_source_desc_by_id(command->SaveIndex, &desc))
            {
                name = desc.title;
            }
            set_format_arg(0, uintptr_t, name);
            break;
        }
        case TITLE_SCRIPT_LOADSC:
        {
            commandName = STR_TITLE_EDITOR_COMMAND_LOAD_FILE;
            const char * name = "";
            auto scenario =
                GetScenarioRepository()->GetByInternalName(command->Scenario);
            if (command->Scenario[0] == '\0')
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
            set_format_arg(0, uintptr_t, name);
            break;
        }
        default:
            log_warning("Unknown command %d", command->Type);
        }

        char buffer[256];
        if ((selected || hover) && !error)
        {
            format_string(buffer, 256, commandName, gCommonFormatArgs);
        }
        else
        {
            format_string(buffer + 1, 255, commandName, gCommonFormatArgs);
            buffer[0] = (utf8)(error ? ((selected || hover) ? FORMAT_LIGHTPINK : FORMAT_RED) : FORMAT_BLACK);
        }
        set_format_arg(0, uintptr_t, &buffer);
        gfx_draw_string_left(dpi, STR_STRING, gCommonFormatArgs, w->colours[1], x + 5, y);
    }
}

static void window_title_editor_draw_tab_images(rct_drawpixelinfo * dpi, rct_window * w)
{
    for (int32_t i = 0; i < WINDOW_TITLE_EDITOR_TAB_COUNT; i++)
    {
        int32_t x = 0;
        int32_t y = 0;
        int32_t spriteId = window_title_editor_tab_sprites[i];
        if (w->selected_tab == i)
        {
            spriteId += w->frame_no / window_title_editor_tab_animation_divisor[w->selected_tab];
        }
        if (i == 1)
        {
            x = 4;
            y = 1;
        }
        gfx_draw_sprite(dpi, spriteId, w->x + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].left + x, w->y + w->widgets[WIDX_TITLE_EDITOR_PRESETS_TAB + i].top + y, 0);
    }
}

static void window_title_editor_load_sequence(size_t index)
{
    if (index >= title_sequence_manager_get_count())
        return;

    const char * path = title_sequence_manager_get_path(index);
    TitleSequence * titleSequence = LoadTitleSequence(path);
    if (titleSequence == nullptr)
    {
        context_show_error(STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, STR_NONE);
        return;
    }

    _selectedTitleSequence = index;
    size_t predefinedIndex = title_sequence_manager_get_predefined_index(index);
    _isSequenceReadOnly = (predefinedIndex != SIZE_MAX);
    _sequenceName = title_sequence_manager_get_name(index);
    FreeTitleSequence(_editingTitleSequence);
    _editingTitleSequence = titleSequence;

    window_close_by_class(WC_TITLE_COMMAND_EDITOR);
}

static ITitleSequencePlayer * window_title_editor_get_player()
{
    return (ITitleSequencePlayer *)title_get_sequence_player();
}

static bool window_title_editor_check_can_edit()
{
    bool commandEditorOpen = (window_find_by_class(WC_TITLE_COMMAND_EDITOR) != nullptr);

    if (_isSequenceReadOnly)
        context_show_error(STR_ERROR_CANT_CHANGE_TITLE_SEQUENCE, STR_NONE);
    else if (title_is_previewing_sequence())
        context_show_error(STR_TITLE_EDITOR_ERR_CANT_EDIT_WHILE_PLAYING, STR_TITLE_EDITOR_PRESS_STOP_TO_CONTINUE_EDITING);
    else if (commandEditorOpen)
        context_show_error(STR_TITLE_EDITOR_ERR_CANT_CHANGE_WHILE_EDITOR_IS_OPEN, STR_NONE);
    else
        return true;

    return false;
}

static bool save_filename_exists(const utf8 * filename)
{
    TitleSequence * seq = _editingTitleSequence;
    for (size_t i = 0; i < seq->NumSaves; i++)
    {
        const utf8 * savePath = seq->Saves[i];

        if (_stricmp(savePath, filename) == 0)
            return true;
    }
    return false;
}

static void window_title_editor_add_park_callback(int32_t result, const utf8 * path)
{
    uint32_t extension = get_file_extension_type(path);
    if (extension != FILE_EXTENSION_SV4 && extension != FILE_EXTENSION_SV6)
        return;

    const utf8 * filename = path_get_filename(path);
    if (save_filename_exists(filename))
    {
        free(_renameSavePath);
        _renameSavePath = _strdup(filename);
        // set_format_arg(0, intptr_t, (intptr_t)&_renameSavePath);
        rct_window * w = window_find_by_class(WC_TITLE_EDITOR);
        window_text_input_open(w, WIDX_TITLE_EDITOR_RENAME_SAVE, STR_FILEBROWSER_RENAME_SAVE_TITLE, STR_ERROR_EXISTING_NAME, STR_STRING, (uintptr_t)_renameSavePath, 52 - 1);
        return;
    }

    TitleSequenceAddPark(_editingTitleSequence, path, filename);
}

static void window_title_editor_rename_park(size_t index, const utf8 * name)
{
    if (!filename_valid_characters(name))
    {
        context_show_error(STR_ERROR_INVALID_CHARACTERS, STR_NONE);
        return;
    }

    for (size_t i = 0; i < _editingTitleSequence->NumSaves; i++)
    {
        if (i != index)
        {
            const utf8 * savePath = _editingTitleSequence->Saves[i];
            if (_strcmpi(savePath, name) == 0)
            {
                context_show_error(STR_ERROR_EXISTING_NAME, STR_NONE);
                return;
            }
        }
    }

    if (TitleSequenceRenamePark(_editingTitleSequence, index, name))
    {
        TitleSequenceSave(_editingTitleSequence);
    }
}
