/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "UiExtensions.h"

    #include "CustomImages.h"
    #include "CustomMenu.h"
    #include "ScGraphicsContext.hpp"
    #include "ScImageManager.hpp"
    #include "ScTileSelection.hpp"
    #include "ScTitleSequence.hpp"
    #include "ScUi.hpp"
    #include "ScWidget.hpp"
    #include "ScWindow.h"

    #include <openrct2/scripting/ScriptEngine.h>

using namespace OpenRCT2::Scripting;

ScGraphicsContext OpenRCT2::Scripting::gScGraphicsContext;
ScImageManager OpenRCT2::Scripting::gScImageManager;
ScTileSelection OpenRCT2::Scripting::gScTileSelection;
ScTool OpenRCT2::Scripting::gScTool;
ScUi OpenRCT2::Scripting::gScUi;
ScViewport OpenRCT2::Scripting::gScViewport;

ScWidget OpenRCT2::Scripting::gScWidget;

ScTitleSequence OpenRCT2::Scripting::gScTitleSequence;
ScTitleSequenceManager OpenRCT2::Scripting::gScTitleSequenceManager;
ScTitleSequencePark OpenRCT2::Scripting::gScTitleSequencePark;
ScWindow OpenRCT2::Scripting::gScWindow;

static void InitialiseContext(JSContext* ctx)
{
    JSValue glb = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, glb, "titleSequenceManager", gScTitleSequenceManager.New(ctx));
    JS_SetPropertyStr(ctx, glb, "ui", gScUi.New(ctx, &OpenRCT2::GetContext()->GetScriptEngine()));
    JS_FreeValue(ctx, glb);
}

void UiScriptExtensions::Extend(ScriptEngine& scriptEngine)
{
    JSContext* ctx = scriptEngine.GetContext();

    gScGraphicsContext.Register(ctx);
    gScImageManager.Register(ctx);
    gScTileSelection.Register(ctx);
    gScTool.Register(ctx);
    gScUi.Register(ctx);
    gScViewport.Register(ctx);

    gScWidget.Register(ctx);

    gScTitleSequence.Register(ctx);
    gScTitleSequenceManager.Register(ctx);
    gScTitleSequencePark.Register(ctx);
    gScWindow.Register(ctx);

    InitialiseCustomImages(scriptEngine);
    InitialiseCustomMenuItems(scriptEngine);
    scriptEngine.SubscribeToPluginStoppedEvent(
        [](std::shared_ptr<Plugin> plugin) -> void { Ui::Windows::CloseWindowsOwnedByPlugin(plugin); });

    scriptEngine.RegisterExtension(InitialiseContext);
}

#endif
