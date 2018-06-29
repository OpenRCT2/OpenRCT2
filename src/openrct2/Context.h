/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

#include <memory>
#include <string>

interface IObjectManager;
interface IObjectRepository;
interface IScenarioRepository;
interface IStream;
interface ITrackDesignRepository;

class Intent;
struct rct_window;
using rct_windowclass = uint8_t;

struct CursorState
{
    int32_t  x, y;
    uint8_t   left, middle, right, any;
    int32_t  wheel;
    int32_t  old;
    bool    touch, touchIsDouble;
    uint32_t  touchDownTimestamp;
};

struct TextInputSession
{
    utf8 * Buffer;          // UTF-8 stream
    size_t BufferSize;      // Maximum number of bytes (excluding null terminator)
    size_t Size;            // Number of bytes (excluding null terminator)
    size_t Length;          // Number of codepoints
    size_t SelectionStart;  // Selection start, in bytes
    size_t SelectionSize;   // Selection length in bytes

    const utf8 * ImeBuffer; // IME UTF-8 stream
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

namespace OpenRCT2
{
    class GameState;
    interface IPlatformEnvironment;

    namespace Audio
    {
        interface IAudioContext;
    }

    namespace Drawing
    {
        interface IDrawingEngine;
    }

    namespace Localisation
    {
        class LocalisationService;
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

        virtual std::shared_ptr<Audio::IAudioContext> GetAudioContext() abstract;
        virtual std::shared_ptr<Ui::IUiContext> GetUiContext() abstract;
        virtual GameState * GetGameState() abstract;
        virtual std::shared_ptr<IPlatformEnvironment> GetPlatformEnvironment() abstract;
        virtual Localisation::LocalisationService& GetLocalisationService() abstract;
        virtual std::shared_ptr<IObjectManager> GetObjectManager() abstract;
        virtual std::shared_ptr<IObjectRepository> GetObjectRepository() abstract;
        virtual ITrackDesignRepository * GetTrackDesignRepository() abstract;
        virtual IScenarioRepository *    GetScenarioRepository() abstract;
        virtual int32_t GetDrawingEngineType() abstract;
        virtual Drawing::IDrawingEngine * GetDrawingEngine() abstract;

        virtual int32_t RunOpenRCT2(int argc, const char * * argv) abstract;

        virtual bool Initialise() abstract;
        virtual void InitialiseDrawingEngine() abstract;
        virtual void DisposeDrawingEngine() abstract;
        virtual bool LoadParkFromFile(const std::string &path, bool loadTitleScreenOnFail = false) abstract;
        virtual bool LoadParkFromStream(IStream * stream, const std::string &path, bool loadTitleScreenFirstOnFail = false) abstract;
        virtual void WriteLine(const std::string &s) abstract;
        virtual void Finish() abstract;
        virtual void Quit() abstract;

        /**
         * This is deprecated, use IPlatformEnvironment.
         */
        virtual std::string GetPathLegacy(int32_t pathId) abstract;
    };

    std::unique_ptr<IContext> CreateContext();
    std::unique_ptr<IContext> CreateContext(
        const std::shared_ptr<IPlatformEnvironment>& env,
        const std::shared_ptr<Audio::IAudioContext>& audioContext,
        const std::shared_ptr<Ui::IUiContext>& uiContext);
    IContext * GetContext();
} // namespace OpenRCT2

enum
{
    // The game update interval in milliseconds, (1000 / 40fps) = 25ms
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

void context_init();
void context_setcurrentcursor(int32_t cursor);
void context_update_cursor_scale();
void context_hide_cursor();
void context_show_cursor();
void context_get_cursor_position(int32_t * x, int32_t * y);
void context_get_cursor_position_scaled(int32_t * x, int32_t * y);
void context_set_cursor_position(int32_t x, int32_t y);
const CursorState * context_get_cursor_state();
const uint8_t * context_get_keys_state();
const uint8_t * context_get_keys_pressed();
TextInputSession * context_start_text_input(utf8 * buffer, size_t maxLength);
void context_stop_text_input();
bool context_is_input_active();
void context_trigger_resize();
void context_set_fullscreen_mode(int32_t mode);
void context_recreate_window();
int32_t context_get_resolutions(struct Resolution * * outResolutions);
int32_t context_get_width();
int32_t context_get_height();
bool context_has_focus();
void context_set_cursor_trap(bool value);
rct_window * context_open_window(rct_windowclass wc);
rct_window * context_open_detail_window(uint8_t type, int32_t id);
rct_window * context_open_window_view(uint8_t view);
rct_window * context_show_error(rct_string_id title, rct_string_id message);
rct_window * context_open_intent(Intent * intent);
void context_broadcast_intent(Intent * intent);
void context_force_close_window_by_class(rct_windowclass wc);
void context_update_map_tooltip();
void context_handle_input();
void context_input_handle_keyboard(bool isTitle);
bool context_read_bmp(void * * outPixels, uint32_t * outWidth, uint32_t * outHeight, const utf8 * path);
void context_quit();
const utf8 * context_get_path_legacy(int32_t pathId);
bool context_load_park_from_file(const utf8 * path);
bool context_load_park_from_stream(void * stream);
