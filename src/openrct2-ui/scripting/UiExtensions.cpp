/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "UiExtensions.h"

#    include "CustomMenu.h"
#    include "ScGraphicsContext.hpp"
#    include "ScTileSelection.hpp"
#    include "ScTitleSequence.hpp"
#    include "ScUi.hpp"
#    include "ScWidget.hpp"
#    include "ScWindow.hpp"

#    include <openrct2/scripting/ScriptEngine.h>

using namespace OpenRCT2::Scripting;

void UiScriptExtensions::Extend(ScriptEngine& scriptEngine)
{
    auto ctx = scriptEngine.GetContext();

    dukglue_register_global(ctx, std::make_shared<ScTitleSequenceManager>(), "titleSequenceManager");
    dukglue_register_global(ctx, std::make_shared<ScUi>(scriptEngine), "ui");

    ScGraphicsContext::Register(ctx);
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

    InitialiseCustomMenuItems(scriptEngine);
}

std::shared_ptr<ScWindow> ScWidget::WindowGet() const
{
    return std::make_shared<ScWindow>(_class, _number);
}

void ScWidget::Register(duk_context* ctx)
{
    dukglue_register_property(ctx, &ScWidget::WindowGet, nullptr, "window");
    dukglue_register_property(ctx, &ScWidget::NameGet, &ScWidget::NameSet, "name");
    dukglue_register_property(ctx, &ScWidget::TypeGet, nullptr, "type");
    dukglue_register_property(ctx, &ScWidget::XGet, &ScWidget::XSet, "x");
    dukglue_register_property(ctx, &ScWidget::YGet, &ScWidget::YSet, "y");
    dukglue_register_property(ctx, &ScWidget::WidthGet, &ScWidget::WidthSet, "width");
    dukglue_register_property(ctx, &ScWidget::HeightGet, &ScWidget::HeightSet, "height");
    dukglue_register_property(ctx, &ScWidget::IsDisabledGet, &ScWidget::IsDisabledSet, "isDisabled");
    dukglue_register_property(ctx, &ScWidget::IsVisibleGet, &ScWidget::IsVisibleSet, "isVisible");
}

#endif
