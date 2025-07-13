/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
ScTool OpenRCT2::Scripting::gScTool;
ScUi OpenRCT2::Scripting::gScUi;

ScWidget OpenRCT2::Scripting::gScWidget;

ScWindow OpenRCT2::Scripting::gScWindow;

static void InitialiseContext(JSContext* ctx)
{
    JSValue glb = JS_GetGlobalObject(ctx);
    // dukglue_register_global(ctx, std::make_shared<ScTitleSequenceManager>(), "titleSequenceManager");
    JS_SetPropertyStr(ctx, glb, "ui", gScUi.New(ctx, &OpenRCT2::GetContext()->GetScriptEngine()));
    JS_FreeValue(ctx, glb);
}

void UiScriptExtensions::Extend(ScriptEngine& scriptEngine)
{
    JSContext* ctx = scriptEngine.GetContext();

    gScGraphicsContext.Register(ctx);
    gScImageManager.Register(ctx);
    // ScTileSelection::Register(ctx);
    gScTool.Register(ctx);
    gScUi.Register(ctx);
    // ScViewport::Register(ctx);

    gScWidget.Register(ctx);
    // ScButtonWidget::Register(ctx);
    // ScColourPickerWidget::Register(ctx);
    // ScCheckBoxWidget::Register(ctx);
    // ScDropdownWidget::Register(ctx);
    // ScGroupBoxWidget::Register(ctx);
    // ScLabelWidget::Register(ctx);
    // ScListViewWidget::Register(ctx);
    // ScSpinnerWidget::Register(ctx);
    // ScTextBoxWidget::Register(ctx);
    // ScViewportWidget::Register(ctx);

    // ScTitleSequence::Register(ctx);
    // ScTitleSequenceManager::Register(ctx);
    // ScTitleSequencePark::Register(ctx);
    gScWindow.Register(ctx);

    InitialiseCustomImages(scriptEngine);
    InitialiseCustomMenuItems(scriptEngine);
    scriptEngine.SubscribeToPluginStoppedEvent(
        [](std::shared_ptr<Plugin> plugin) -> void { Ui::Windows::CloseWindowsOwnedByPlugin(plugin); });

    scriptEngine.RegisterExtension(InitialiseContext);
}

#endif
