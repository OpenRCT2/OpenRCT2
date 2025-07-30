/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScEditor.h"

    #include "../../../Context.h"
    #include "../../../OpenRCT2.h"

namespace OpenRCT2::Scripting
{

    bool ScEditor::EditorIsActive()
    {
        return gLegacyScene == LegacyScene::scenarioEditor;
    }

    void ScEditor::ThrowIfEditorIsNotActive()
    {
        if (!EditorIsActive())
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            duk_error(ctx, DUK_ERR_ERROR, "Scenario editor not active.");
        }
    }

    std::shared_ptr<ScLandscape> ScEditor::getLandscape()
    {
        ThrowIfEditorIsNotActive();
        return std::make_shared<ScLandscape>();
    }

    void ScEditor::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScEditor::getLandscape, nullptr, "landscape");
    }

} // namespace OpenRCT2::Scripting
#endif
