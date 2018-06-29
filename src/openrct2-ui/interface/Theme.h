/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2/interface/Window.h>

enum {
    UITHEME_FLAG_PREDEFINED                            = 1 << 0,
    UITHEME_FLAG_USE_LIGHTS_RIDE                       = 1 << 1,
    UITHEME_FLAG_USE_LIGHTS_PARK                       = 1 << 2,
    UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT  = 1 << 3,
    UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR               = 1 << 4,
};

void colour_scheme_update(rct_window *window);
void colour_scheme_update_all();
void colour_scheme_update_by_class(rct_window *window, rct_windowclass classification);

void         theme_manager_initialise();
void         theme_manager_load_available_themes();
size_t       theme_manager_get_num_available_themes();
const utf8 * theme_manager_get_available_theme_path(size_t index);
const utf8 * theme_manager_get_available_theme_config_name(size_t index);
const utf8 * theme_manager_get_available_theme_name(size_t index);
size_t       theme_manager_get_active_available_theme_index();
void         theme_manager_set_active_available_theme(size_t index);
size_t       theme_get_index_for_name(const utf8 * name);

colour_t theme_get_colour(rct_windowclass wc, uint8_t index);
void     theme_set_colour(rct_windowclass wc, uint8_t index, colour_t colour);
uint8_t    theme_get_flags();
void     theme_set_flags(uint8_t flags);
void     theme_save();
void     theme_rename(const utf8 * name);
void     theme_duplicate(const utf8 * name);
void     theme_delete();

uint8_t         theme_desc_get_num_colours(rct_windowclass wc);
rct_string_id theme_desc_get_name(rct_windowclass wc);
