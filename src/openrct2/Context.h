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

#pragma once

#include "common.h"

typedef struct CursorState
{
    sint32  x, y;
    uint8   left, middle, right, any;
    sint32  wheel;
    sint32  old;
    bool    touch, touchIsDouble;
    uint32  touchDownTimestamp;
} CursorState;

typedef struct TextInputSession
{
    utf8 * Buffer;          // UTF-8 stream
    size_t BufferSize;      // Maximum number of bytes (excluding null terminator)
    size_t Size;            // Number of bytes (excluding null terminator)
    size_t Length;          // Number of codepoints
    size_t SelectionStart;  // Selection start, in bytes
    size_t SelectionSize;   // Selection length in bytes
} TextInputSession;

struct Resolution
{
    sint32 Width;
    sint32 Height;
};

enum
{
    CURSOR_UP = 0,
    CURSOR_DOWN = 1,
    CURSOR_CHANGED = 2,
    CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
    CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
};

#ifdef __cplusplus

namespace OpenRCT2
{
    namespace Audio
    {
        interface IAudioContext;
    }

    namespace Ui
    {
        interface IUiContext;
    }

    /**
     * Represents an instance of OpenRCT2 and can be used to get various services.
     */
    interface IContext
    {
        virtual ~IContext() = default;

        virtual Audio::IAudioContext *  GetAudioContext() abstract;
        virtual Ui::IUiContext *        GetUiContext() abstract;

        virtual sint32 RunOpenRCT2(int argc, char * * argv) abstract;
        virtual void Finish() abstract;
    };

    IContext * CreateContext();
    IContext * CreateContext(Audio::IAudioContext * audioContext, Ui::IUiContext * uiContext);
    IContext * GetContext();
}

#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif
    void context_setcurrentcursor(sint32 cursor);
    void context_hide_cursor();
    void context_show_cursor();
    void context_get_cursor_position(sint32 * x, sint32 * y);
    void context_get_cursor_position_scaled(sint32 * x, sint32 * y);
    void context_set_cursor_position(sint32 x, sint32 y);
    const CursorState * context_get_cursor_state();
    const uint8 * context_get_keys_state();
    const uint8 * context_get_keys_pressed();
    TextInputSession * context_start_text_input(utf8 * buffer, size_t maxLength);
    void context_stop_text_input();
    bool context_is_input_active();
    void context_trigger_resize();
    void context_set_fullscreen_mode(sint32 mode);
    sint32 context_get_resolutions(struct Resolution * * outResolutions);
    sint32 context_get_width();
    sint32 context_get_height();

#ifdef __cplusplus
}
#endif
