/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../config/ConfigTypes.h"
#include "../interface/Cursors.h"

#include <memory>
#include <string>
#include <vector>

struct ScreenCoordsXY;
struct DrawPixelInfo;
struct ITitleSequencePlayer;

namespace OpenRCT2
{
    namespace Drawing
    {
        struct IDrawingEngineFactory;
        struct IWeatherDrawer;
        using DrawWeatherFunc = void (*)(
            DrawPixelInfo& dpi, OpenRCT2::Drawing::IWeatherDrawer* weatherDrawer, int32_t left, int32_t top, int32_t width,
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

            virtual void InitialiseScriptExtensions() = 0;
            virtual void Tick() = 0;
            virtual void Draw(DrawPixelInfo& dpi) = 0;

            // Window
            virtual void CreateWindow() = 0;
            virtual void CloseWindow() = 0;
            virtual void RecreateWindow() = 0;
            virtual void* GetWindow() = 0;
            virtual int32_t GetWidth() = 0;
            virtual int32_t GetHeight() = 0;
            virtual ScaleQuality GetScaleQuality() = 0;
            virtual void SetFullscreenMode(FULLSCREEN_MODE mode) = 0;
            virtual const std::vector<Resolution>& GetFullscreenResolutions() = 0;
            virtual bool HasFocus() = 0;
            virtual bool IsMinimised() = 0;
            virtual bool IsSteamOverlayActive() = 0;
            virtual void ProcessMessages() = 0;
            virtual void TriggerResize() = 0;

            virtual void ShowMessageBox(const std::string& message) = 0;
            virtual int32_t ShowMessageBox(
                const std::string& title, const std::string& message, const std::vector<std::string>& options)
                = 0;

            virtual bool HasMenuSupport() = 0;
            // Creates a menu with a series of options, returns the index of the selected option
            virtual int32_t ShowMenuDialog(
                const std::vector<std::string>& options, const std::string& title, const std::string& text)
                = 0;
            virtual void OpenFolder(const std::string& path) = 0;
            virtual void OpenURL(const std::string& url) = 0;
            virtual std::string ShowFileDialog(const FileDialogDesc& desc) = 0;
            virtual std::string ShowDirectoryDialog(const std::string& title) = 0;
            virtual bool HasFilePicker() const = 0;

            // Input
            virtual const CursorState* GetCursorState() = 0;
            virtual CursorID GetCursor() = 0;
            virtual void SetCursor(CursorID cursor) = 0;
            virtual void SetCursorScale(uint8_t scale) = 0;
            virtual void SetCursorVisible(bool value) = 0;
            virtual ScreenCoordsXY GetCursorPosition() = 0;
            virtual void SetCursorPosition(const ScreenCoordsXY& cursorPosition) = 0;
            virtual void SetCursorTrap(bool value) = 0;
            virtual const uint8_t* GetKeysState() = 0;
            virtual const uint8_t* GetKeysPressed() = 0;
            virtual void SetKeysPressed(uint32_t keysym, uint8_t scancode) = 0;

            // Drawing
            [[nodiscard]] virtual std::shared_ptr<Drawing::IDrawingEngineFactory> GetDrawingEngineFactory() = 0;
            virtual void DrawWeatherAnimation(
                OpenRCT2::Drawing::IWeatherDrawer* weatherDrawer, DrawPixelInfo& dpi,
                OpenRCT2::Drawing::DrawWeatherFunc drawFunc)
                = 0;

            // Text input
            virtual bool IsTextInputActive() = 0;
            virtual TextInputSession* StartTextInput(u8string& buffer, size_t maxLength) = 0;
            virtual void StopTextInput() = 0;

            // In-game UI
            virtual IWindowManager* GetWindowManager() = 0;

            // Clipboard
            virtual bool SetClipboardText(const utf8* target) = 0;

            // HACK Until all title logic is moved to libopenrct2ui, we will need to provide some services
            virtual ITitleSequencePlayer* GetTitleSequencePlayer() = 0;
        };

        [[nodiscard]] std::shared_ptr<IUiContext> CreateDummyUiContext();
    } // namespace Ui
} // namespace OpenRCT2
