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

#include "../localisation/localisation.h"
#include "textinputbuffer.h"

void textinputbuffer_init(textinputbuffer * tib, utf8 * buffer, size_t size)
{
    assert(buffer != NULL);
    assert(size > 0);

    tib->buffer = buffer;
    tib->max_size = size - 1;
    tib->current_size = strlen(buffer);
    tib->selection_offset = tib->current_size;
    tib->selection_size = 0;

    textinputbuffer_recalculate_length(tib);
}

void textinputbuffer_clear(textinputbuffer * tib)
{
    tib->buffer[0] = 0;
    tib->current_size = 0;
    tib->length = 0;
    tib->selection_offset = 0;
    tib->selection_size = 0;
}

void textinputbuffer_remove_selected(textinputbuffer * tib)
{
    utf8 * targetShiftPtr = tib->buffer + tib->selection_offset;
    utf8 * sourceShiftPtr = targetShiftPtr + tib->selection_size;
    size_t shiftSize = tib->current_size - tib->selection_offset - tib->selection_size + 1;
    memmove(targetShiftPtr, sourceShiftPtr, shiftSize);
    tib->selection_size = 0;
    textinputbuffer_recalculate_length(tib);
}

void textinputbuffer_insert_codepoint(textinputbuffer * tib, uint32 codepoint)
{
    size_t codepointLength = utf8_get_codepoint_length(codepoint);
    size_t remainingSize = tib->max_size - tib->current_size;
    if (codepointLength <= remainingSize) {
        utf8 * insertPtr = tib->buffer + tib->selection_offset;

        if (tib->selection_offset < tib->current_size) {
            // Shift bytes (including null terminator) right to make room for new codepoint
            utf8 * targetShiftPtr = insertPtr + codepointLength;
            size_t shiftSize = tib->current_size - tib->selection_offset + 1;
            memmove(targetShiftPtr, insertPtr, shiftSize);
        } else {
            // Character is appended onto the end, so set byte after it to null terminator
            tib->buffer[tib->current_size + codepointLength] = 0;
        }

        utf8_write_codepoint(insertPtr, codepoint);
        tib->selection_offset += codepointLength;
        tib->current_size += codepointLength;
        tib->length++;
    }
}

void textinputbuffer_insert(textinputbuffer * tib, utf8 * source)
{
    const utf8 *ch = source;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
        textinputbuffer_insert_codepoint(tib, codepoint);
    }
}

void textinputbuffer_cursor_left(textinputbuffer * tib)
{
    size_t selectionOffset = tib->selection_offset;
    if (selectionOffset > 0) {
        const utf8 * ch = tib->buffer + selectionOffset;
        do {
            ch--;
            selectionOffset--;
        } while (!utf8_is_codepoint_start(ch) && selectionOffset > 0);

        tib->selection_offset = selectionOffset;
    }
}

void textinputbuffer_cursor_right(textinputbuffer * tib)
{
    size_t selectionOffset = tib->selection_offset;
    size_t selectionMaxOffset = tib->current_size;
    if (selectionOffset < selectionMaxOffset) {
        const utf8 * ch = tib->buffer + selectionOffset;
        do {
            ch++;
            selectionOffset++;
        } while (!utf8_is_codepoint_start(ch) && selectionOffset < selectionMaxOffset);

        tib->selection_size = max(0, tib->selection_size - (selectionOffset - tib->selection_offset));
        tib->selection_offset = selectionOffset;
    }
}

void textinputbuffer_cursor_home(textinputbuffer * tib)
{
    tib->selection_offset = 0;
}

void textinputbuffer_cursor_end(textinputbuffer * tib)
{
    tib->selection_offset = tib->current_size;
}

void textinputbuffer_recalculate_length(textinputbuffer * tib)
{
    tib->current_size = strlen(tib->buffer);
    tib->length = utf8_length(tib->buffer);
}
