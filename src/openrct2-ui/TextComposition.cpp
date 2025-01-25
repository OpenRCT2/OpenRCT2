/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TextComposition.h"

#include "UiContext.h"
#include "UiStringIds.h"
#include "interface/InGameConsole.h"
#include "interface/Window.h"

#include <SDL.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/core/Memory.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/core/UTF8.h>
#include <openrct2/ui/UiContext.h>

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

TextInputSession* TextComposition::Start(u8string& buffer, size_t maxLength)
{
    SDL_StartTextInput();
    _session.Buffer = &buffer;
    _session.MaxLength = maxLength;
    _session.SelectionStart = buffer.size();
    _session.SelectionSize = 0;
    _session.ImeBuffer = _imeBuffer;
    RecalculateLength();
    return &_session;
}

void TextComposition::Stop()
{
    SDL_StopTextInput();
    _session.Buffer = nullptr;
    _session.ImeBuffer = nullptr;
    _imeActive = false;
}

void TextComposition::HandleMessage(const SDL_Event* e)
{
    auto& console = GetInGameConsole();

    switch (e->type)
    {
        case SDL_TEXTEDITING:
            // When inputting Korean characters, `edit.length` is always zero
            String::set(_imeBuffer, sizeof(_imeBuffer), e->edit.text);
            _imeStart = e->edit.start;
            _imeLength = e->edit.length;
            _imeActive = ((e->edit.length != 0 || String::sizeOf(e->edit.text) != 0) && _imeBuffer[0] != '\0');
            break;
        case SDL_TEXTINPUT:
            // will receive an `SDL_TEXTINPUT` event when a composition is committed
            _imeActive = false;
            _imeBuffer[0] = '\0';
            if (_session.Buffer != nullptr)
            {
                // HACK ` will close console, so don't input any text
                if (e->text.text[0] == '`' && console.IsOpen())
                {
                    break;
                }

                Insert(e->text.text);

                console.RefreshCaret(_session.SelectionStart);
                OpenRCT2::Ui::Windows::WindowUpdateTextbox();
            }
            break;
        case SDL_KEYDOWN:
        {
            if (_imeActive)
            {
                break;
            }

            uint16_t modifier = e->key.keysym.mod;
            SDL_Keycode key = e->key.keysym.sym;
            SDL_Scancode scancode = e->key.keysym.scancode;
            if (key == SDLK_KP_ENTER)
            {
                // Map Keypad enter to regular enter.
                key = SDLK_RETURN;
                scancode = SDL_SCANCODE_RETURN;
            }

            GetContext()->GetUiContext()->SetKeysPressed(key, scancode);

            // Text input
            if (_session.Buffer == nullptr)
            {
                break;
            }

            switch (key)
            {
                case SDLK_BACKSPACE:
                    // If backspace and we have input text with a cursor position nonzero
                    if (_session.SelectionStart > 0)
                    {
                        size_t endOffset = _session.SelectionStart;
                        if (modifier & KEYBOARD_PRIMARY_MODIFIER)
                            CaretMoveToLeftToken();
                        else
                            CaretMoveLeft();
                        _session.SelectionSize = endOffset - _session.SelectionStart;
                        Delete();

                        console.RefreshCaret(_session.SelectionStart);
                        OpenRCT2::Ui::Windows::WindowUpdateTextbox();
                    }
                    break;
                case SDLK_HOME:
                    CaretMoveToStart();
                    console.RefreshCaret(_session.SelectionStart);
                    break;
                case SDLK_END:
                    CaretMoveToEnd();
                    console.RefreshCaret(_session.SelectionStart);
                    break;
                case SDLK_DELETE:
                {
                    size_t startOffset = _session.SelectionStart;
                    if (modifier & KEYBOARD_PRIMARY_MODIFIER)
                        CaretMoveToRightToken();
                    else
                        CaretMoveRight();
                    _session.SelectionSize = _session.SelectionStart - startOffset;
                    _session.SelectionStart = startOffset;
                    Delete();
                    console.RefreshCaret(_session.SelectionStart);
                    OpenRCT2::Ui::Windows::WindowUpdateTextbox();
                    break;
                }
                case SDLK_RETURN:
                    OpenRCT2::Ui::Windows::WindowCancelTextbox();
                    break;
                case SDLK_LEFT:
                    if (modifier & KEYBOARD_PRIMARY_MODIFIER)
                        CaretMoveToLeftToken();
                    else
                        CaretMoveLeft();
                    console.RefreshCaret(_session.SelectionStart);
                    break;
                case SDLK_RIGHT:
                    if (modifier & KEYBOARD_PRIMARY_MODIFIER)
                        CaretMoveToRightToken();
                    else
                        CaretMoveRight();
                    console.RefreshCaret(_session.SelectionStart);
                    break;
                case SDLK_c:
                    if ((modifier & KEYBOARD_PRIMARY_MODIFIER) && _session.Length)
                    {
                        OpenRCT2::GetContext()->GetUiContext()->SetClipboardText(_session.Buffer->c_str());
                        ContextShowError(STR_COPY_INPUT_TO_CLIPBOARD, kStringIdNone, {});
                    }
                    break;
                case SDLK_v:
                    if ((modifier & KEYBOARD_PRIMARY_MODIFIER) && SDL_HasClipboardText())
                    {
                        utf8* text = SDL_GetClipboardText();
                        Insert(text);
                        SDL_free(text);
                        OpenRCT2::Ui::Windows::WindowUpdateTextbox();
                    }
                    break;
            }
        }
    }
}

