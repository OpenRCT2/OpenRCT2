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

#include "../common.h"

namespace OpenRCT2
{
    namespace Drawing
    {
        enum class  DRAWING_ENGINE_TYPE;
        interface   IDrawingEngine;
    }

    namespace Ui
    {
        enum class FULLSCREEN_MODE
        {
            WINDOWED,
            FULLSCREEN,
            FULLSCREEN_DESKTOP,
        };

        struct TextInputSession
        {
            const utf8 * Buffer;    // UTF-8 stream
            size_t BufferSize;      // Maximum number of bytes (excluding null terminator)
            size_t Size;            // Number of bytes (excluding null terminator)
            uint32 Length;          // Number of codepoints
            size_t SelectionStart;  // Selection start, in bytes
            size_t SelectionSize;   // Selection length in bytes
        };

        /**
         * Represents the window or screen that OpenRCT2 is presented on.
         */
        interface IUiContext
        {
            virtual ~IUiContext() = default;

            // Window
            virtual void *  GetWindow() abstract;
            virtual sint32  GetWidth() abstract;
            virtual sint32  GetHeight() abstract;
            virtual void    SetFullscreenMode(FULLSCREEN_MODE mode) abstract;

            // Drawing
            virtual Drawing::IDrawingEngine *   CreateDrawingEngine(Drawing::DRAWING_ENGINE_TYPE type) abstract;

            // Text input
            virtual bool                        IsTextInputActive() abstract;
            virtual const TextInputSession *    StartTextInput(utf8 * buffer, sint32 bufferSize) abstract;
            virtual void                        StopTextInput() abstract;
        };
    }
}
