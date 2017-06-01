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

#include <string>
#include <vector>
#include "../common.h"
#include "../Context.h"
#include "../interface/Cursors.h"

struct SDL_Window;

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

        inline bool operator <(const Resolution& lhs, const Resolution& rhs)
        {
            sint32 areaA = lhs.Width * lhs.Height;
            sint32 areaB = rhs.Width * rhs.Height;
            if (areaA == areaB)
            {
                return lhs.Width < rhs.Width;
            }
            return areaA < areaB;
        }

        inline bool operator ==(const Resolution& lhs, const Resolution& rhs)
        {
            return lhs.Width == rhs.Width &&
                   lhs.Height == rhs.Height;
        }

        inline bool operator !=(const Resolution& lhs, const Resolution& rhs)
        {
            return !(lhs == rhs);
        }

        enum class FILE_DIALOG_TYPE
        {
            OPEN,
            SAVE,
        };

        struct FileDialogDesc
        {
            struct Filter
            {
                std::string Name;       // E.g. "Image Files"
                std::string Pattern;    // E.g. "*.png;*.jpg;*.gif"
            };

            FILE_DIALOG_TYPE    Type = FILE_DIALOG_TYPE::OPEN;
            std::string         Title;
            std::string         InitialDirectory;
            std::string         DefaultFilename;
            std::vector<Filter> Filters;
        };

        /**
         * Represents the window or screen that OpenRCT2 is presented on.
         */
        interface IUiContext
        {
            virtual ~IUiContext() = default;

            // Window
            virtual void            CreateWindow() abstract;
            virtual void            CloseWindow() abstract;
            virtual SDL_Window *    GetWindow() abstract;
            virtual sint32  GetWidth() abstract;
            virtual sint32  GetHeight() abstract;
            virtual void    SetFullscreenMode(FULLSCREEN_MODE mode) abstract;
            virtual std::vector<Resolution> GetFullscreenResolutions() abstract;
            virtual bool HasFocus() abstract;
            virtual bool IsMinimised() abstract;
            virtual bool IsSteamOverlayActive() abstract;
            virtual void ProcessMessages() abstract;
            virtual void TriggerResize() abstract;

            virtual void        ShowMessageBox(const std::string &message) abstract;
            virtual std::string ShowFileDialog(const FileDialogDesc &desc) abstract;
            virtual std::string ShowDirectoryDialog(const std::string &title) abstract;

            // Input
            virtual const CursorState * GetCursorState() abstract;
            virtual CURSOR_ID           GetCursor() abstract;
            virtual void                SetCursor(CURSOR_ID cursor) abstract;
            virtual void                SetCursorVisible(bool value) abstract;
            virtual void                GetCursorPosition(sint32 * x, sint32 * y) abstract;
            virtual void                SetCursorPosition(sint32 x, sint32 y) abstract;
            virtual void                SetCursorTrap(bool value) abstract;
            virtual const uint8 *       GetKeysState() abstract;
            virtual const uint8 *       GetKeysPressed() abstract;
            virtual void                SetKeysPressed(uint32 keysym, uint8 scancode) abstract;

            // Drawing
            virtual Drawing::IDrawingEngine *   CreateDrawingEngine(Drawing::DRAWING_ENGINE_TYPE type) abstract;

            // Text input
            virtual bool                IsTextInputActive() abstract;
            virtual TextInputSession *  StartTextInput(utf8 * buffer, size_t bufferSize) abstract;
            virtual void                StopTextInput() abstract;
        };
    }
}
