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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/core/String.hpp>
#include "TextComposition.h"

extern "C"
{
    #include <openrct2/interface/console.h>
    #include <openrct2/interface/window.h>
    #include <openrct2/localisation/localisation.h>
}

#ifdef __MACOSX__
    // macOS uses COMMAND rather than CTRL for many keyboard shortcuts
    #define KEYBOARD_PRIMARY_MODIFIER KMOD_GUI
#else
    #define KEYBOARD_PRIMARY_MODIFIER KMOD_CTRL
#endif

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

bool TextComposition::IsActive()
{
    return SDL_IsTextInputActive() && _session.Buffer != nullptr;
}

TextInputSession * TextComposition::Start(utf8 * buffer, size_t bufferSize)
{
    Guard::ArgumentNotNull(buffer);

    // TODO This doesn't work, and position could be improved to where text entry is
    SDL_Rect rect = { 10, 10, 100, 100 };
    SDL_SetTextInputRect(&rect);
    SDL_StartTextInput();

    _session.Buffer = buffer;
    _session.BufferSize = bufferSize - 1;
    _session.Size = strlen(buffer);
    _session.SelectionStart = _session.Size;
    _session.SelectionSize = 0;
    RecalculateLength();
    return &_session;
}

void TextComposition::Stop()
{
    SDL_StopTextInput();
    _session.Buffer = nullptr;
    _imeActive = false;
}

void TextComposition::HandleMessage(const SDL_Event * e)
{
    switch (e->type) {
    case SDL_TEXTEDITING:
        // When inputting Korean characters, `edit.length` is always zero
        String::Set(_imeBuffer, sizeof(_imeBuffer), e->edit.text);
        _imeStart = e->edit.start;
        _imeLength = e->edit.length;
        _imeActive = ((e->edit.length != 0 || String::SizeOf(e->edit.text) != 0) && _imeBuffer[0] != '\0');
        break;
    case SDL_TEXTINPUT:
        // will receive an `SDL_TEXTINPUT` event when a composition is committed
        _imeActive = false;
        if (_session.Buffer != nullptr)
        {
            // HACK ` will close console, so don't input any text
            if (e->text.text[0] == '`' && gConsoleOpen) {
                break;
            }

            utf8 * newText = String::Duplicate(e->text.text);
            utf8_remove_formatting(newText, false);
            Insert(newText);
            Memory::Free(newText);

            console_refresh_caret();
            window_update_textbox();
        }
        break;
    case SDL_KEYDOWN:
    {
        if (_imeActive)
        {
            break;
        }

        uint16 modifier = e->key.keysym.mod;
        SDL_Keycode key = e->key.keysym.sym;
        if (key == SDLK_KP_ENTER)
        {
            // Map Keypad enter to regular enter.
            key = SDLK_RETURN;
        }

        // _lastKeyPressed = e.key.keysym.sym;
        // _keysPressed[e.key.keysym.scancode] = 1;

        // Text input
        if (_session.Buffer == nullptr)
        {
            break;
        }

        // Clear the input on <CTRL>Backspace (Windows/Linux) or <MOD>Backspace (macOS)
        if (key == SDLK_BACKSPACE && (modifier & KEYBOARD_PRIMARY_MODIFIER))
        {
            Clear();
            console_refresh_caret();
            window_update_textbox();
        }

        switch (key) {
        case SDLK_BACKSPACE:
            // If backspace and we have input text with a cursor position none zero
            if (_session.SelectionStart > 0)
            {
                size_t endOffset = _session.SelectionStart;
                CursorLeft();
                _session.SelectionSize = endOffset - _session.SelectionStart;
                Delete();

                console_refresh_caret();
                window_update_textbox();
            }
            break;
        case SDLK_HOME:
            CursorHome();
            console_refresh_caret();
            break;
        case SDLK_END:
            CursorEnd();
            console_refresh_caret();
            break;
        case SDLK_DELETE:
        {
            size_t startOffset = _session.SelectionStart;
            CursorRight();
            _session.SelectionSize = _session.SelectionStart - startOffset;
            _session.SelectionStart = startOffset;
            Delete();
            console_refresh_caret();
            window_update_textbox();
            break;
        }
        case SDLK_RETURN:
            window_cancel_textbox();
            break;
        case SDLK_LEFT:
            CursorLeft();
            console_refresh_caret();
            break;
        case SDLK_RIGHT:
            CursorRight();
            console_refresh_caret();
            break;
        case SDLK_v:
            if ((modifier & KEYBOARD_PRIMARY_MODIFIER) && SDL_HasClipboardText())
            {
                utf8 * text = SDL_GetClipboardText();
                utf8_remove_formatting(text, false);
                Insert(text);
                SDL_free(text);
                window_update_textbox();
            }
            break;
        }
    }
    }
}

