/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2/interface/Window.h>

enum
{
    UITHEME_FLAG_PREDEFINED = 1 << 0,
    UITHEME_FLAG_USE_LIGHTS_RIDE = 1 << 1,
    UITHEME_FLAG_USE_LIGHTS_PARK = 1 << 2,
    UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT = 1 << 3,
    UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR = 1 << 4,
    UITHEME_FLAG_USE_RCTC_RIDE_LIGHTS = 1 << 5,
    UITHEME_FLAG_USE_RCTC_PARK_LIGHTS = 1 << 6
};

void ColourSchemeUpdate(rct_window* window);
void ColourSchemeUpdateAll();
void ColourSchemeUpdateByClass(rct_window* window, WindowClass classification);

void ThemeManagerInitialise();
void ThemeManagerLoadAvailableThemes();
size_t ThemeManagerGetNumAvailableThemes();
const utf8* ThemeManagerGetAvailableThemePath(size_t index);
const utf8* ThemeManagerGetAvailableThemeConfigName(size_t index);
const utf8* ThemeManagerGetAvailableThemeName(size_t index);
size_t ThemeManagerGetAvailableThemeIndex();
void ThemeManagerSetActiveAvailableTheme(size_t index);
size_t ThemeGetIndexForName(const utf8* name);

colour_t ThemeGetColour(WindowClass wc, uint8_t index);
void ThemeSetColour(WindowClass wc, uint8_t index, colour_t colour);
uint8_t ThemeGetFlags();
void ThemeSetFlags(uint8_t flags);
void ThemeSave();
void ThemeRename(const utf8* name);
void ThemeDuplicate(const utf8* name);
void ThemeDelete();

uint8_t ThemeDescGetNumColours(WindowClass wc);
StringId ThemeDescGetName(WindowClass wc);
