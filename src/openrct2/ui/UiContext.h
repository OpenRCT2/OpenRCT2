/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../common.h"
#include "../config/Config.h"
#include "../interface/Cursors.h"

#include <memory>
#include <string>
#include <vector>

struct ScreenCoordsXY;
struct rct_drawpixelinfo;
struct ITitleSequencePlayer;

namespace OpenRCT2
{
    namespace Drawing
    {
        struct IDrawingEngineFactory;
        struct IWeatherDrawer;
        using DrawWeatherFunc = void (*)(
            rct_drawpixelinfo* dpi, OpenRCT2::Drawing::IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width,
            int32_t height);
    } // namespace Drawing

    namespace Ui
    {
        struct IWindowManager;

        enum class FULLSCREEN_MODE
        {
            WINDOWED,
            FULLSCREEN,
            FULLSCREEN_DESKTOP,
        };

        inline bool operator<(const Resolution& lhs, const Resolution& rhs)
        {
            int32_t areaA = lhs.Width * lhs.Height;
            int32_t areaB = rhs.Width * rhs.Height;
            if (areaA == areaB)
            {
                return lhs.Width < rhs.Width;
            }
            return areaA < areaB;
        }

        inline bool operator==(const Resolution& lhs, const Resolution& rhs)
        {
            return lhs.Width == rhs.Width && lhs.Height == rhs.Height;
        }

        inline bool operator!=(const Resolution& lhs, const Resolution& rhs)
        {
            return !(lhs == rhs);
        }

        enum class FileDialogType : uint8_t
        {
            Open,
            Save
        };

        struct FileDialogDesc
        {
            struct Filter
            {
                u8string Name;    // E.g. "Image Files"
                u8string Pattern; // E.g. "*.png;*.jpg;*.gif"

                Filter(u8string_view name, u8string_view pattern)
                    : Name(name)
                    , Pattern(pattern)
                {
                }
            };

            FileDialogType Type = FileDialogType::Open;
            u8string Title;
            u8string InitialDirectory;
            u8string DefaultFilename;
            std::vector<Filter> Filters;
        };

        /**
         * Represents the window or screen that OpenRCT2 is presented on.
         */
        struct IUiContext
        {
            virtual ~IUiContext() = default;

            virtual void Initialise() abstract;
            virtual void Tick() abstract;
            virtual void Draw(rct_drawpixelinfo* dpi) abstract;

            // Window
            virtual void CreateWindow() abstract;
            virtual void CloseWindow() abstract;
            virtual void RecreateWindow() abstract;
            virtual void* GetWindow() abstract;
            virtual int32_t GetWidth() abstract;
            virtual int32_t GetHeight() abstract;
            virtual ScaleQuality GetScaleQuality() abstract;
            virtual void SetFullscreenMode(FULLSCREEN_MODE mode) abstract;
            virtual const std::vector<Resolution>& GetFullscreenResolutions() abstract;
            virtual bool HasFocus() abstract;
            virtual bool IsMinimised() abstract;
            virtual bool IsSteamOverlayActive() abstract;
            virtual void ProcessMessages() abstract;
            virtual void TriggerResize() abstract;

            virtual void ShowMessageBox(const std::string& message) abstract;
            virtual int32_t ShowMessageBox(
                const std::string& title, const std::string& message, const std::vector<std::string>& options) abstract;

            virtual bool HasMenuSupport() abstract;
            // Creates a menu with a series of options, returns the index of the selected option
            virtual int32_t ShowMenuDialog(
                const std::vector<std::string>& options, const std::string& title, const std::string& text) abstract;
            virtual void OpenFolder(const std::string& path) abstract;
            virtual void OpenURL(const std::string& url) abstract;
            virtual std::string ShowFileDialog(const FileDialogDesc& desc) abstract;
            virtual std::string ShowDirectoryDialog(const std::string& title) abstract;
            virtual bool HasFilePicker() const abstract;

            // Input
            virtual const CursorState* GetCursorState() abstract;
            virtual CursorID GetCursor() abstract;
            virtual void SetCursor(CursorID cursor) abstract;
            virtual void SetCursorScale(uint8_t scale) abstract;
            virtual void SetCursorVisible(bool value) abstract;
            virtual ScreenCoordsXY GetCursorPosition() abstract;
            virtual void SetCursorPosition(const ScreenCoordsXY& cursorPosition) abstract;
            virtual void SetCursorTrap(bool value) abstract;
            virtual const uint8_t* GetKeysState() abstract;
            virtual const uint8_t* GetKeysPressed() abstract;
            virtual void SetKeysPressed(uint32_t keysym, uint8_t scancode) abstract;

            // Drawing
            [[nodiscard]] virtual std::shared_ptr<Drawing::IDrawingEngineFactory> GetDrawingEngineFactory() abstract;
            virtual void DrawWeatherAnimation(
                OpenRCT2::Drawing::IWeatherDrawer* weatherDrawer, rct_drawpixelinfo* dpi,
                OpenRCT2::Drawing::DrawWeatherFunc drawFunc) abstract;

            // Text input
            virtual bool IsTextInputActive() abstract;
            virtual TextInputSession* StartTextInput(utf8* buffer, size_t bufferSize) abstract;
            virtual void StopTextInput() abstract;

            // In-game UI
            virtual IWindowManager* GetWindowManager() abstract;

            // Clipboard
            virtual bool SetClipboardText(const utf8* target) abstract;

            // HACK Until all title logic is moved to libopenrct2ui, we will need to provide some services
            virtual ITitleSequencePlayer* GetTitleSequencePlayer() abstract;
        };

        [[nodiscard]] std::shared_ptr<IUiContext> CreateDummyUiContext();
    } // namespace Ui
} // namespace OpenRCT2
