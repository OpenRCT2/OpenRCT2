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

#pragma once

#include "common.h"

#include "interface/window.h"


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

    const utf8 * ImeBuffer; // IME UTF-8 stream
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

#include <string>

namespace OpenRCT2
{
    interface IPlatformEnvironment;

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

        virtual bool Initialise() abstract;
        virtual bool LoadParkFromFile(const std::string &path, bool loadTitleScreenOnFail = false) abstract;
        virtual void Finish() abstract;
        virtual void Quit() abstract;

        /**
         * This is deprecated, use IPlatformEnvironment.
         */
        virtual std::string GetPathLegacy(sint32 pathId) abstract;
    };

    IContext * CreateContext();
    IContext * CreateContext(IPlatformEnvironment * env, Audio::IAudioContext * audioContext, Ui::IUiContext * uiContext);
    IContext * GetContext();
}

#endif // __cplusplus

enum
{
    // The game update inverval in milliseconds, (1000 / 40fps) = 25ms
    GAME_UPDATE_TIME_MS = 25,
    // The number of logical update / ticks per second.
    GAME_UPDATE_FPS = 40,
    // The maximum amount of updates in case rendering is slower
    GAME_MAX_UPDATES = 4,
    // The maximum threshold to advance.
    GAME_UPDATE_MAX_THRESHOLD = GAME_UPDATE_TIME_MS * GAME_MAX_UPDATES,
};

/**
 * Legacy get_file_path IDs.
 * Remove when context_get_path_legacy is removed.
 */
enum
{
    PATH_ID_G1,
    PATH_ID_PLUGIN,
    PATH_ID_CSS1,
    PATH_ID_CSS2,
    PATH_ID_CSS4,
    PATH_ID_CSS5,
    PATH_ID_CSS6,
    PATH_ID_CSS7,
    PATH_ID_CSS8,
    PATH_ID_CSS9,
    PATH_ID_CSS11,
    PATH_ID_CSS12,
    PATH_ID_CSS13,
    PATH_ID_CSS14,
    PATH_ID_CSS15,
    PATH_ID_CSS3,
    PATH_ID_CSS17,
    PATH_ID_CSS18,
    PATH_ID_CSS19,
    PATH_ID_CSS20,
    PATH_ID_CSS21,
    PATH_ID_CSS22,
    PATH_ID_SCORES,
    PATH_ID_CSS23,
    PATH_ID_CSS24,
    PATH_ID_CSS25,
    PATH_ID_CSS26,
    PATH_ID_CSS27,
    PATH_ID_CSS28,
    PATH_ID_CSS29,
    PATH_ID_CSS30,
    PATH_ID_CSS31,
    PATH_ID_CSS32,
    PATH_ID_CSS33,
    PATH_ID_CSS34,
    PATH_ID_CSS35,
    PATH_ID_CSS36,
    PATH_ID_CSS37,
    PATH_ID_CSS38,
    PATH_ID_CUSTOM1,
    PATH_ID_CUSTOM2,
    PATH_ID_CSS39,
    PATH_ID_CSS40,
    PATH_ID_CSS41,
    PATH_ID_SIXFLAGS_MAGICMOUNTAIN,
    PATH_ID_CSS42,
    PATH_ID_CSS43,
    PATH_ID_CSS44,
    PATH_ID_CSS45,
    PATH_ID_CSS46,
    PATH_ID_CSS50,
    PATH_ID_END,
};

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
    void context_recreate_window();
    sint32 context_get_resolutions(struct Resolution * * outResolutions);
    sint32 context_get_width();
    sint32 context_get_height();
    bool context_has_focus();
    void context_set_cursor_trap(bool value);
    rct_window * context_open_window(rct_windowclass wc);
    rct_window * context_open_window_view(uint8 view);
    void context_input_handle_keyboard(bool isTitle);
    bool context_read_bmp(void * * outPixels, uint32 * outWidth, uint32 * outHeight, const utf8 * path);
    void context_quit();
    const utf8 * context_get_path_legacy(sint32 pathId);
    bool context_load_park_from_file(const utf8 * path);
#ifdef __cplusplus
}
#endif
