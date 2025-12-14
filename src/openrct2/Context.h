/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/BackgroundWorker.hpp"
#include "core/StringTypes.h"
#include "interface/WindowClasses.h"
#include "localisation/StringIdType.h"
#include "world/Location.hpp"

#include <memory>

enum class CursorID : uint8_t;
enum class DrawingEngine : int32_t;
struct IGameStateSnapshots;
struct IScenarioRepository;
struct ITrackDesignRepository;
struct NewVersionInfo;
struct TTFFontDescriptor;

namespace OpenRCT2
{
    enum class WindowDetail : uint8_t;
    enum class WindowView : uint8_t;

    class AssetPackManager;
    class Formatter;
    class Intent;
    struct CursorState;
    struct IObjectManager;
    struct IObjectRepository;
    struct IPlatformEnvironment;
    struct IReplayManager;
    struct IScene;
    struct IStream;
    struct TextInputSession;
    struct WindowBase;

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

    namespace Network
    {
        class NetworkBase;
    }

    namespace Scripting
    {
        class ScriptEngine;
    }

    namespace Ui
    {
        struct FileDialogDesc;
        struct IUiContext;
    } // namespace Ui

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

        [[nodiscard]] virtual Audio::IAudioContext& GetAudioContext() = 0;
        [[nodiscard]] virtual Ui::IUiContext& GetUiContext() = 0;
        [[nodiscard]] virtual IPlatformEnvironment& GetPlatformEnvironment() = 0;
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
        virtual Network::NetworkBase& GetNetwork() = 0;
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
        virtual void ResetSubsystems() = 0;

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

        virtual BackgroundWorker& GetBackgroundWorker() = 0;
    };

    [[nodiscard]] std::unique_ptr<IContext> CreateContext();
    [[nodiscard]] std::unique_ptr<IContext> CreateContext(
        std::unique_ptr<IPlatformEnvironment>&& env, std::unique_ptr<Audio::IAudioContext>&& audioContext,
        std::unique_ptr<Ui::IUiContext>&& uiContext);
    [[nodiscard]] IContext* GetContext();

    void ContextInit();
    void ContextResetSubsystems();
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
    WindowBase* ContextOpenWindow(WindowClass wc);
    WindowBase* ContextOpenDetailWindow(WindowDetail type, int32_t id);
    WindowBase* ContextOpenWindowView(WindowView view);
    WindowBase* ContextShowError(StringId title, StringId message, const class Formatter& args, bool autoClose = false);
    WindowBase* ContextOpenIntent(Intent* intent);
    void ContextBroadcastIntent(Intent* intent);
    void ContextForceCloseWindowByClass(WindowClass wc);
    void ContextHandleInput();
    void ContextInputHandleKeyboard(bool isTitle);
    void ContextQuit();
    bool ContextLoadParkFromStream(void* stream);
    u8string ContextOpenCommonFileDialog(Ui::FileDialogDesc& desc);
} // namespace OpenRCT2
