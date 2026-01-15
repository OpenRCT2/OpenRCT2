/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LoadOrQuitAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"
#include "../ui/WindowManager.h"

namespace OpenRCT2::GameActions
{
    LoadOrQuitAction::LoadOrQuitAction(LoadOrQuitModes mode, PromptMode savePromptMode)
        : _mode(mode)
        , _savePromptMode(savePromptMode)
    {
    }

    void LoadOrQuitAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("mode", _mode);
        visitor.Visit("savePromptMode", _savePromptMode);
    }

    uint16_t LoadOrQuitAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::ClientOnly | Flags::AllowWhilePaused;
    }

    void LoadOrQuitAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_mode) << DS_TAG(_savePromptMode);
    }

    Result LoadOrQuitAction::Query(GameState_t& gameState) const
    {
        return Result();
    }

    Result LoadOrQuitAction::Execute(GameState_t& gameState) const
    {
        auto mode = static_cast<LoadOrQuitModes>(_mode);
        switch (mode)
        {
            case LoadOrQuitModes::OpenSavePrompt:
                gSavePromptMode = _savePromptMode;
                ContextOpenWindow(WindowClass::savePrompt);
                break;
            case LoadOrQuitModes::CloseSavePrompt:
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseByClass(WindowClass::savePrompt);
                break;
            }
            default:
                GameLoadOrQuitNoSavePrompt();
                break;
        }
        return Result();
    }
} // namespace OpenRCT2::GameActions
