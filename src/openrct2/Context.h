/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"
#include "interface/WindowClasses.h"
#include "localisation/StringIdType.h"
#include "world/Location.hpp"

#include <memory>

struct IObjectManager;
struct IObjectRepository;
struct IScenarioRepository;
enum class DrawingEngine : int32_t;
enum class CursorID : uint8_t;

namespace OpenRCT2
{
    struct IStream;
    class Intent;
    struct WindowBase;
} // namespace OpenRCT2

struct ITrackDesignRepository;
struct IGameStateSnapshots;

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
    u8string* Buffer;      // UTF-8 string buffer, non-owning.
    size_t Length;         // Number of codepoints
    size_t MaxLength;      // Maximum length of text, Length can't be larger than this.
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
    class AssetPackManager;

    struct IPlatformEnvironment;
    struct IReplayManager;
    struct IScene;

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

        [[nodiscard]] virtual std::shared_ptr<Audio::IAudioContext> GetAudioContext() = 0;
        [[nodiscard]] virtual std::shared_ptr<Ui::IUiContext> GetUiContext() = 0;
        [[nodiscard]] virtual std::shared_ptr<IPlatformEnvironment> GetPlatformEnvironment() = 0;
        virtual Localisation::LocalisationService& GetLocalisationService() = 0;
        virtual IObjectManager& GetObjectManager() = 0;
        virtual IObjectRepository& GetObjectRepository() = 0;
#ifdef ENABLE_SCRIPTING
        virtual Scripting::ScriptEngine& GetScriptEngine() = 0;
#endif
        virtual ITrackDesignRepository* GetTrackDesignRepository() = 0;
        virtual IScenarioRepository* GetScenarioRepository() = 0;
        virtual IReplayManager* GetReplayManager() = 0;
        virtual AssetPackManager* GetAssetPackManager() = 0;
        virtual IGameStateSnapshots* GetGameStateSnapshots() = 0;
        virtual DrawingEngine GetDrawingEngineType() = 0;
        virtual Drawing::IDrawingEngine* GetDrawingEngine() = 0;
        virtual Paint::Painter* GetPainter() = 0;
#ifndef DISABLE_NETWORK
        virtual NetworkBase& GetNetwork() = 0;
#endif

        virtual IScene* GetPreloaderScene() = 0;
        virtual IScene* GetIntroScene() = 0;
        virtual IScene* GetTitleScene() = 0;
        virtual IScene* GetGameScene() = 0;
        virtual IScene* GetEditorScene() = 0;

        virtual IScene* GetActiveScene() = 0;
        virtual void SetActiveScene(IScene* screen) = 0;

        virtual int32_t RunOpenRCT2(int argc, const char** argv) = 0;

        virtual bool Initialise() = 0;
        virtual void InitialiseDrawingEngine() = 0;
        virtual void DisposeDrawingEngine() = 0;

        virtual void OpenProgress(StringId captionStringId) = 0;
        virtual void SetProgress(uint32_t currentProgress, uint32_t totalCount, StringId format = kStringIdNone) = 0;
        virtual void CloseProgress() = 0;

        virtual bool LoadParkFromFile(const u8string& path, bool loadTitleScreenOnFail = false, bool asScenario = false) = 0;
        virtual bool LoadParkFromStream(
            IStream* stream, const std::string& path, bool loadTitleScreenFirstOnFail = false, bool asScenario = false)
            = 0;
        virtual void WriteLine(const std::string& s) = 0;
        virtual void WriteErrorLine(const std::string& s) = 0;
        virtual void Finish() = 0;
        virtual void Quit() = 0;

        virtual bool HasNewVersionInfo() const = 0;
        virtual const NewVersionInfo* GetNewVersionInfo() const = 0;

        virtual void SetTimeScale(float newScale) = 0;
        virtual float GetTimeScale() const = 0;
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
    constexpr uint32_t kGameUpdateFPS = 40;
    // The maximum amount of updates in case rendering is slower
    constexpr uint32_t kGameMaxUpdates = 4;
    // The game update interval in milliseconds, (1000 / 40fps) = 25ms
    constexpr float kGameUpdateTimeMS = 1.0f / kGameUpdateFPS;
    // The maximum threshold to advance.
    constexpr float kGameUpdateMaxThreshold = kGameUpdateTimeMS * kGameMaxUpdates;
}; // namespace

constexpr float kGameMinTimeScale = 0.1f;
constexpr float kGameMaxTimeScale = 5.0f;

void ContextInit();
void ContextSetCurrentCursor(CursorID cursor);
void ContextUpdateCursorScale();
void ContextHideCursor();
void ContextShowCursor();
ScreenCoordsXY ContextGetCursorPosition();
ScreenCoordsXY ContextGetCursorPositionScaled();
void ContextSetCursorPosition(const ScreenCoordsXY& cursorPosition);
const CursorState* ContextGetCursorState();
const uint8_t* ContextGetKeysState();
const uint8_t* ContextGetKeysPressed();
TextInputSession* ContextStartTextInput(u8string& buffer, size_t maxLength);
void ContextStopTextInput();
bool ContextIsInputActive();
void ContextTriggerResize();
void ContextSetFullscreenMode(int32_t mode);
void ContextRecreateWindow();
int32_t ContextGetWidth();
int32_t ContextGetHeight();
bool ContextHasFocus();
void ContextSetCursorTrap(bool value);
OpenRCT2::WindowBase* ContextOpenWindow(WindowClass wc);
OpenRCT2::WindowBase* ContextOpenDetailWindow(uint8_t type, int32_t id);
OpenRCT2::WindowBase* ContextOpenWindowView(uint8_t view);
OpenRCT2::WindowBase* ContextShowError(StringId title, StringId message, const class Formatter& args, bool autoClose = false);
OpenRCT2::WindowBase* ContextOpenIntent(OpenRCT2::Intent* intent);
void ContextBroadcastIntent(OpenRCT2::Intent* intent);
void ContextForceCloseWindowByClass(WindowClass wc);
void ContextHandleInput();
void ContextInputHandleKeyboard(bool isTitle);
void ContextQuit();
bool ContextLoadParkFromStream(void* stream);
u8string ContextOpenCommonFileDialog(OpenRCT2::Ui::FileDialogDesc& desc);
