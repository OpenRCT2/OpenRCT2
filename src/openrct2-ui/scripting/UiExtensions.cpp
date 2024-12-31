/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "UiExtensions.h"

    #include "CustomImages.h"
    #include "CustomMenu.h"
    #include "ScGraphicsContext.hpp"
    #include "ScImageManager.hpp"
    #include "ScTileSelection.hpp"
    #include "ScTitleSequence.hpp"
    #include "ScUi.hpp"
    #include "ScWidget.hpp"
    #include "ScWindow.hpp"

    #include <openrct2/scripting/ScriptEngine.h>

using namespace OpenRCT2::Scripting;

void UiScriptExtensions::Extend(ScriptEngine& scriptEngine)
{
    auto ctx = scriptEngine.GetContext();

    dukglue_register_global(ctx, std::make_shared<ScTitleSequenceManager>(), "titleSequenceManager");
    dukglue_register_global(ctx, std::make_shared<ScUi>(scriptEngine), "ui");

    ScGraphicsContext::Register(ctx);
    ScImageManager::Register(ctx);
    ScTileSelection::Register(ctx);
    ScTool::Register(ctx);
    ScUi::Register(ctx);
    ScViewport::Register(ctx);

    ScWidget::Register(ctx);
    ScButtonWidget::Register(ctx);
    ScColourPickerWidget::Register(ctx);
    ScCheckBoxWidget::Register(ctx);
    ScDropdownWidget::Register(ctx);
    ScGroupBoxWidget::Register(ctx);
    ScLabelWidget::Register(ctx);
    ScListViewWidget::Register(ctx);
    ScSpinnerWidget::Register(ctx);
    ScTextBoxWidget::Register(ctx);
    ScViewportWidget::Register(ctx);

    ScTitleSequence::Register(ctx);
    ScTitleSequenceManager::Register(ctx);
    ScTitleSequencePark::Register(ctx);
    ScWindow::Register(ctx);

    InitialiseCustomImages(scriptEngine);
    InitialiseCustomMenuItems(scriptEngine);
    scriptEngine.SubscribeToPluginStoppedEvent(
        [](std::shared_ptr<Plugin> plugin) -> void { CloseWindowsOwnedByPlugin(plugin); });
}

std::shared_ptr<ScWindow> ScWidget::window_get() const
{
    return std::make_shared<ScWindow>(_class, _number);
}

void ScWidget::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScWidget::window_get, nullptr, "window");
    dukglue_register_property(ctx, &ScWidget::name_get, &ScWidget::name_set, "name");
    dukglue_register_property(ctx, &ScWidget::type_get, nullptr, "type");
    dukglue_register_property(ctx, &ScWidget::x_get, &ScWidget::x_set, "x");
    dukglue_register_property(ctx, &ScWidget::y_get, &ScWidget::y_set, "y");
    dukglue_register_property(ctx, &ScWidget::width_get, &ScWidget::width_set, "width");
    dukglue_register_property(ctx, &ScWidget::height_get, &ScWidget::height_set, "height");
    dukglue_register_property(ctx, &ScWidget::tooltip_get, &ScWidget::tooltip_set, "tooltip");
    dukglue_register_property(ctx, &ScWidget::isDisabled_get, &ScWidget::isDisabled_set, "isDisabled");
    dukglue_register_property(ctx, &ScWidget::isVisible_get, &ScWidget::isVisible_set, "isVisible");
}

#endif