void TextComposition::CaretMoveToStart()
{
    _session.SelectionStart = 0;
}

void TextComposition::CaretMoveToEnd()
{
    size_t selectionOffset = _session.Buffer->size();
    const utf8* ch = _session.Buffer->c_str() + _session.SelectionStart;
    while (!UTF8IsCodepointStart(ch) && selectionOffset > 0)
    {
        ch--;
        selectionOffset--;
    }

    _session.SelectionStart = selectionOffset;
}

void TextComposition::CaretMoveLeft()
{
    size_t selectionOffset = _session.SelectionStart;
    if (selectionOffset == 0)
        return;

    const utf8* ch = _session.Buffer->c_str() + selectionOffset;
    do
    {
        ch--;
        selectionOffset--;
    } while (!UTF8IsCodepointStart(ch) && selectionOffset > 0);

    _session.SelectionStart = selectionOffset;
}

void TextComposition::CaretMoveRight()
{
    size_t selectionOffset = _session.SelectionStart;
    size_t selectionMaxOffset = _session.Buffer->size();
    if (selectionOffset >= selectionMaxOffset)
        return;

    const utf8* ch = _session.Buffer->c_str() + _session.SelectionStart;
    do
    {
        ch++;
        selectionOffset++;
    } while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset);

    _session.SelectionSize = std::max<size_t>(0, _session.SelectionSize - (selectionOffset - _session.SelectionStart));
    _session.SelectionStart = selectionOffset;
}

static bool isWhitespace(uint32_t cp)
{
    return cp == ' ' || cp == '\t';
}

void TextComposition::CaretMoveToLeftToken()
{
    if (_session.SelectionStart == 0)
        return;

    size_t selectionOffset = _session.SelectionStart - 1;
    size_t lastChar = selectionOffset;

    const utf8* ch = _session.Buffer->c_str() + selectionOffset;

    // Read until first non-whitespace.
    while (selectionOffset > 0)
    {
        while (!UTF8IsCodepointStart(ch) && selectionOffset > 0)
        {
            ch--;
            selectionOffset--;
        }

        auto cp = UTF8GetNext(ch, nullptr);
        if (!isWhitespace(cp))
        {
            lastChar = selectionOffset;
            break;
        }
        if (selectionOffset == 0)
            break;
        ch--;
        selectionOffset--;
    }

    // Skip white spaces.
    while (selectionOffset > 0)
    {
        while (!UTF8IsCodepointStart(ch) && selectionOffset > 0)
        {
            ch--;
            selectionOffset--;
        }

        auto cp = UTF8GetNext(ch, nullptr);
        if (isWhitespace(cp))
            break;

        lastChar = selectionOffset;
        if (selectionOffset == 0)
            break;
        ch--;
        selectionOffset--;
    }

    _session.SelectionSize = _session.SelectionSize - (selectionOffset - _session.SelectionStart);
    _session.SelectionStart = selectionOffset == 0 ? 0 : lastChar;
}

