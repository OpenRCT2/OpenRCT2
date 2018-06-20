/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../common.h"
#include "../Context.h"
#include "../interface/Cursors.h"

struct rct_drawpixelinfo;
interface ITitleSequencePlayer;

namespace OpenRCT2
{
    namespace Drawing
    {
        interface IDrawingEngineFactory;
        interface IRainDrawer;
        using DrawRainFunc = void(*)(OpenRCT2::Drawing::IRainDrawer * rainDrawer, int32_t left, int32_t top, int32_t width, int32_t height);
    } // namespace Drawing

    namespace Ui
    {
        interface IWindowManager;

        enum class FULLSCREEN_MODE
        {
            WINDOWED,
            FULLSCREEN,
            FULLSCREEN_DESKTOP,
        };

        inline bool operator <(const Resolution& lhs, const Resolution& rhs)
        {
            int32_t areaA = lhs.Width * lhs.Height;
            int32_t areaB = rhs.Width * rhs.Height;
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

            virtual void    Update() abstract;
            virtual void    Draw(rct_drawpixelinfo * dpi) abstract;

            // Window
            virtual void    CreateWindow() abstract;
            virtual void    CloseWindow() abstract;
            virtual void    RecreateWindow() abstract;
            virtual void *  GetWindow() abstract;
            virtual int32_t  GetWidth() abstract;
            virtual int32_t  GetHeight() abstract;
            virtual int32_t  GetScaleQuality() abstract;
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
            virtual void                SetCursorScale(uint8_t scale) abstract;
            virtual void                SetCursorVisible(bool value) abstract;
            virtual void                GetCursorPosition(int32_t * x, int32_t * y) abstract;
            virtual void                SetCursorPosition(int32_t x, int32_t y) abstract;
            virtual void                SetCursorTrap(bool value) abstract;
            virtual const uint8_t *       GetKeysState() abstract;
            virtual const uint8_t *       GetKeysPressed() abstract;
            virtual void                SetKeysPressed(uint32_t keysym, uint8_t scancode) abstract;

            // Drawing
            virtual std::shared_ptr<Drawing::IDrawingEngineFactory> GetDrawingEngineFactory() abstract;
            virtual void DrawRainAnimation(OpenRCT2::Drawing::IRainDrawer* rainDrawer, rct_drawpixelinfo* dpi, OpenRCT2::Drawing::DrawRainFunc drawFunc) abstract;

            // Text input
            virtual bool                IsTextInputActive() abstract;
            virtual TextInputSession *  StartTextInput(utf8 * buffer, size_t bufferSize) abstract;
            virtual void                StopTextInput() abstract;

            // In-game UI
            virtual IWindowManager *    GetWindowManager() abstract;

            // Clipboard
            virtual bool              SetClipboardText(const utf8* target) abstract;

            // HACK Until all title logic is moved to libopenrct2ui, we will need to provide some services
            virtual ITitleSequencePlayer * GetTitleSequencePlayer() abstract;
        };

        std::shared_ptr<IUiContext> CreateDummyUiContext();
    } // namespace Ui
} // namespace OpenRCT2
