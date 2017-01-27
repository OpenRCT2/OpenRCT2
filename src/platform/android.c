#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifdef __ANDROID__

#include "platform.h"
#include <wchar.h>

int mbtowc(wchar_t *__restrict pwc, const char *__restrict s, size_t n) {
    static mbstate_t mbs;
    size_t rval;
    if (s == NULL) {
        /* No support for state dependent encodings. */
        memset(&mbs, 0, sizeof(mbs));
        return (0);
    }
    rval = mbrtowc(pwc, s, n, &mbs);
    if (rval == (size_t) -1 || rval == (size_t) -2)
        return (-1);
    return ((int) rval);
}

bool platform_check_steam_overlay_attached() {
    STUB();
    return false;
}

bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc) {
    STUB();
    return false;
}

void platform_get_exe_path(utf8 *outPath)
{
    strcpy(outPath, "/sdcard/openrct2");
}

void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator) {
    strcpy(buffer, "/sdcard/openrct2-user/");
}

void platform_show_messagebox(char *message)
{
    STUB();
}

utf8 *platform_open_directory_browser(utf8 *title)
{
    log_info(title);
    STUB();
    return "/sdcard/rct2";
}

bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer)
{
    STUB();
    return false;
}

void platform_posix_sub_resolve_openrct_data_path(utf8 *out) {
    strcpy(out, "/sdcard/openrct2");
}

uint8 platform_get_locale_currency(){
	return platform_get_currency_value(NULL);
}

uint8 platform_get_locale_measurement_format(){
	return MEASUREMENT_FORMAT_METRIC;
}

#endif
