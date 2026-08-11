/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/Console.hpp"
#include "../entity/EntityRegistry.h"
#include "../network/NetworkTypes.h"
#include "CommandLine.hpp"

#include <memory>

using namespace OpenRCT2::CommandLine;

namespace OpenRCT2
{
    // clang-format off
    static constexpr CommandLineOptionDefinition kNoOptions[]
    {
        kOptionTableEnd
    };

    static ExitCode HandleSimulate(CommandLineArgEnumerator* argEnumerator);

    const CommandLineCommand CommandLine::kSimulateCommands[]{
        // Main commands
        DefineCommand("", "<park file> <ticks>", kNoOptions, HandleSimulate),
        kCommandTableEnd
    };
    // clang-format on

    static ExitCode HandleSimulate(CommandLineArgEnumerator* argEnumerator)
    {
        const utf8* inputPath;
        if (!argEnumerator->TryPopString(&inputPath))
        {
            Console::Error::WriteLine("Expected a save file path");
            return ExitCode::fail;
        }

        int32_t ticks;
        if (!argEnumerator->TryPopInteger(&ticks))
        {
            Console::Error::WriteLine("Expected a number of ticks to simulate");
            return ExitCode::fail;
        }

        gOpenRCT2Headless = true;

#ifndef DISABLE_NETWORK
        gNetworkStart = Network::Mode::server;
#endif

        std::unique_ptr<IContext> context(CreateContext());
        if (context->Initialise())
        {
            if (!context->LoadParkFromFile(inputPath))
            {
                return ExitCode::fail;
            }

            Console::WriteLine("Running %d ticks...", ticks);
            for (int32_t i = 0; i < ticks; i++)
            {
                gameStateUpdateLogic();
            }
            Console::WriteLine("Completed: %s", getGameState().entities.GetAllEntitiesChecksum().ToString().c_str());
        }
        else
        {
            Console::Error::WriteLine("Context initialization failed.");
            return ExitCode::fail;
        }

        return ExitCode::ok;
    }
} // namespace OpenRCT2