void TextComposition::CursorHome()
{
    _session.SelectionStart = 0;
}

void TextComposition::CursorEnd()
{
    _session.SelectionStart = _session.SelectionSize;
}

void TextComposition::CursorLeft()
{
    size_t selectionOffset = _session.SelectionStart;
    if (selectionOffset > 0)
    {
        const utf8 * ch = _session.Buffer + selectionOffset;
        do
        {
            ch--;
            selectionOffset--;
        }
        while (!utf8_is_codepoint_start(ch) && selectionOffset > 0);

        _session.SelectionStart = selectionOffset;
    }
}

void TextComposition::CursorRight()
{
    size_t selectionOffset = _session.SelectionStart;
    size_t selectionMaxOffset = _session.Size;
    if (selectionOffset < selectionMaxOffset)
    {
        const utf8 * ch = _session.Buffer + _session.SelectionStart;
        do
        {
            ch++;
            selectionOffset++;
        }
        while (!utf8_is_codepoint_start(ch) && selectionOffset < selectionMaxOffset);

        _session.SelectionSize = Math::Max<size_t>(0, _session.SelectionSize - (selectionOffset - _session.SelectionStart));
        _session.SelectionStart = selectionOffset;
    }
}

void TextComposition::Insert(const utf8 * text)
{
    const utf8 * ch = text;
    uint32 codepoint;
    while ((codepoint = utf8_get_next(ch, &ch)) != 0)
    {
        InsertCodepoint(codepoint);
    }
}

void TextComposition::InsertCodepoint(codepoint_t codepoint)
{
    size_t codepointLength = utf8_get_codepoint_length(codepoint);
    size_t remainingSize = _session.BufferSize - _session.Size;
    if (codepointLength <= remainingSize)
    {
        utf8 * buffer = _session.Buffer;
        utf8 * insertPtr = buffer + _session.SelectionStart;
        if (_session.SelectionStart < _session.Size)
        {
            // Shift bytes (including null terminator) right to make room for new codepoint
            utf8 * targetShiftPtr = insertPtr + codepointLength;
            size_t shiftSize = _session.Size - _session.SelectionStart + 1;
            memmove(targetShiftPtr, insertPtr, shiftSize);
        }
        else
        {
            // Character is appended onto the end, so set byte after it to null terminator
            buffer[_session.Size + codepointLength] = 0;
        }

        utf8_write_codepoint(insertPtr, codepoint);
        _session.SelectionStart += codepointLength;
        _session.Size += codepointLength;
        _session.Length++;
    }
}

void TextComposition::Clear()
{
    utf8 * buffer = _session.Buffer;
    buffer[0] = 0;
    _session.Size = 0;
    _session.Length = 0;
    _session.SelectionStart = 0;
    _session.SelectionSize = 0;
}

void TextComposition::Delete()
{
    utf8 * buffer = _session.Buffer;
    utf8 * targetShiftPtr = buffer + _session.SelectionStart;
    utf8 * sourceShiftPtr = targetShiftPtr + _session.SelectionSize;
    size_t shiftSize = _session.SelectionSize - _session.SelectionStart - _session.SelectionSize + 1;
    memmove(targetShiftPtr, sourceShiftPtr, shiftSize);
    _session.SelectionSize = 0;
    RecalculateLength();
}

void TextComposition::RecalculateLength()
{
    _session.Size = String::SizeOf(_session.Buffer);
    _session.Length = String::LengthOf(_session.Buffer);
}
