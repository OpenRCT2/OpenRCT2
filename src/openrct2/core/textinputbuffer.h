#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#ifndef _TEXTINPUTBUFFER_H_
#define _TEXTINPUTBUFFER_H_

#include "../common.h"

typedef struct textinputbuffer {
    utf8 * buffer;
    size_t max_size;            // Maximum number of bytes (excluding null terminator)
    size_t current_size;        // Number of bytes (excluding null terminator)

    uint32 length;              // Number of codepoints

    size_t selection_offset;    // Selection start, in bytes
    size_t selection_size;      // Selection length in bytes
} textinputbuffer;

void textinputbuffer_init(textinputbuffer * tib, utf8 * buffer, size_t size);
void textinputbuffer_clear(textinputbuffer * tib);
void textinputbuffer_remove_selected(textinputbuffer * tib);
void textinputbuffer_insert_codepoint(textinputbuffer * tib, uint32 codepoint);
void textinputbuffer_insert(textinputbuffer * tib, utf8 * source);
void textinputbuffer_cursor_left(textinputbuffer * tib);
void textinputbuffer_cursor_right(textinputbuffer * tib);
void textinputbuffer_cursor_home(textinputbuffer * tib);
void textinputbuffer_cursor_end(textinputbuffer * tib);
void textinputbuffer_recalculate_length(textinputbuffer * tib);

#endif