void TextComposition::CaretMoveToRightToken()
{
    size_t selectionOffset = _session.SelectionStart;
    size_t selectionMaxOffset = _session.Buffer->size();

    if (selectionOffset >= selectionMaxOffset)
        return;

    const utf8* ch = _session.Buffer->c_str() + selectionOffset;

    // Find a valid codepoint start.
    while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset)
    {
        ch++;
        selectionOffset++;
    }
    auto cp = UTF8GetNext(ch, nullptr);

    if (isWhitespace(cp))
    {
        // Read until first non-whitespace.
        while (selectionOffset < selectionMaxOffset)
        {
            do
            {
                ch++;
                selectionOffset++;
            } while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset);

            cp = UTF8GetNext(ch, nullptr);
            if (!isWhitespace(cp))
                break;
        }
    }
    else
    {
        // Read until first non-whitespace.
        while (selectionOffset < selectionMaxOffset)
        {
            do
            {
                ch++;
                selectionOffset++;
            } while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset);

            cp = UTF8GetNext(ch, nullptr);
            if (isWhitespace(cp))
                break;
        }

        // Skip white spaces.
        while (selectionOffset < selectionMaxOffset)
        {
            // Read until first non-whitespace.
            do
            {
                ch++;
                selectionOffset++;
            } while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset);

            cp = UTF8GetNext(ch, nullptr);
            if (!isWhitespace(cp))
                break;
        }
    }

    _session.SelectionSize = std::max<size_t>(0, _session.SelectionSize - (selectionOffset - _session.SelectionStart));
    _session.SelectionStart = selectionOffset;
}

void TextComposition::Insert(const utf8* text)
{
    const utf8* ch = text;
    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(ch, &ch)) != 0)
    {
        InsertCodepoint(codepoint);
    }
}

void TextComposition::InsertCodepoint(codepoint_t codepoint)
{
    size_t codepointLength = UTF8GetCodepointLength(codepoint);
    size_t remainingSize = _session.MaxLength - _session.Length;
    if (remainingSize > 0)
    {
        const auto bufSize = _session.Buffer->size();
        _session.Buffer->resize(_session.Buffer->size() + codepointLength);

        // FIXME: Just insert the codepoint into the string, don't use memmove

        utf8* buffer = _session.Buffer->data();
        utf8* insertPtr = buffer + _session.SelectionStart;
        if (_session.SelectionStart < bufSize)
        {
            // Shift bytes to the right to make room for new codepoint
            utf8* targetShiftPtr = insertPtr + codepointLength;
            size_t shiftSize = bufSize - _session.SelectionStart;
            memmove(targetShiftPtr, insertPtr, shiftSize);
        }

        UTF8WriteCodepoint(insertPtr, codepoint);
        _session.SelectionStart += codepointLength;
        _session.Length++;
    }
}

void TextComposition::Clear()
{
    _session.Buffer->clear();
    _session.Length = 0;
    _session.SelectionStart = 0;
    _session.SelectionSize = 0;
}

void TextComposition::Delete()
{
    size_t selectionOffset = _session.SelectionStart;
    size_t selectionMaxOffset = std::min(_session.SelectionStart + _session.SelectionSize, _session.Buffer->size());
    if (selectionOffset >= selectionMaxOffset)
        return;

    // Find out how many bytes to delete.
    const utf8* ch = _session.Buffer->c_str() + _session.SelectionStart;
    while (selectionOffset < selectionMaxOffset)
    {
        do
        {
            ch++;
            selectionOffset++;
        } while (!UTF8IsCodepointStart(ch) && selectionOffset < selectionMaxOffset);
    }

    size_t bytesToSkip = selectionOffset - _session.SelectionStart;
    if (bytesToSkip == 0)
        return;

    _session.Buffer->erase(
        _session.Buffer->begin() + _session.SelectionStart, _session.Buffer->begin() + _session.SelectionStart + bytesToSkip);
    _session.SelectionSize = 0;

    RecalculateLength();
}

void TextComposition::RecalculateLength()
{
    _session.Length = String::lengthOf(_session.Buffer->c_str());
}
