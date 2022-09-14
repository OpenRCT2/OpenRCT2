/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/String.hpp"
#include "interface/WindowClasses.h"
#include "world/Location.hpp"

#include <memory>
#include <string>

struct IObjectManager;
struct IObjectRepository;
struct IScenarioRepository;
enum class DrawingEngine : int32_t;
enum class CursorID : uint8_t;

namespace OpenRCT2
{
    struct IStream;
}
struct ITrackDesignRepository;
struct IGameStateSnapshots;

class Intent;
struct rct_window;
struct NewVersionInfo;

struct TTFFontDescriptor;
namespace OpenRCT2::Ui
{
    struct FileDialogDesc;
}

struct CursorState
{
    ScreenCoordsXY position;
    uint8_t left, middle, right, any;
    int32_t wheel;
    int32_t old;
    bool touch, touchIsDouble;
    uint32_t touchDownTimestamp;
};

struct TextInputSession
{
    utf8* Buffer;          // UTF-8 stream
    size_t BufferSize;     // Maximum number of bytes (excluding null terminator)
    size_t Size;           // Number of bytes (excluding null terminator)
    size_t Length;         // Number of codepoints
    size_t SelectionStart; // Selection start, in bytes
    size_t SelectionSize;  // Selection length in bytes

    const utf8* ImeBuffer; // IME UTF-8 stream
};

struct Resolution
{
    int32_t Width;
    int32_t Height;
};

enum
{
    CURSOR_UP = 0,
    CURSOR_DOWN = 1,
    CURSOR_CHANGED = 2,
    CURSOR_RELEASED = CURSOR_UP | CURSOR_CHANGED,
    CURSOR_PRESSED = CURSOR_DOWN | CURSOR_CHANGED,
};

class NetworkBase;

namespace OpenRCT2
{
    class GameState;

    struct IPlatformEnvironment;
    struct IReplayManager;

    namespace Audio
    {
        struct IAudioContext;
    }

    namespace Drawing
    {
        struct IDrawingEngine;
    }

    namespace Localisation
    {
        class LocalisationService;
    }

    namespace Scripting
    {
        class ScriptEngine;
    }

    namespace Ui
    {
        struct IUiContext;
    }

    namespace Paint
    {
        struct Painter;
    }

    /**
     * Represents an instance of OpenRCT2 and can be used to get various services.
     */
    struct IContext
    {
        virtual ~IContext() = default;

        [[nodiscard]] virtual std::shared_ptr<Audio::IAudioContext> GetAudioContext() abstract;
        [[nodiscard]] virtual std::shared_ptr<Ui::IUiContext> GetUiContext() abstract;
        virtual GameState* GetGameState() abstract;
        [[nodiscard]] virtual std::shared_ptr<IPlatformEnvironment> GetPlatformEnvironment() abstract;
        virtual Localisation::LocalisationService& GetLocalisationService() abstract;
        virtual IObjectManager& GetObjectManager() abstract;
        virtual IObjectRepository& GetObjectRepository() abstract;
#ifdef ENABLE_SCRIPTING
        virtual Scripting::ScriptEngine& GetScriptEngine() abstract;
#endif
        virtual ITrackDesignRepository* GetTrackDesignRepository() abstract;
        virtual IScenarioRepository* GetScenarioRepository() abstract;
        virtual IReplayManager* GetReplayManager() abstract;
        virtual IGameStateSnapshots* GetGameStateSnapshots() abstract;
        virtual DrawingEngine GetDrawingEngineType() abstract;
        virtual Drawing::IDrawingEngine* GetDrawingEngine() abstract;
        virtual Paint::Painter* GetPainter() abstract;
#ifndef DISABLE_NETWORK
        virtual NetworkBase& GetNetwork() abstract;
#endif
        virtual int32_t RunOpenRCT2(int argc, const char** argv) abstract;

        virtual bool Initialise() abstract;
        virtual void InitialiseDrawingEngine() abstract;
        virtual void DisposeDrawingEngine() abstract;
        virtual bool LoadParkFromFile(u8string_view path, bool loadTitleScreenOnFail = false, bool asScenario = false) abstract;
        virtual bool LoadParkFromStream(
            IStream* stream, u8string_view path, bool loadTitleScreenFirstOnFail = false, bool asScenario = false) abstract;
        virtual void WriteLine(std::string_view s) abstract;
        virtual void WriteErrorLine(std::string_view s) abstract;
        virtual void Finish() abstract;
        virtual void Quit() abstract;

        virtual bool HasNewVersionInfo() const abstract;
        virtual const NewVersionInfo* GetNewVersionInfo() const abstract;

        virtual void SetTimeScale(float newScale) abstract;
        virtual float GetTimeScale() const abstract;
    };

    [[nodiscard]] std::unique_ptr<IContext> CreateContext();
    [[nodiscard]] std::unique_ptr<IContext> CreateContext(
        const std::shared_ptr<IPlatformEnvironment>& env, const std::shared_ptr<Audio::IAudioContext>& audioContext,
        const std::shared_ptr<Ui::IUiContext>& uiContext);
    [[nodiscard]] IContext* GetContext();
} // namespace OpenRCT2

namespace
{
    // The number of logical update / ticks per second.
    constexpr uint32_t GAME_UPDATE_FPS = 40;
    // The maximum amount of updates in case rendering is slower
    constexpr uint32_t GAME_MAX_UPDATES = 4;
    // The game update interval in milliseconds, (1000 / 40fps) = 25ms
    constexpr float GAME_UPDATE_TIME_MS = 1.0f / GAME_UPDATE_FPS;
    // The maximum threshold to advance.
    constexpr float GAME_UPDATE_MAX_THRESHOLD = GAME_UPDATE_TIME_MS * GAME_MAX_UPDATES;
}; // namespace

constexpr float GAME_MIN_TIME_SCALE = 0.1f;
constexpr float GAME_MAX_TIME_SCALE = 5.0f;

void context_init();
void context_setcurrentcursor(CursorID cursor);
void context_update_cursor_scale();
void context_hide_cursor();
void context_show_cursor();
ScreenCoordsXY context_get_cursor_position();
ScreenCoordsXY context_get_cursor_position_scaled();
void context_set_cursor_position(const ScreenCoordsXY& cursorPosition);
const CursorState* context_get_cursor_state();
const uint8_t* context_get_keys_state();
const uint8_t* context_get_keys_pressed();
TextInputSession* context_start_text_input(utf8* buffer, size_t maxLength);
void context_stop_text_input();
bool context_is_input_active();
void context_trigger_resize();
void context_set_fullscreen_mode(int32_t mode);
void context_recreate_window();
int32_t context_get_width();
int32_t context_get_height();
bool context_has_focus();
void context_set_cursor_trap(bool value);
rct_window* context_open_window(WindowClass wc);
rct_window* context_open_detail_window(uint8_t type, int32_t id);
rct_window* context_open_window_view(uint8_t view);
rct_window* context_show_error(StringId title, StringId message, const class Formatter& args);
rct_window* context_open_intent(Intent* intent);
void context_broadcast_intent(Intent* intent);
void context_force_close_window_by_class(WindowClass wc);
void context_update_map_tooltip();
void context_handle_input();
void context_input_handle_keyboard(bool isTitle);
void context_quit();
bool context_load_park_from_file(u8string_view path);
bool context_load_park_from_stream(void* stream);
bool ContextOpenCommonFileDialog(utf8* outFilename, OpenRCT2::Ui::FileDialogDesc& desc, size_t outSize);
u8string ContextOpenCommonFileDialog(OpenRCT2::Ui::FileDialogDesc& desc);
