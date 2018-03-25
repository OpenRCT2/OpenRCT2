#include <openrct2/scripting/ScriptEngine.h>
#include "UiExtensions.h"
#include "ScUi.hpp"
#include "ScWindow.hpp"

using namespace OpenRCT2::Scripting;

void UiScriptExtensions::Extend(ScriptEngine& scriptEngine)
{
    auto ctx = scriptEngine.GetContext();

    dukglue_register_global(ctx, std::make_shared<ScUi>(scriptEngine), "ui");

    ScUi::Register(ctx);
    ScWindow::Register(ctx);
}
